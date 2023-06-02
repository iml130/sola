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

#include "cpps/common/agv_description.h"
#include "cpps/common/boundaries.h"
#include "cpps/common/cpps_application.h"
#include "cpps/common/cpps_logger_ns3.h"
#include "cpps/model/ability.h"
#include "cpps/model/kinematics.h"
#include "cpps/negotiation/mrta_configuration.h"
#include "manager/manager.h"
#include "minhton/logging/logger.h"
#include "minhton/logging/logger_interface.h"
#include "natter-ns3/natter_logger_ns3.h"
#include "natter/logger_interface.h"
#include "ns3/bridge-helper.h"
#include "ns3/csma-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"

namespace daisi::cpps {

struct DistAbs {
  uint64_t abs;
};

struct DistProb {
  float prob;
};

struct DistGaussian {
  std::normal_distribution<> dist;
};

struct MaterialFlowInfo {
  std::vector<std::shared_ptr<ScenariofileParser::Table>> descriptions;

  std::vector<ns3::Vector> locations;
};

struct SpawnInfo {
  uint64_t start_time = 0;
  std::string type;
  std::string friendly_name;
  std::variant<DistAbs, DistProb, DistGaussian> distribution = {};

  bool operator>(const SpawnInfo &other) const {
    return this->start_time > other.start_time || this->friendly_name > other.friendly_name;
  }
};

class CppsManager : public daisi::Manager<CppsApplication> {
public:
  explicit CppsManager(const std::string &scenario_config_file);
  virtual void setup() override;
  void initAGV(uint32_t index);
  void connect(int index);
  void publishService(int index);
  void scheduleMaterialFlow(const SpawnInfo &info);
  void executeMaterialFlow(int index, const std::string &friendly_name);

private:
  void setupNodes();

  void setupNetworkEthernet();
  void setupNetworkWifi();
  virtual void scheduleEvents() override;
  virtual uint64_t getNumberOfNodes() override;
  std::string getDatabaseFilename() override;

  void spawnAGV(uint32_t agv_index, const AgvDeviceProperties &properties,
                const TopologyNs3 &topology);

  void parse();
  void parseAGVs();
  void parseTOs();
  void parseAgvSpawn(const std::shared_ptr<ScenariofileParser::Table> &spawn_description);
  void parseToSpawn(const std::shared_ptr<ScenariofileParser::Table> &spawn_description);
  void parseTopology();
  void parseScenarioSequence();
  void parseMaterialFlowModel(const std::shared_ptr<MaterialFlowModel> &model,
                              const std::shared_ptr<ScenariofileParser::Table> &model_description);

  void parseMRTAConfiguration();
  static std::unordered_map<std::string, InteractionProtocolType> interaction_protocol_types_map_;
  static std::unordered_map<std::string, TaskManagementType> task_management_types_map_;
  static std::unordered_map<std::string, UtilityEvaluationComponents>
      utility_evaluation_components_map_;

  // Initiates shutdown of finished TOs
  void clearFinishedMaterialFlows();

  Kinematics parseKinematics(std::shared_ptr<daisi::ScenariofileParser::Table> description);
  mrta::model::Ability parseAGVAbility(
      std::shared_ptr<daisi::ScenariofileParser::Table> description);

  MRTAConfig mrta_config_;

  Boundaries parseBoundaries(const std::shared_ptr<ScenariofileParser::Table> &description);
  void checkStarted(uint32_t index);

  uint64_t number_agvs_later_ = 0;
  uint64_t number_agvs_initial_ = 0;
  uint64_t number_material_flow_nodes_ = 0;
  uint64_t number_material_flows_ = 0;
  uint64_t number_material_flows_scheduled_for_execution_ = 0;
  uint64_t number_material_flows_finished_ = 0;

  bool material_flow_nodes_leave_after_finish_ = true;
  int width_ = 0;
  int height_ = 0;
  int depth_ = 0;
  std::vector<AgvDeviceProperties> agv_device_properties_;
  std::priority_queue<SpawnInfo, std::vector<SpawnInfo>, std::greater<SpawnInfo>> spawn_info_;
  std::priority_queue<SpawnInfo, std::vector<SpawnInfo>, std::greater<SpawnInfo>> schedule_info_;

  // friendly name -> model
  std::unordered_map<std::string, MaterialFlowInfo> material_flow_models_;

  // Nodes / Network
  ns3::NodeContainer agvs_;
  ns3::NodeContainer material_flows_;
  ns3::NodeContainer access_points_;

  ns3::NetDeviceContainer agv_sta_dev_;
};

}  // namespace daisi::cpps

#endif
