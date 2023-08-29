// Copyright 2023 The SOLA authors
//
// This file is part of DAISI.
//
// DAISI is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation; version 2.
//
// DAISI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with DAISI. If not, see
// <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-2.0-only

#include <unordered_map>

#include "minhton/core/constants.h"
#include "minhton/exception/algorithm_exception.h"
#include "minhton_manager_scheduler.h"
#include "minhton_scenariofile_helper.h"
#include "ns3/double.h"
#include "ns3/simulator.h"
#include "utils/random_engine.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::minhton_ns3 {

void MinhtonManager::Scheduler::setupIndexQueues() {
  std::vector<uint64_t> temp(manager_.node_container_.GetN());
  std::iota(temp.begin(), temp.end(), 0);
  temp.erase(temp.begin());
  init_index_deque_.push_back(0);  // root is initialized

  std::shuffle(temp.begin(), temp.end(), daisi::global_random_engine);
  std::move(temp.begin(), temp.end(), std::back_inserter(uninit_index_deque_));
}

void MinhtonManager::Scheduler::setupRequestingNodes() {
  if (!requests_off_) {
    std::vector<std::string> all_ips;

    for (uint64_t i = 0; i < manager_.node_container_.GetN(); i++) {
      auto app =
          manager_.node_container_.Get(i)->GetApplication(0)->GetObject<MinhtonApplication>();

      all_ips.push_back(app->getNodeInfo().getAddress());
    }

    int requesting_nodes_size = 0;
    if (requests_.absolute_nodes_number) {
      requesting_nodes_size = *requests_.absolute_nodes_number;
    } else if (requests_.random_nodes_percentage) {
      requesting_nodes_size =
          std::ceil(manager_.getNumberOfNodes() * (*requests_.random_nodes_percentage));
    } else {
      throw std::invalid_argument("Not implemented");
    }

    std::shuffle(all_ips.begin(), all_ips.end(), daisi::global_random_engine);
    all_ips.resize(requesting_nodes_size);

    requesting_nodes_ips_ = all_ips;
  }
}

void MinhtonManager::Scheduler::scheduleMixedExecution(uint64_t join_num, uint64_t leave_num,
                                                       uint64_t search_num, uint64_t current_time,
                                                       uint64_t delay) {
  std::vector<uint8_t> random_sequence;
  std::vector<uint8_t> join_seq(join_num);
  std::vector<uint8_t> leave_seq(leave_num);
  std::vector<uint8_t> search_seq(search_num);

  std::fill(join_seq.begin(), join_seq.end(), 0);
  std::fill(leave_seq.begin(), leave_seq.end(), 1);
  std::fill(search_seq.begin(), search_seq.end(), 2);

  random_sequence.insert(random_sequence.end(), join_seq.begin(), join_seq.end());
  random_sequence.insert(random_sequence.end(), leave_seq.begin(), leave_seq.end());
  random_sequence.insert(random_sequence.end(), search_seq.begin(), search_seq.end());

  std::shuffle(std::begin(random_sequence), std::end(random_sequence), daisi::global_random_engine);

  for (auto const type : random_sequence) {
    current_time += delay;

    if (type == 0) {  // join
      Simulator::Schedule(MilliSeconds(current_time),
                          &MinhtonManager::Scheduler::executeOneRandomJoin, this);
    } else if (type == 1) {  // leave
      Simulator::Schedule(MilliSeconds(current_time),
                          &MinhtonManager::Scheduler::executeOneRandomLeave, this);
    } else {  // search
      Simulator::Schedule(MilliSeconds(current_time),
                          &MinhtonManager::Scheduler::executeOneRandomSearchExact, this);
    }
  }
}

void MinhtonManager::Scheduler::executeOneJoinByPosition(uint16_t level, uint16_t number) {
  std::cout << "\texecuteOneJoinByPosition on (" << level << ":" << number << ") at "
            << Simulator::Now().GetMilliSeconds() << std::endl;

  if (this->uninit_index_deque_.empty()) {
    throw std::logic_error("No uninit join to enter the network left");
  }

  bool found_initialized_node_at_pos = false;
  for (uint64_t join_to_index = 0; join_to_index < manager_.node_container_.GetN();
       join_to_index++) {
    auto app = manager_.node_container_.Get(join_to_index)
                   ->GetApplication(0)
                   ->GetObject<MinhtonApplication>();

    if (app->getNodeInfo().getLevel() == level && app->getNodeInfo().getNumber() == number) {
      if (app->getNodeInfo().isInitialized()) {
        found_initialized_node_at_pos = true;
        uint64_t uninit_index = this->uninit_index_deque_.front();
        this->initiateJoinNow(join_to_index, uninit_index);
        this->uninit_index_deque_.pop_front();
        this->init_index_deque_.push_back(uninit_index);
      } else {
        continue;
      }
    }
  }

  if (!found_initialized_node_at_pos) {
    throw std::invalid_argument("position to join to not found");
  }
}

void MinhtonManager::Scheduler::executeOneJoinByIndex(uint16_t join_to_index) {
  std::cout << "\texecuteOneJoinByIndex on index " << join_to_index << " at "
            << Simulator::Now().GetMilliSeconds() << std::endl;

  if (join_to_index >= manager_.node_container_.GetN()) {
    throw std::invalid_argument("Index to join to out of range");
  }

  if (this->uninit_index_deque_.empty()) {
    throw std::logic_error("No uninit join to enter the network left");
  }
  auto app = manager_.node_container_.Get(join_to_index)
                 ->GetApplication(0)
                 ->GetObject<MinhtonApplication>();

  if (app->getNodeInfo().isInitialized()) {
    uint64_t uninit_index = this->uninit_index_deque_.front();
    this->initiateJoinNow(join_to_index, uninit_index);
    this->uninit_index_deque_.pop_front();
    this->init_index_deque_.push_back(uninit_index);
  } else {
    std::cout << "node to join to found but is not initialized";
  }
}

void MinhtonManager::Scheduler::executeOneJoinOnRoot() {
  if (this->uninit_index_deque_.empty()) {
    throw std::logic_error("No uninit join to enter the network left");
  }

  uint64_t uninit_index = this->uninit_index_deque_.front();
  uint64_t root_index = this->getRootIndex();
  std::cout << "\texecuteOneJoinOnRoot from " << uninit_index << " to " << root_index << " at "
            << Simulator::Now().GetMilliSeconds() << std::endl;
  this->initiateJoinNow(root_index, uninit_index);
  this->uninit_index_deque_.pop_front();
  this->init_index_deque_.push_back(uninit_index);
}

void MinhtonManager::Scheduler::executeOneJoinDiscover() {
  throw std::runtime_error("currently not supported");
  // if (this->uninit_index_deque_.empty()) {
  //   throw std::logic_error("No uninit join to enter the network left");
  // }

  // uint64_t uninit_index = this->uninit_index_deque_.front();
  // std::cout << "\texecuteOneJoinDiscover from " << uninit_index << " at "
  //           << Simulator::Now().GetMilliSeconds() << std::endl;
  // this->initiateJoinNowDiscover(uninit_index);
  // this->uninit_index_deque_.pop_front();
  // this->init_index_deque_.push_back(uninit_index);
}

void MinhtonManager::Scheduler::Scheduler::executeOneRandomJoin() {
  if (this->uninit_index_deque_.empty()) {
    throw std::logic_error("No uninit join to enter the network left");
  }

  uint64_t uninit_index = this->uninit_index_deque_.front();

  // random index within [0; uininit_index-1]
  std::uniform_int_distribution<uint64_t> dist(0, init_index_deque_.size() - 1);
  uint64_t random_index_to_join_in_deque = dist(daisi::global_random_engine);
  uint64_t random_index_to_join = this->init_index_deque_.at(random_index_to_join_in_deque);

  std::cout << "\texecuteOneRandomJoin from " << uninit_index << " to " << random_index_to_join
            << " at " << Simulator::Now().GetMilliSeconds() << ", #"
            << this->init_index_deque_.size() << std::endl;
  this->initiateJoinNow(random_index_to_join, uninit_index);
  this->uninit_index_deque_.pop_front();
  this->init_index_deque_.push_back(uninit_index);
}

// Join via Multicast
void MinhtonManager::Scheduler::initiateJoinNowDiscover(uint64_t entering_node_index) {
  auto entering_app = manager_.node_container_.Get(entering_node_index)
                          ->GetApplication(0)
                          ->GetObject<MinhtonApplication>();

  entering_app->processSignal(
      minhton::Signal::joinNetworkViaBootstrap("225.1.2.4", minhton::kDefaultIpPort));

  initiatePeerDiscoverEnvironmentAfterJoin(entering_app);
}

void MinhtonManager::Scheduler::initiateJoinNow(uint64_t node_to_join_to_index,
                                                uint64_t entering_node_index) {
  auto app_to_join_to = manager_.node_container_.Get(node_to_join_to_index)
                            ->GetApplication(0)
                            ->GetObject<MinhtonApplication>();
  // std::string node_to_join_to_addr_string =
  //     getIpv4AddressString(manager_.interfaces_.GetAddress(node_to_join_to_index));
  const minhton::NodeInfo nodeinfo_to_join_to = app_to_join_to->getNodeInfo();

  auto entering_app = manager_.node_container_.Get(entering_node_index)
                          ->GetApplication(0)
                          ->GetObject<MinhtonApplication>();
  entering_app->processSignal(minhton::Signal::joinNetworkViaNodeInfo(nodeinfo_to_join_to));

  initiatePeerDiscoverEnvironmentAfterJoin(entering_app);
}

void MinhtonManager::Scheduler::executeOneLeaveByPosition(uint16_t level, uint16_t number) {
  std::cout << "\texecuteOneLeaveByPosition on (" << level << ":" << number << ") at "
            << Simulator::Now().GetMilliSeconds() << std::endl;

  for (uint64_t leave_index = 0; leave_index < manager_.node_container_.GetN(); leave_index++) {
    auto app = manager_.node_container_.Get(leave_index)
                   ->GetApplication(0)
                   ->GetObject<MinhtonApplication>();

    if (app->getNodeInfo().getLevel() == level && app->getNodeInfo().getNumber() == number) {
      if (app->getNodeInfo().isInitialized()) {
        this->initiateLeaveNow(leave_index);

        for (auto it = this->init_index_deque_.begin(); it != this->init_index_deque_.end(); it++) {
          if (leave_index == *it) {
            this->init_index_deque_.erase(it);
            break;
          }
        }
        this->uninit_index_deque_.push_back(leave_index);
        return;
      }
    }
  }

  throw std::invalid_argument("position to leave to not found");
}

void MinhtonManager::Scheduler::executeOneLeaveByIndex(uint16_t index) {
  std::cout << "\texecuteOneLeaveByIndex on index " << index << " at "
            << Simulator::Now().GetMilliSeconds() << std::endl;

  if (index >= manager_.node_container_.GetN()) {
    throw std::invalid_argument("Index to leave to out of range");
  }

  auto app =
      manager_.node_container_.Get(index)->GetApplication(0)->GetObject<MinhtonApplication>();

  if (app->getNodeInfo().isInitialized()) {
    this->initiateLeaveNow(index);
    for (auto it = this->init_index_deque_.begin(); it != this->init_index_deque_.end(); it++) {
      if (index == *it) {
        this->init_index_deque_.erase(it);
        break;
      }
    }
    this->uninit_index_deque_.push_back(index);
  } else {
    std::cout << "node to leave to found but is not initialized";
  }
}

void MinhtonManager::Scheduler::executeOneRandomLeave() {
  if (this->init_index_deque_.empty()) {
    throw std::logic_error("No uninit join to leave the network left");
  }

  std::uniform_int_distribution<uint64_t> dist(0, init_index_deque_.size() - 1);
  uint64_t random_index_to_leave_in_deque = dist(daisi::global_random_engine);
  uint64_t random_index_to_leave = this->init_index_deque_.at(random_index_to_leave_in_deque);

  std::cout << "\texecuteOneRandomLeave from " << random_index_to_leave << " at "
            << Simulator::Now().GetMilliSeconds() << ", #" << this->init_index_deque_.size() - 1
            << std::endl;
  this->initiateLeaveNow(random_index_to_leave);

  for (auto it = this->init_index_deque_.begin(); it != this->init_index_deque_.end(); it++) {
    if (random_index_to_leave == *it) {
      this->init_index_deque_.erase(it);
      break;
    }
  }
  this->uninit_index_deque_.push_back(random_index_to_leave);
}

void MinhtonManager::Scheduler::executeOneLeaveOnRoot() {
  uint64_t root_index = this->getRootIndex();
  std::cout << "\texecuteOneLeaveOnRoot on " << root_index << " at "
            << Simulator::Now().GetMilliSeconds() << std::endl;
  this->initiateLeaveNow(root_index);

  // finding root_index in deque and erasing
  for (auto it = this->init_index_deque_.begin(); it != this->init_index_deque_.end(); it++) {
    if (root_index == *it) {
      this->init_index_deque_.erase(it);
      break;
    }
  }
  this->uninit_index_deque_.push_back(root_index);
}

void MinhtonManager::Scheduler::initiateLeaveNow(uint64_t node_to_leave_to_index) {
  auto leaving_app = manager_.node_container_.Get(node_to_leave_to_index)
                         ->GetApplication(0)
                         ->GetObject<MinhtonApplication>();
  leaving_app->processSignal(minhton::Signal::leaveNetwork());
}

void MinhtonManager::Scheduler::initiateFailureNow(uint64_t node_to_fail_to_index) {
  auto failing_app = manager_.node_container_.Get(node_to_fail_to_index)
                         ->GetApplication(0)
                         ->GetObject<MinhtonApplication>();

  std::cout << "\tinitiateFailureNow on index " << node_to_fail_to_index << " at "
            << Simulator::Now().GetMilliSeconds() << std::endl;
  failing_app->StopApplication();
}

void MinhtonManager::Scheduler::scheduleSearchExactAll(uint64_t delay) {
  uint64_t counter = 0;

  for (uint64_t i = 0; i < manager_.node_container_.GetN(); i++) {
    auto app_i =
        manager_.node_container_.Get(i)->GetApplication(0)->GetObject<MinhtonApplication>();
    minhton::NodeInfo node_info_i = app_i->getNodeInfo();
    if (node_info_i.isInitialized()) {
      for (uint64_t k = 0; k < manager_.node_container_.GetN(); k++) {
        minhton::NodeInfo node_info_k = manager_.node_container_.Get(k)
                                            ->GetApplication(0)
                                            ->GetObject<MinhtonApplication>()
                                            ->getNodeInfo();
        if ((i != k) && node_info_k.isInitialized()) {
          uint64_t current_delay = ++counter * delay;
          Simulator::Schedule(MilliSeconds(current_delay),
                              &MinhtonManager::Scheduler::executeOneSearchExact, this, app_i,
                              node_info_k.getLevel(), node_info_k.getNumber());
        }
      }
    }
  }
}

void MinhtonManager::Scheduler::scheduleSearchExactMany(uint64_t delay, uint16_t number) {
  uint64_t current_delay = 0;

  auto existing_positions_tuple = this->getExistingPositions();
  auto existing_positions = std::get<1>(existing_positions_tuple);
  // auto existing_indices = std::get<0>(existing_positions_tuple);

  assert(number <= existing_positions.size() * (existing_positions.size() - 1));

  std::uniform_int_distribution<uint64_t> distrib(0, existing_positions.size() - 1);

  uint16_t count = 0;
  while (count < number) {
    uint64_t start = distrib(daisi::global_random_engine);
    uint64_t end = distrib(daisi::global_random_engine);

    if (start != end) {
      current_delay += delay;
      count++;

      auto app_start =
          manager_.node_container_.Get(start)->GetApplication(0)->GetObject<MinhtonApplication>();
      uint16_t node_end_level = std::get<0>(existing_positions[end]);
      uint16_t node_end_number = std::get<1>(existing_positions[end]);

      Simulator::Schedule(MilliSeconds(current_delay),
                          &MinhtonManager::Scheduler::executeOneSearchExact, this, app_start,
                          node_end_level, node_end_number);
    }
  }
}

void MinhtonManager::Scheduler::executeOneSearchExact(Ptr<MinhtonApplication> src_app,
                                                      uint32_t dest_level, uint32_t dest_number) {
  std::cout << "\texecuteOneSearchExact to " << dest_level << ":" << dest_number << " at "
            << Simulator::Now().GetMilliSeconds() << std::endl;
  if (src_app->getNodeInfo().isInitialized()) {
    src_app->executeSearchExactTest(dest_level, dest_number);
  }
}

void MinhtonManager::Scheduler::executeOneRandomSearchExact() {
  std::cout << "\texecuteOneRandomSearchExact" << std::endl;

  std::uniform_int_distribution<uint64_t> dist(0, init_index_deque_.size() - 1);
  uint64_t rand_1 = dist(daisi::global_random_engine);
  uint64_t rand_2 = dist(daisi::global_random_engine);

  while (rand_1 == rand_2) {
    rand_2 = dist(daisi::global_random_engine);
  }

  auto app_1 = manager_.node_container_.Get(this->init_index_deque_.at(rand_1))
                   ->GetApplication(0)
                   ->GetObject<MinhtonApplication>();

  auto app_2 = manager_.node_container_.Get(this->init_index_deque_.at(rand_2))
                   ->GetApplication(0)
                   ->GetObject<MinhtonApplication>();

  minhton::NodeInfo node_info_2 = app_2->getNodeInfo();
  app_1->executeSearchExactTest(node_info_2.getLevel(), node_info_2.getNumber());
}

void MinhtonManager::Scheduler::executeOneFailByPosition(uint16_t level, uint16_t number) {
  std::cout << "\texecuteOneFailByPosition on (" << level << ":" << number << ") at "
            << Simulator::Now().GetMilliSeconds() << std::endl;

  for (uint64_t fail_index = 0; fail_index < manager_.node_container_.GetN(); fail_index++) {
    auto app = manager_.node_container_.Get(fail_index)
                   ->GetApplication(0)
                   ->GetObject<MinhtonApplication>();

    if (app->getNodeInfo().getLevel() == level && app->getNodeInfo().getNumber() == number) {
      if (app->getNodeInfo().isInitialized()) {
        this->initiateFailureNow(fail_index);

        for (auto it = this->init_index_deque_.begin(); it != this->init_index_deque_.end(); it++) {
          if (fail_index == *it) {
            this->init_index_deque_.erase(it);
            break;
          }
        }
        this->uninit_index_deque_.push_back(fail_index);
        return;
      }
    }
  }

  throw std::invalid_argument("position to fail to not found");
}

void MinhtonManager::Scheduler::executeOneRandomFail() {
  if (this->init_index_deque_.empty()) {
    throw std::logic_error("No uninit join to fail in the network left");
  }

  uint64_t index_to_fail = this->init_index_deque_.front();

  std::cout << "\texecuteOneRandomFail from " << index_to_fail << " at "
            << Simulator::Now().GetMilliSeconds() << std::endl;
  this->initiateFailureNow(index_to_fail);

  this->init_index_deque_.pop_front();
  this->uninit_index_deque_.push_back(index_to_fail);
}

uint64_t MinhtonManager::Scheduler::getRootIndex() {
  minhton::NodeInfo previous_root_node_info =
      manager_.node_container_.Get(this->latest_root_index_)
          ->GetApplication(0)
          ->GetObject<MinhtonApplication>()
          ->getNodeInfo();
  if ((previous_root_node_info.getLevel() == 0) && (previous_root_node_info.getNumber() == 0) &&
      previous_root_node_info.isInitialized()) {
    return this->latest_root_index_;
  }

  for (uint64_t i = 0; i < manager_.node_container_.GetN(); i++) {
    minhton::NodeInfo node_info = manager_.node_container_.Get(i)
                                      ->GetApplication(0)
                                      ->GetObject<MinhtonApplication>()
                                      ->getNodeInfo();
    if ((node_info.getLevel() == 0) && (node_info.getNumber() == 0) && node_info.isInitialized()) {
      this->latest_root_index_ = i;
      return i;
    }
  }

  throw std::logic_error("No initialized root node found. Manager-Scheduling Error");
}

void MinhtonManager::Scheduler::scheduleValidateLeave(uint64_t delay) {
  for (uint64_t i = 0; i < manager_.getNumberOfNodes(); i++) {
    auto app = manager_.node_container_.Get(i)->GetApplication(0)->GetObject<MinhtonApplication>();
    const minhton::NodeInfo node_info = app->getNodeInfo();

    if (node_info.isInitialized()) {
      Simulator::Schedule(MilliSeconds((2 * i + 1) * delay),
                          &MinhtonManager::Scheduler::executeOneLeaveByIndex, this, i);
      Simulator::Schedule(MilliSeconds((2 * i + 2) * delay),
                          &MinhtonManager::Scheduler::executeOneRandomJoin, this);
    }
  }
}

std::tuple<std::vector<uint64_t>, std::vector<std::tuple<uint16_t, uint16_t>>>
MinhtonManager::Scheduler::getExistingPositions() {
  std::vector<std::tuple<uint16_t, uint16_t>> positions;
  std::vector<uint64_t> indices;

  for (uint64_t i = 0; i < manager_.getNumberOfNodes(); i++) {
    minhton::NodeInfo node_info = manager_.node_container_.Get(i)
                                      ->GetApplication(0)
                                      ->GetObject<MinhtonApplication>()
                                      ->getNodeInfo();

    if (node_info.isInitialized()) {
      positions.push_back(
          std::make_tuple<uint16_t, uint16_t>(node_info.getLevel(), node_info.getNumber()));
      indices.push_back(i);
    }
  }

  return std::make_tuple(indices, positions);
}

Ptr<MinhtonApplication> MinhtonManager::Scheduler::getApplicationAtPosition(uint16_t level,
                                                                            uint16_t number) {
  for (uint64_t i = 0; i < manager_.getNumberOfNodes(); i++) {
    auto app = manager_.node_container_.Get(i)->GetApplication(0)->GetObject<MinhtonApplication>();
    minhton::NodeInfo node_info = app->getNodeInfo();

    if (node_info.isInitialized() && node_info.getLevel() == level &&
        node_info.getNumber() == number) {
      return app;
    }
  }
  throw std::runtime_error("No node found at given location");
}

void MinhtonManager::Scheduler::executeFindQuery(minhton::FindQuery query) {
  Ptr<MinhtonApplication> app = this->getApplicationAtPosition(
      query.getRequestingNode().getLevel(), query.getRequestingNode().getNumber());
  if (app) {
    app->executeFindQuery(query);
  }
}

void MinhtonManager::Scheduler::setupRootBehavior() {
  Ptr<MinhtonApplication> app = getApplicationAtPosition(0, 0);
  initiateNodeContent(app);
}

void MinhtonManager::Scheduler::executeStaticNetworkBuild(uint32_t number) {
  uint16_t fanout = manager_.parser_.getFanout();
  uint32_t max_nodes = number + 1;  // with root

  auto calc_index = [&](const uint32_t &l, const uint32_t &n) {
    if (l == 0) {
      return 0U;
    }
    return (uint32_t)(((1 - pow(fanout, l)) / (1 - fanout)) + n);
  };
  // auto calc_index = [&](const uint32_t &l, const uint32_t &n) { return pow(fanout, l) - 1 + n; };

  manager_.node_container_.Get(0)
      ->GetApplication(0)
      ->GetObject<MinhtonApplication>()
      ->getNodeInfo();

  // index -> NodeInfo, Neighbors, AdjLeft, AdjRight
  std::unordered_map<uint64_t, std::tuple<minhton::NodeInfo, std::vector<minhton::NodeInfo>,
                                          minhton::NodeInfo, minhton::NodeInfo>>
      info;
  info.reserve(max_nodes);

  // get node info objects without positions
  for (uint64_t i = 0; i < manager_.node_container_.GetN(); i++) {
    auto app = manager_.node_container_.Get(i)->GetApplication(0)->GetObject<MinhtonApplication>();
    info[i] = {app->getNodeInfo(), {}, minhton::NodeInfo(), minhton::NodeInfo()};
  }

  // setting positions, left to right
  uint64_t i = 0;
  bool stop = false;
  for (uint32_t l = 0;; l++) {
    for (uint32_t n = 0; n < pow(fanout, l); n++) {
      i = calc_index(l, n);
      if (i > max_nodes - 1) {
        stop = true;
        break;
      }

      std::get<0>(info[i]).setLogicalNodeInfo(minhton::LogicalNodeInfo(l, n, fanout));
    }
    if (stop) break;
  }

  i = 0;
  stop = false;
  for (uint32_t l = 0;; l++) {
    for (uint32_t n = 0; n < pow(fanout, l); n++) {
      i = calc_index(l, n);
      if (i > max_nodes - 1) {
        stop = true;
        break;
      }

      // parent if exists
      if (l > 0) {
        auto parent_pos = minhton::calcParent(l, n, fanout);
        auto parent_index = calc_index(std::get<0>(parent_pos), std::get<1>(parent_pos));
        if (parent_index < max_nodes) {
          auto parent_node = std::get<0>(info[parent_index]);
          std::get<1>(info[i]).push_back(parent_node);
        }
      }

      // children if exist
      auto child_positions = minhton::calcChildren(l, n, fanout);
      for (auto const &cp : child_positions) {
        auto child_index = calc_index(std::get<0>(cp), std::get<1>(cp));
        if (child_index < max_nodes) {
          auto child_node = std::get<0>(info[child_index]);
          std::get<1>(info[i]).push_back(child_node);
        }
      }

      // rt neighbors and children
      auto rt_positions = minhton::calcLeftRT(l, n, fanout);
      auto right_rt_positions = minhton::calcRightRT(l, n, fanout);
      rt_positions.insert(rt_positions.end(), right_rt_positions.begin(), right_rt_positions.end());
      for (auto const &p : rt_positions) {
        auto neighbor_index = calc_index(std::get<0>(p), std::get<1>(p));
        if (neighbor_index < max_nodes) {
          auto neighbor_node = std::get<0>(info[neighbor_index]);
          std::get<1>(info[i]).push_back(neighbor_node);

          // rt neighbor children
          auto neighbor_children_positions =
              minhton::calcChildren(std::get<0>(p), std::get<1>(p), fanout);
          for (auto const &c : neighbor_children_positions) {
            auto child_index = calc_index(std::get<0>(c), std::get<1>(c));
            if (child_index < max_nodes) {
              auto child_node = std::get<0>(info[child_index]);
              std::get<1>(info[i]).push_back(child_node);
            }
          }
        }
      }
    }
    if (stop) break;
  }

  // calc adjacents
  std::vector<std::tuple<uint64_t, minhton::NodeInfo>> index_node;
  auto take_index_and_node =
      [](std::pair<const uint64_t, std::tuple<minhton::NodeInfo, std::vector<minhton::NodeInfo>,
                                              minhton::NodeInfo, minhton::NodeInfo>> &pair) {
        return std::make_tuple(pair.first, std::get<0>(pair.second));
      };
  std::transform(info.begin(), info.end(), std::back_inserter(index_node), take_index_and_node);

  auto sort_by_horizontal_pos = [](std::tuple<uint64_t, minhton::NodeInfo> &tupl1,
                                   std::tuple<uint64_t, minhton::NodeInfo> &tupl2) {
    return std::get<1>(tupl1).getLogicalNodeInfo().getHorizontalValue() <
           std::get<1>(tupl2).getLogicalNodeInfo().getHorizontalValue();
  };
  std::sort(index_node.begin(), index_node.end(), sort_by_horizontal_pos);

  // setting adjacents
  for (uint64_t sort_index = 1; sort_index < index_node.size() - 1; sort_index++) {
    auto adj_left = std::get<1>(index_node[sort_index - 1]);
    auto adj_right = std::get<1>(index_node[sort_index + 1]);

    uint64_t info_index = std::get<0>(index_node[sort_index]);
    std::get<2>(info[info_index]) = adj_left;
    std::get<3>(info[info_index]) = adj_right;
  }

  // leftmost has only adj right
  uint64_t leftmost_index = std::get<0>(index_node[0]);
  std::get<3>(info[leftmost_index]) = std::get<1>(index_node[1]);

  // rightmost has only adj left
  uint64_t rightmost_index = std::get<0>(index_node[index_node.size() - 1]);
  std::get<2>(info[rightmost_index]) = std::get<1>(index_node[index_node.size() - 2]);

  // give positions to MINHTON app
  for (uint64_t j = 0; j < manager_.node_container_.GetN(); j++) {
    auto app = manager_.node_container_.Get(j)->GetApplication(0)->GetObject<MinhtonApplication>();
    auto our_info = info[j];
    if (j > 0) {
      initiatePeerDiscoverEnvironmentAfterStaticBuild(app, j);
    }
    app->setStaticBuildNeighbors(std::get<0>(our_info), std::get<1>(our_info),
                                 std::get<2>(our_info), std::get<3>(our_info));
  }
}

void MinhtonManager::Scheduler::initiatePeerDiscoverEnvironment(Ptr<MinhtonApplication> app,
                                                                uint64_t init_content_delay,
                                                                uint64_t init_request_delay) {
  if (!attributes_off_) {
    if (init_content_delay == 0) {
      initiateNodeContent(app);
    } else {
      Simulator::Schedule(MilliSeconds(init_content_delay),
                          &MinhtonManager::Scheduler::initiateNodeContent, this, app);
    }
  }

  if (!requests_off_) {
    std::string ip = app->getNodeInfo().getAddress();
    bool is_requesting = std::find(requesting_nodes_ips_.begin(), requesting_nodes_ips_.end(),
                                   ip) != requesting_nodes_ips_.end();

    if (is_requesting) {
      if (init_request_delay == 0) {
        makeRequest(app);
      } else {
        Simulator::Schedule(MilliSeconds(init_request_delay),
                            &MinhtonManager::Scheduler::makeRequest, this, app);
      }
    }
  }
}

void MinhtonManager::Scheduler::initiatePeerDiscoverEnvironmentAfterStaticBuild(
    Ptr<MinhtonApplication> app, uint64_t index) {
  uint64_t request_delay_diff = 500 * (index + 1);

  initiatePeerDiscoverEnvironment(app, 1, request_delay_diff);
}

void MinhtonManager::Scheduler::initiatePeerDiscoverEnvironmentAfterJoin(
    Ptr<MinhtonApplication> app) {
  auto container =
      helper::toTimeoutLengthsContainer(manager_.parser_.getTable<uint64_t>("timeouts"));
  uint64_t content_delay = container.join_response + container.join_accept_ack_response;
  initiatePeerDiscoverEnvironment(app, content_delay, content_delay + 100);
}

void MinhtonManager::Scheduler::initiateNodeContent(Ptr<MinhtonApplication> app) {
  std::vector<minhton::Entry> insert_entries;

  for (auto &node_attr : node_content_attributes_) {
    auto rand_val = uniform_zero_one_distribution_(daisi::global_random_engine);
    if (rand_val > node_attr.presence_percentage) {
      continue;
    }

    minhton::NodeData::Value initial_value = node_attr.getRandomContent();
    minhton::NodeData::ValueType value_type = node_attr.value_type;

    insert_entries.push_back({node_attr.name, initial_value, value_type});

    if (value_type == minhton::NodeData::ValueType::kValueDynamic) {
      uint64_t update_delay = node_attr.update_delay_distribution(daisi::global_random_engine);
      Simulator::Schedule(MilliSeconds(update_delay),
                          &MinhtonManager::Scheduler::updateNodeAttribute, this, app,
                          node_attr.name);
    }
  }

  app->localTestDataInsert(insert_entries);
}

void MinhtonManager::Scheduler::updateNodeAttribute(Ptr<MinhtonApplication> app,
                                                    minhton::NodeData::Key key) {
  auto it = std::find_if(node_content_attributes_.begin(), node_content_attributes_.end(),
                         [&](const NodeAttribute &attr) { return attr.name == key; });
  if (it == node_content_attributes_.end()) {
    throw std::logic_error("shouldnt happen");
  }

  minhton::NodeData::Value update_value = it->getRandomContent();

  try {
    app->localTestDataUpdate({{key, update_value, it->value_type}});
    uint64_t next_update_delay = it->update_delay_distribution(daisi::global_random_engine);
    Simulator::Schedule(MilliSeconds(next_update_delay),
                        &MinhtonManager::Scheduler::updateNodeAttribute, this, app, it->name);
  } catch (const minhton::AlgorithmException &e) {
  };
}

void MinhtonManager::Scheduler::initiateRequestsOnAllNodes() {
  for (uint64_t i = 0; i < manager_.node_container_.GetN(); i++) {
    auto app = manager_.node_container_.Get(i)->GetApplication(0)->GetObject<MinhtonApplication>();

    uint64_t request_delay = requests_.request_delay_distribution(daisi::global_random_engine);
    Simulator::Schedule(MilliSeconds(request_delay), &MinhtonManager::Scheduler::makeRequest, this,
                        app);
  }
}

void MinhtonManager::Scheduler::makeRequest(Ptr<MinhtonApplication> app) {
  uint8_t depth = requests_.query_depth_distribution(daisi::global_random_engine);
  auto query = createFindQuery(depth);

  if (requests_limit_on_) {
    requests_counter_++;
    std::cout << "REQUEST-COUNTER " << requests_counter_ << " ";
  }

  app->executeFindQuery(query);

  if (requests_limit_on_) {
    if (requests_limit_ <= requests_counter_) {
      uint64_t stop_delay = 10000;
      ns3::Simulator::Stop(ns3::MilliSeconds(stop_delay));
      std::cout << "STOPPING SOON" << std::endl;
    }
  }

  uint64_t next_request_delay = requests_.request_delay_distribution(daisi::global_random_engine);
  Simulator::Schedule(MilliSeconds(next_request_delay), &MinhtonManager::Scheduler::makeRequest,
                      this, app);
}

minhton::FindQuery MinhtonManager::Scheduler::createFindQuery(uint8_t depth) {
  minhton::FindQuery query;

  query.setBooleanExpression(createBooleanExpression(depth));
  query.setScope(minhton::FindQuery::FindQueryScope::kAll);

  query.setInquireOutdatedAttributes(requests_.query_inquire_outdated);
  query.setInquireUnknownAttributes(requests_.query_inquire_unknown);

  query.setValidityThreshold(
      requests_.validity_threshold_distribution(daisi::global_random_engine));

  return query;
}

std::shared_ptr<minhton::BooleanExpression> MinhtonManager::Scheduler::createBooleanExpression(
    uint8_t depth) {
  std::vector<std::string> temp(attribute_names_);
  std::shuffle(temp.begin(), temp.end(), daisi::global_random_engine);
  if (temp.size() > depth) {
    temp.resize(depth);
  }

  auto result = createBooleanExpressionRecursive(temp);
  assert(result->getDepth() == temp.size());

  return result;
}

std::shared_ptr<minhton::BooleanExpression>
MinhtonManager::Scheduler::createBooleanExpressionRecursive(
    std::vector<minhton::NodeData::Key> open_keys) {
  uint8_t size_keys = open_keys.size();
  float not_expr_threshold = 0.2;

  auto rand_val0 = uniform_zero_one_distribution_(daisi::global_random_engine);
  auto rand_val1 = uniform_zero_one_distribution_(daisi::global_random_engine);
  auto rand_val2 = uniform_zero_one_distribution_(daisi::global_random_engine);

  if (size_keys == 1) {
    auto x0 = createBooleanExpressionForKey(open_keys[0]);
    auto x0n = rand_val1 <= not_expr_threshold ? std::make_shared<minhton::NotExpression>(x0) : x0;
    return x0n;
  }

  std::shared_ptr<minhton::BooleanExpression> x0;
  std::shared_ptr<minhton::BooleanExpression> x1;

  if (size_keys == 2) {
    x0 = createBooleanExpressionForKey(open_keys[0]);
    x1 = createBooleanExpressionForKey(open_keys[1]);

  } else {
    std::vector<minhton::NodeData::Key> first_open_keys(open_keys.begin(),
                                                        open_keys.begin() + size_keys / 2);
    std::vector<minhton::NodeData::Key> second_open_keys(open_keys.begin() + size_keys / 2 + 1,
                                                         open_keys.end());

    x0 = createBooleanExpressionRecursive(first_open_keys);
    x1 = createBooleanExpressionRecursive(second_open_keys);
  }

  auto x0n = rand_val1 <= not_expr_threshold ? std::make_shared<minhton::NotExpression>(x0) : x0;
  auto x1n = rand_val2 <= not_expr_threshold ? std::make_shared<minhton::NotExpression>(x1) : x1;

  if (rand_val0 <= 0.5) {
    return std::make_shared<minhton::AndExpression>(x0n, x1n);
  }
  return std::make_shared<minhton::OrExpression>(x0n, x1n);
}

std::shared_ptr<minhton::BooleanExpression>
MinhtonManager::Scheduler::createBooleanExpressionForKey(minhton::NodeData::Key key) {
  auto attribute_it = std::find_if(node_content_attributes_.begin(), node_content_attributes_.end(),
                                   [&](const NodeAttribute &attr) { return attr.name == key; });
  if (attribute_it == node_content_attributes_.end()) {
    throw std::invalid_argument("unknown key");
  }

  if (attribute_it->content_is_numerical) {
    // numeric -> comparison expression

    float comp_value = attribute_it->content_distribution->GetValue();
    if (comp_value == attribute_it->content_distribution->GetValue()) {
      // constant numeric
      return std::make_shared<minhton::NumericComparisonExpression<float>>(
          key, minhton::ComparisonTypes::kEqualTo, comp_value);
    }

    // gaussian or uniform numeric
    auto rand_val = uniform_zero_one_distribution_(daisi::global_random_engine);

    minhton::ComparisonTypes comp_type = minhton::kEqualTo;
    rand_val *= 4;  // [0,4]
    if (rand_val <= 1) {
      comp_type = minhton::ComparisonTypes::kLessThan;
    } else if (rand_val <= 2) {
      comp_type = minhton::ComparisonTypes::kGreater;
    } else if (rand_val <= 3) {
      comp_type = minhton::ComparisonTypes::kLessThanOrEqualTo;
    } else {
      comp_type = minhton::ComparisonTypes::kGreaterThanOrEqualTo;
    }
    // equal to and not equal to does not make sense for floats and distributions

    return std::make_shared<minhton::NumericComparisonExpression<float>>(key, comp_type,
                                                                         comp_value);
  }

  uint8_t rand_choice = attribute_it->content_distribution->GetInteger();
  auto rand_value = attribute_it->empirical_value_mapping[rand_choice];

  if (auto pval = std::get_if<std::string>(&rand_value)) {
    // empirical string distribution
    return std::make_shared<minhton::StringEqualityExpression>(key, *pval);
  }

  if (auto pval = std::get_if<int>(&rand_value)) {
    // constant distribution
    return std::make_shared<minhton::NumericComparisonExpression<int>>(
        key, minhton::ComparisonTypes::kEqualTo, *pval);
  }

  if (auto pval = std::get_if<float>(&rand_value)) {
    // constant distribution
    return std::make_shared<minhton::NumericComparisonExpression<float>>(
        key, minhton::ComparisonTypes::kEqualTo, *pval);
  }

  if (std::get_if<bool>(&rand_value) != nullptr && *std::get_if<bool>(&rand_value)) {
    throw std::logic_error("not implemented yet");
  }

  throw std::logic_error("invalid type");
}

void MinhtonManager::Scheduler::activateRequestCountdown(uint16_t limit) {
  requests_limit_ = limit;
  requests_limit_on_ = true;

  // Ref #102
  throw std::runtime_error("Logging currently not implemented for request-countdown");
  // LOG_EVENT(minhton::EventType::kRequestCountdownStart, 0);
}

}  // namespace daisi::minhton_ns3
