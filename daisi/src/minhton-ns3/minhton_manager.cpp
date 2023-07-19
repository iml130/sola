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

#include "minhton_manager.h"

#include <cmath>

#include "minhton_application.h"
#include "minhton_manager_scheduler.h"
#include "minhton_node_ns3.h"
#include "minhton_scenariofile_helper.h"

using namespace minhton;
using namespace ns3;

namespace daisi::minhton_ns3 {

MinhtonManager::MinhtonManager(const std::string &scenariofile_path)
    : Manager<MinhtonApplication>(scenariofile_path) {
  Manager::initLogger();
  scheduler_ = std::make_shared<MinhtonManager::Scheduler>(*this);
}

void MinhtonManager::setup() {
  Manager<MinhtonApplication>::setup();
  setupNodeConfigurations();
}

void MinhtonManager::initNode(uint32_t id, minhton::ConfigNode config) {
  Ptr<MinhtonApplication> app =
      this->node_container_.Get(id)->GetApplication(0)->GetObject<MinhtonApplication>();

  app->initializeNode(config);
}

void MinhtonManager::setupNodeConfigurations() {
  std::string root_addr_string = getIpv4AddressString(this->interfaces_.GetAddress(0));

  auto algorithm_types_container =
      helper::toAlgorithmContainer(parser_.getTable<std::string>("algorithms"));
  auto timeout_lengths_container =
      helper::toTimeoutLengthsContainer(parser_.getTable<uint64_t>("timeouts"));

  for (uint32_t i = 0; i < getNumberOfNodes(); i++) {
    minhton::ConfigNode node_config;

    node_config.setIsRoot(i == 0);

    if (node_config.isRoot()) {
      node_config.setFanout(parser_.getFanout());
    } else {
      node_config.setJoinInfo({JoinInfo::kIp, root_addr_string, minhton::kDefaultIpPort});
    }

    node_config.setVerbose(true);
    node_config.setAlgorithmTypesContainer(algorithm_types_container);
    node_config.setTimeoutLengthsContainer(timeout_lengths_container);

    // Init after starting simulation
    ns3::Simulator::Schedule(ns3::MilliSeconds(1), &MinhtonManager::initNode, this, i, node_config);
  }
}

uint64_t MinhtonManager::getNumberOfNodes() {
  uint64_t current_number_of_nodes = 1;
  uint64_t max_num = current_number_of_nodes;
  uint64_t min_num = current_number_of_nodes;

  auto scenario_sequence = parser_.getScenarioSequence();
  for (std::shared_ptr<ScenariofileParser::Table> command : scenario_sequence) {
    auto map = command->content;

    auto it_join_many = map.find("join-many");
    if (it_join_many != map.end()) {
      auto cnodes = INNER_TABLE(it_join_many)->getRequired<uint64_t>("number");
      current_number_of_nodes += cnodes;

      max_num = std::max(max_num, current_number_of_nodes);
      continue;
    }

    auto it_build_static = map.find("static-build");
    if (it_build_static != map.end()) {
      auto cnodes = INNER_TABLE(it_build_static)->getRequired<uint64_t>("number");
      current_number_of_nodes += cnodes;

      max_num = std::max(max_num, current_number_of_nodes);
      continue;
    }

    auto it_join_one = map.find("join-one");
    if (it_join_one != map.end()) {
      current_number_of_nodes++;

      max_num = std::max(max_num, current_number_of_nodes);
      continue;
    }

    auto it_leave_many = map.find("leave-many");
    if (it_leave_many != map.end()) {
      auto cnodes = INNER_TABLE(it_leave_many)->getRequired<uint64_t>("number");
      current_number_of_nodes -= cnodes;

      min_num = std::min(min_num, current_number_of_nodes);
      continue;
    }

    auto it_leave_one = map.find("leave-one");
    if (it_leave_one != map.end()) {
      current_number_of_nodes--;

      min_num = std::min(min_num, current_number_of_nodes);
      continue;
    }

    auto it_mixed_execution = map.find("mixed-execution");
    if (it_mixed_execution != map.end()) {
      auto leave_nodes = INNER_TABLE(it_mixed_execution)->getRequired<uint64_t>("leave-number");
      auto join_nodes = INNER_TABLE(it_mixed_execution)->getRequired<uint64_t>("join-number");

      min_num = std::min(min_num, current_number_of_nodes - leave_nodes);
      max_num = std::max(max_num, current_number_of_nodes - join_nodes);

      current_number_of_nodes += join_nodes;
      current_number_of_nodes -= leave_nodes;

      continue;
    }
  }

  if (min_num < 1) {
    throw std::invalid_argument("There would be less than 1 node in the network at some point. "
                                "This is not possible. Too many nodes are leaving.");
  }

  return max_num;
}

std::string MinhtonManager::getDatabaseFilename() {
  return generateDBNameWithMinhtonInfo("minhton", parser_.getFanout(), this->getNumberOfNodes());
}

}  // namespace daisi::minhton_ns3
