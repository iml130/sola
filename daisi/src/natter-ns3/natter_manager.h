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

#ifndef DAISI_NATTER_NS3_NATTER_MANAGER_H_
#define DAISI_NATTER_NS3_NATTER_MANAGER_H_

#include <deque>

#include "manager/core_network.h"
#include "manager/manager.h"
#include "natter_application.h"
#include "natter_logger_ns3.h"
#include "natter_scenariofile.h"
#include "ns3/node-container.h"

namespace daisi::natter_ns3 {

class NatterManager : public Manager {
public:
  explicit NatterManager(const std::string &scenariofile_path);

private:
  void setupImpl() override;
  GeneralScenariofile getGeneralScenariofile() const override { return scenariofile_; }
  std::string getDatabaseFilename() const override;
  std::string getAdditionalParameters() const override;

  struct NodeInfo {
    const uint16_t fanout;
    const solanet::UUID own_uuid;
    const std::string ip;
    const uint16_t port;
    const uint32_t own_level;
    const uint32_t own_number;
    const ns3::Ptr<NatterApplication> app;
    const uint32_t container_index;
  };

  void scheduleEvents();
  uint64_t getNumberOfNodes() const;

  void scheduleEvent(const Join &step, ns3::Time &current_time);
  void scheduleEvent(const Publish &step, ns3::Time &current_time);

  void joinTopic(int number_nodes);
  void publishRandom(uint32_t message_size);
  void publish(uint32_t message_size, uint32_t publishing_node);

  NodeInfo getNodeInfo(uint32_t index) const;

  /*
   * Add node info to node of other_id
   */
  void addPeer(uint32_t other_id, const NodeInfo &info);
  void connectParent(const NodeInfo &info);
  std::set<uint32_t> connectChildren(const NodeInfo &info);
  std::set<uint32_t> connectNeighbors(const NodeInfo &info);
  void connectAdjacents(const NodeInfo &info, const std::vector<uint32_t> &lin_proj);
  void connectRoutingTableNeighborChildren(const NodeInfo &info, const std::set<uint32_t> &children,
                                           const std::set<uint32_t> &neighbors);

  void joinMinhton();
  ns3::Ptr<NatterApplication> getApplication(uint32_t id) const;
  void removeAllLinks(uint32_t id);
  NatterScenariofile scenariofile_;

  ns3::NodeContainer nodes_;

  CoreNetwork core_network_;

  const std::string topic_ = "TOPIC1";
};

}  // namespace daisi::natter_ns3

#endif
