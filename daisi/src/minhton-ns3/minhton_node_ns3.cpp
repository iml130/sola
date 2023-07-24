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

#include "minhton_node_ns3.h"

#include <cassert>

#include "minhton/logging/logging.h"
#include "minhton/message/empty.h"
#include "minhton/message/se_types.h"

using namespace ns3;
namespace daisi::minhton_ns3 {

MinhtonNodeNs3::MinhtonNodeNs3(const minhton::ConfigNode &config)
    : node_(config, minhton::NeighborCallbackFct{}, false) {}

TypeId MinhtonNodeNs3::GetTypeId() {
  static TypeId tid = TypeId("ns3::MinhtonNodeNs3").SetParent<Object>();
  return tid;
}

TypeId MinhtonNodeNs3::GetInstanceTypeId() const { return GetTypeId(); }

void MinhtonNodeNs3::executeSearchExactTest(uint32_t dest_level, uint32_t dest_number) {
  auto our_node_info = node_.getNodeInfo();
  if (our_node_info.getLevel() == dest_level && our_node_info.getNumber() == dest_number) {
    return;
  }

  auto dest_node = minhton::NodeInfo(dest_level, dest_number, our_node_info.getFanout());
  minhton::MinhtonMessageHeader header(node_.getNodeInfo(), dest_node);
  minhton::MessageEmpty query(header);
  node_.performSearchExactTest(dest_node, std::make_shared<minhton::MessageSEVariant>(query));
}

void MinhtonNodeNs3::processSignal(const minhton::Signal &signal) { node_.processSignal(signal); }

void MinhtonNodeNs3::localTestDataInsert(const std::vector<minhton::Entry> &entries) {
  node_.insert(entries);
}

void MinhtonNodeNs3::localTestDataUpdate(const std::vector<minhton::Entry> &entries) {
  node_.update(entries);
}

void MinhtonNodeNs3::localTestDataRemove(const std::vector<minhton::NodeData::Key> &keys) {
  node_.remove(keys);
}

void MinhtonNodeNs3::executeFindQuery(const minhton::FindQuery &query) { node_.find(query); }

void MinhtonNodeNs3::setStaticBuildNeighbors(const minhton::NodeInfo &self_pos,
                                             const std::vector<minhton::NodeInfo> &neighbors,
                                             const minhton::NodeInfo &adj_left,
                                             const minhton::NodeInfo &adj_right) {
  minhton::FSMState init_state = minhton::FSMState::kConnected;
  node_.initFSM(init_state);

  node_.getRoutingInformation()->setPosition(self_pos.getLogicalNodeInfo());
  if (self_pos.getLevel() != 0) {
    const auto &l_node_info = self_pos.getLogicalNodeInfo();
    node_.getAccessContainer()->logger.logNode({l_node_info.getUuid(), l_node_info.getLevel(),
                                                l_node_info.getNumber(), l_node_info.getFanout(),
                                                l_node_info.isInitialized()});
  }

  if (adj_left.isInitialized()) {
    node_.getRoutingInformation()->setAdjacentLeft(adj_left);
  }
  if (adj_right.isInitialized()) {
    node_.getRoutingInformation()->setAdjacentRight(adj_right);
  }

  for (auto const &neighbor : neighbors) {
    node_.getRoutingInformation()->updateNeighbor(neighbor);
  }
}

minhton::NodeInfo MinhtonNodeNs3::getNodeInfo() { return node_.getNodeInfo(); }

}  // namespace daisi::minhton_ns3
