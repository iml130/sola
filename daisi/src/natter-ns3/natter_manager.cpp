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

#include "static_network_calculation.h"
#include "utils/random_engine.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::natter_ns3 {

#define TOPIC "TOPIC1"

NatterManager::NatterManager(const std::string &scenariofile_path)
    : Manager<NatterApplication>(scenariofile_path),
      mode_(natterModeFromString(parser_.getParsedContent()->getRequired<std::string>("mode"))) {
  Manager::initLogger();
}

uint64_t NatterManager::getNumberOfNodes() {
  return parser_.getParsedContent()->getRequired<uint64_t>("numberNodes");
}

void NatterManager::setup() {
  Manager<NatterApplication>::setup();

  // Set natter mode
  for (auto node = nodeContainer_.Begin(); node != nodeContainer_.End(); node++) {
    (*node)->GetApplication(0)->GetObject<NatterApplication>()->setMode(mode_);
  }
}

std::pair<ns3::Ptr<NatterNodeNs3>, ns3::Ptr<NatterApplication>> NatterManager::getNodes(
    uint32_t id) const {
  auto app = nodeContainer_.Get(id)->GetApplication(0)->GetObject<NatterApplication>();
  return {app->getNatterNode(), app};
}

NatterManager::NodeInfo NatterManager::getNodeInfo(uint32_t index) {
  uint64_t fanout = parser_.getFanout();
  const uint32_t own_level = natter_ns3::calculateLevel(index, fanout);
  const uint32_t own_number = natter_ns3::calculateNumber(index, fanout, own_level);
  auto [node, app] = getNodes(index);
  return {fanout, node->getUUID(), app->getIP(), app->getPort(), own_level, own_number, node, app,
          index};
}

void NatterManager::addPeer(uint32_t other_id, const NodeInfo &info) {
  getNatterNodeByID(other_id)->addPeer(TOPIC, info.own_uuid, info.ip, info.port, info.own_level,
                                       info.own_number, info.fanout);
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

  auto get_neighbor_distance = [](const uint32_t d, const uint64_t fanout,
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

      auto [node_child, app_child] = getNodes(child);
      std::string child_ip = app_child->getIP();
      uint16_t child_port = app_child->getPort();
      getNatterNodeByID(neighbor)->addPeer(TOPIC, node_child->getUUID(), child_ip, child_port,
                                           child_level, child_number, info.fanout);
    }
  }
}

void NatterManager::joinMinhton() {
  auto lin_proj = natter_ns3::createLinearProjection(getNumberOfNodes(), parser_.getFanout(), 0);

  // Initialize nodes
  for (uint32_t i = 0; i < getNumberOfNodes(); i++) {
    NodeInfo info = getNodeInfo(i);

    info.node->subscribeTopic(TOPIC, info.own_level, info.own_number, info.fanout);

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
    if (mode_ == NatterMode::kMinhcast) {
      connectAdjacents(info, lin_proj);
      connectRoutingTableNeighborChildren(info, children, neighbors);
    }
  }
}

ns3::Ptr<NatterNodeNs3> NatterManager::getNatterNodeByID(uint32_t id) const {
  auto [node, app] = getNodes(id);
  return node;
}

void NatterManager::publish(uint32_t message_size, uint32_t publishing_node) {
  if (publishing_node >= getNumberOfNodes())
    throw std::runtime_error("publishing node out of bounds");

  std::cout << "PUBLISH AT " << Simulator::Now().GetMicroSeconds() << " FROM " << publishing_node
            << std::endl;
  this->nodeContainer_.Get(publishing_node)
      ->GetApplication(0)
      ->GetObject<NatterApplication>()
      ->getNatterNode()
      ->publish(TOPIC, std::string(message_size, 'a'));
}

void NatterManager::publishRandom(uint32_t message_size) {
  std::uniform_int_distribution<uint64_t> dist(0, getNumberOfNodes() - 1);
  uint32_t publishing_node = dist(daisi::global_random_engine);
  publish(message_size, publishing_node);
}

void NatterManager::joinTopic(int /*number*/) { throw std::runtime_error("Not implemented yet"); }

void NatterManager::scheduleEvents() {
  uint64_t default_delay = parser_.getDefaultDelay();
  uint64_t current_time = 0;

  auto scenario_sequence = parser_.getScenarioSequence();

  for (const std::shared_ptr<ScenariofileParser::Table> &command : scenario_sequence) {
    auto map = command->content;

    auto it_join = map.find("join");
    if (it_join != map.end()) {
      std::optional<uint64_t> optional_delay = INNER_TABLE(it_join)->getOptional<uint64_t>("delay");
      uint64_t join_delay = optional_delay ? default_delay + *optional_delay : default_delay;

      auto mode = INNER_TABLE(it_join)->getRequired<std::string>("mode");
      STRING_TO_LOWER(mode);

      current_time += join_delay;

      if (mode == "minhton") {
        if (parser_.getFanout() < 2) throw std::runtime_error("fanout must be >=2 for minhton");
        Simulator::Schedule(MilliSeconds(current_time), &NatterManager::joinMinhton, this);
      } else {
        throw std::invalid_argument("Invalid mode type for join");
      }
      continue;
    }

    auto it_publish = map.find("publish");
    if (it_publish != map.end()) {
      std::optional<uint64_t> optional_delay =
          INNER_TABLE(it_publish)->getOptional<uint64_t>("delay");
      uint64_t publish_delay = optional_delay ? default_delay + *optional_delay : default_delay;

      auto mode = INNER_TABLE(it_publish)->getRequired<std::string>("mode");
      auto number = INNER_TABLE(it_publish)->getRequired<uint64_t>("number");
      auto message_size = INNER_TABLE(it_publish)->getRequired<uint64_t>("message_size");

      for (uint32_t i = 0; i < number; i++) {
        current_time += publish_delay;
        if (mode == "random")
          Simulator::Schedule(MilliSeconds(current_time), &NatterManager::publishRandom, this,
                              message_size);
        else if (mode == "sequential")
          Simulator::Schedule(MilliSeconds(current_time), &NatterManager::publish, this,
                              message_size, i % getNumberOfNodes());
        else
          throw std::runtime_error("Invalid publish mode");
      }
      continue;
    }
  }
}

std::string NatterManager::getDatabaseFilename() {
  return generateDBNameWithMinhtonInfo("natter", parser_.getFanout(), getNumberOfNodes());
}

}  // namespace daisi::natter_ns3
