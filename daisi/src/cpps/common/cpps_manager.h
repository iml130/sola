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

#ifndef DAISI_CPPS_MANAGER_H_
#define DAISI_CPPS_MANAGER_H_

#include <deque>
#include <optional>
#include <queue>
#include <unordered_map>

#include "cpps/amr/amr_topology.h"
#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/common/cpps_application.h"
#include "cpps/common/cpps_logger_ns3.h"
#include "cpps/logical/material_flow/material_flow_logical_agent.h"
#include "manager/manager.h"
#include "ns3/bridge-helper.h"
#include "ns3/csma-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "scenariofile/cpps_scenariofile.h"

namespace daisi::cpps {

class CppsManager : public daisi::Manager<CppsApplication> {
public:
  explicit CppsManager(const std::string &scenario_config_file);
  void setup() override;

  void initAMR(uint32_t index);
  void connectAMR(uint32_t index);
  void startAMR(uint32_t index);

  void initMF(uint32_t index);
  void startMF(uint32_t index);

  void scheduleMaterialFlow(const SpawnInfoScenario &info);
  void executeMaterialFlow(int index, const std::string &friendly_name);

private:
  void initialSpawn();

  void setupNodes();
  void setupNetworkEthernet();
  void setupNetworkWifi();

  void scheduleEvents() override;

  uint64_t getNumberOfNodes() override;
  std::string getDatabaseFilename() override;

  void spawnAMR(uint32_t amr_index, const AmrDescription &description, const Topology &topology);

  // Initiates shutdown of finished TOs
  void clearFinishedMaterialFlows();

  void checkStarted(uint32_t index);

  CppsScenariofile scenario_;

  std::unordered_map<std::string, AmrDescription> amr_descriptions_;
  std::unordered_map<std::string, MaterialFlowDescriptionScenario> material_flow_descriptions_;

  std::priority_queue<SpawnInfoScenario, std::vector<SpawnInfoScenario>, std::greater<>>
      spawn_info_;
  std::priority_queue<SpawnInfoScenario, std::vector<SpawnInfoScenario>, std::greater<>>
      schedule_info_;

  // counters for scheduling
  uint64_t number_material_flows_scheduled_for_execution_ = 0;
  uint64_t number_material_flows_finished_ = 0;

  // Nodes / Network
  ns3::NodeContainer amrs_;
  ns3::NodeContainer material_flows_;
  ns3::NodeContainer access_points_;

  ns3::NetDeviceContainer amr_sta_dev_;
};

}  // namespace daisi::cpps

#endif
