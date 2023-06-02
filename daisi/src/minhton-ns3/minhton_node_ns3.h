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

#ifndef DAISI_MINHTON_NS3_MINHTON_NODE_NS3_H_
#define DAISI_MINHTON_NS3_MINHTON_NODE_NS3_H_

#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/core/node.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "solanet-ns3/sola_message_ns3.h"

namespace daisi::minhton_ns3 {

class MinhtonNodeNs3 : public ns3::Object {
public:
  explicit MinhtonNodeNs3(const minhton::ConfigNode &config);
  ~MinhtonNodeNs3() = default;

  static ns3::TypeId GetTypeId();
  ns3::TypeId GetInstanceTypeId() const;

  void processSignal(const minhton::Signal &signal);

  minhton::NodeInfo getNodeInfo();

  void executeSearchExactTest(uint32_t dest_level, uint32_t dest_number);

  void executeFindQuery(const minhton::FindQuery &query);

  void localTestDataInsert(const std::vector<minhton::Entry> &entries);
  void localTestDataUpdate(const std::vector<minhton::Entry> &entries);
  void localTestDataRemove(const std::vector<minhton::NodeData::Key> &keys);

  // void processSignal(minhton::Signal signal);

  void setStaticBuildNeighbors(const minhton::NodeInfo &self_pos,
                               const std::vector<minhton::NodeInfo> &neighbors,
                               const minhton::NodeInfo &adj_left,
                               const minhton::NodeInfo &adj_right);

private:
  minhton::MinhtonNode node_;
};
}  // namespace daisi::minhton_ns3
#endif
