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

#include "natter_manager.h"

#include "manager/sola_helper.h"
#include "static_network_calculation.h"
#include "utils/random_engine.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::natter_ns3 {

#define TOPIC "TOPIC1"

NatterManager::NatterManager(const std::string &scenariofile_path)
    : scenariofile_(scenariofile_path) {}

uint64_t NatterManager::getNumberOfNodes() const { return scenariofile_.number_nodes; }

void NatterManager::setupImpl() {
  nodes_.Create(getNumberOfNodes());
  core_network_.addNodesCSMA(nodes_);

  daisi::registerNodes(nodes_);

  daisi::setupApplication<NatterApplication>(nodes_);

  // Set natter mode
  for (size_t i = 0; i < nodes_.GetN(); i++) {
    const NatterMode mode = natterModeFromString(scenariofile_.mode);
    getApplication(i)->setMode(mode);
  }

  scheduleEvents();
}

ns3::Ptr<NatterApplication> NatterManager::getApplication(uint32_t id) const {
  return nodes_.Get(id)->GetApplication(0)->GetObject<NatterApplication>();
}

NatterManager::NodeInfo NatterManager::getNodeInfo(uint32_t index) {
  uint16_t fanout = scenariofile_.fanout;
  const uint32_t own_level = natter_ns3::calculateLevel(index, fanout);
  const uint32_t own_number = natter_ns3::calculateNumber(index, fanout, own_level);
  const ns3::Ptr<NatterApplication> app = getApplication(index);
  const natter::NetworkInfoIPv4 net_info = app->getNetworkInfo();
  return {fanout, app->getUUID(), net_info.ip, net_info.port, own_level, own_number, app, index};
}

void NatterManager::addPeer(uint32_t other_id, const NodeInfo &info) {
  natter::minhcast::NatterMinhcast::NodeInfo natter_info;
  natter_info.position = {info.own_level, info.own_number, info.fanout};
  natter_info.network_info = {info.ip, info.port};
  natter_info.uuid = info.own_uuid;

  getApplication(other_id)->addPeer(TOPIC, natter_info);
}

void NatterManager::connectParent(const NodeInfo &info) {
  auto parent = static_cast<int64_t>(std::ceil(info.container_index / (double)info.fanout) - 1);
  if (parent >= 0) {
    addPeer(parent, info);
  }
}

std::set<uint32_t> NatterManager::connectChildren(const NodeInfo &info) {
  // Children
  std::set<uint32_t> children;
  for (uint32_t j = 1; j <= info.fanout; j++) {
    uint32_t child = info.container_index * info.fanout + j;
    if (child < getNumberOfNodes()) {
      addPeer(child, info);
      children.insert(child);
    }
  }
  return children;
}

std::set<uint32_t> NatterManager::connectNeighbors(const NodeInfo &info) {
  std::set<uint32_t> neighbors;

  auto get_neighbor_distance = [](const uint32_t d, const uint16_t fanout,
                                  const uint32_t i) -> uint32_t {
    return d * static_cast<uint32_t>(std::pow(fanout, i));  // [BATON* - Jagadish]
  };

  // Left neighbor
  for (uint32_t d = 1; d < info.fanout; d++) {
    int j = 0;
    int64_t current_neighbor = info.container_index - get_neighbor_distance(d, info.fanout, j);
    while (current_neighbor >= 0 &&
           natter_ns3::calculateLevel(current_neighbor, info.fanout) == info.own_level) {
      addPeer(current_neighbor, info);
      neighbors.insert(current_neighbor);
      j++;
      current_neighbor = info.container_index - get_neighbor_distance(d, info.fanout, j);
    }
  }

  // Right neighbors
  for (uint32_t d = 1; d < info.fanout; d++) {
    int j = 0;
    uint32_t current_neighbor = info.container_index + get_neighbor_distance(d, info.fanout, j);
    while (natter_ns3::calculateLevel(current_neighbor, info.fanout) == info.own_level &&
           current_neighbor < getNumberOfNodes()) {
      addPeer(current_neighbor, info);
      neighbors.insert(current_neighbor);
      j++;
      current_neighbor = info.container_index + get_neighbor_distance(d, info.fanout, j);
    }
  }
  return neighbors;
}

void NatterManager::connectAdjacents(const NodeInfo &info, const std::vector<uint32_t> &lin_proj) {
  auto it = std::find(lin_proj.begin(), lin_proj.end(), info.container_index);
  uint32_t own_list_index = it - lin_proj.begin();

  // Left
  if (own_list_index > 0) {
    uint32_t adjacent = lin_proj[own_list_index - 1];
    addPeer(adjacent, info);
  }

  // Right
  if (own_list_index < std::numeric_limits<uint32_t>::max() &&
      own_list_index + 1 < lin_proj.size()) {
    uint32_t adjacent = lin_proj[own_list_index + 1];
    addPeer(adjacent, info);
  }
}

void NatterManager::connectRoutingTableNeighborChildren(const NodeInfo &info,
                                                        const std::set<uint32_t> &children,
                                                        const std::set<uint32_t> &neighbors) {
  // Routing neighbor children
  for (uint32_t neighbor : neighbors) {
    for (uint32_t child : children) {
      const uint32_t child_level = natter_ns3::calculateLevel(child, info.fanout);
      const uint32_t child_number = natter_ns3::calculateNumber(child, info.fanout, child_level);

      const ns3::Ptr<NatterApplication> app_child = getApplication(child);
      const natter::NetworkInfoIPv4 child_net_info = app_child->getNetworkInfo();

      natter::minhcast::NatterMinhcast::NodeInfo natter_info;
      natter_info.position = {child_level, child_number, info.fanout};
      natter_info.network_info = {child_net_info.ip, child_net_info.port};
      natter_info.uuid = app_child->getUUID();

      getApplication(neighbor)->addPeer(TOPIC, natter_info);
    }
  }
}

void NatterManager::joinMinhton() {
  auto lin_proj = natter_ns3::createLinearProjection(getNumberOfNodes(), scenariofile_.fanout, 0);

  // Initialize nodes
  for (uint32_t i = 0; i < getNumberOfNodes(); i++) {
    NodeInfo info = getNodeInfo(i);

    // Only level, number and fanout of own peer is required.
    // Other information of the NodeInfo struct only required for adding other peers.
    natter::minhcast::NatterMinhcast::NodeInfo natter_info;
    natter_info.position = {info.own_level, info.own_number, info.fanout};

    info.app->subscribeTopic(TOPIC, natter_info);

    // Log all peers to database
    info.app->setLevelNumber({info.own_level, info.own_number});
  }

  // Create connections
  for (uint32_t i = 0; i < getNumberOfNodes(); i++) {
    NodeInfo info = getNodeInfo(i);

    // Add current node to all other nodes who should know this node
    connectParent(info);
    const std::set<uint32_t> children = connectChildren(info);
    const std::set<uint32_t> neighbors = connectNeighbors(info);
    if (natterModeFromString(scenariofile_.mode) == NatterMode::kMinhcast) {
      connectAdjacents(info, lin_proj);
      connectRoutingTableNeighborChildren(info, children, neighbors);
    }
  }
}

void NatterManager::publish(uint32_t message_size, uint32_t publishing_node) {
  if (publishing_node >= getNumberOfNodes())
    throw std::runtime_error("publishing node out of bounds");

  std::cout << "PUBLISH AT " << Simulator::Now().GetMicroSeconds() << " FROM " << publishing_node
            << std::endl;
  getApplication(publishing_node)->publish(TOPIC, std::string(message_size, 'a'));
}

void NatterManager::publishRandom(uint32_t message_size) {
  std::uniform_int_distribution<uint64_t> dist(0, getNumberOfNodes() - 1);
  uint32_t publishing_node = dist(daisi::global_random_engine);
  publish(message_size, publishing_node);
}

void NatterManager::joinTopic(int /*number*/) { throw std::runtime_error("Not implemented yet"); }

void NatterManager::scheduleEvent(const Join &step, ns3::Time &current_time) {
  const ns3::Time join_delay = scenariofile_.default_delay + step.delay.value_or(ns3::Time(0));
  current_time += join_delay;

  if (step.mode == "minhton") {
    if (scenariofile_.fanout < 2) throw std::runtime_error("fanout must be >=2 for minhton");
    Simulator::Schedule(current_time, &NatterManager::joinMinhton, this);
  } else {
    throw std::invalid_argument("Invalid mode type for join");
  }
}

void NatterManager::scheduleEvent(const Publish &step, ns3::Time &current_time) {
  const ns3::Time publish_delay = scenariofile_.default_delay + step.delay.value_or(ns3::Time(0));

  for (uint32_t i = 0; i < step.number; i++) {
    current_time += publish_delay;
    if (step.mode == "random")
      Simulator::Schedule(current_time, &NatterManager::publishRandom, this, step.message_size);
    else if (step.mode == "sequential")
      Simulator::Schedule(current_time, &NatterManager::publish, this, step.message_size,
                          i % getNumberOfNodes());
    else
      throw std::runtime_error("Invalid publish mode");
  }
}

void NatterManager::scheduleEvents() {
  ns3::Time current_time(0);

  for (const NatterScenarioSequenceStep &step : scenariofile_.scenario_sequence) {
    std::visit([this, &current_time](auto &&step) { scheduleEvent(step, current_time); },
               step.step);
  }
}

std::string NatterManager::getDatabaseFilename() const {
  return generateDBNameWithMinhtonInfo("natter", scenariofile_.fanout, getNumberOfNodes());
}

std::string NatterManager::getAdditionalParameters() const {
  return "NumberOfNodes=" + std::to_string(getNumberOfNodes());
}

}  // namespace daisi::natter_ns3
