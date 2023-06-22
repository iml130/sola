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

#ifndef DAISI_PATH_PLANNING_PATH_PLANNING_MANAGER_H_
#define DAISI_PATH_PLANNING_PATH_PLANNING_MANAGER_H_

#include <deque>
#include <optional>
#include <unordered_map>
#include <utility>

#include "agv_physical_basic.h"
#include "cpps/amr/amr_kinematics.h"
#include "cpps/common/cpps_logger_ns3.h"
#include "manager/manager.h"
#include "minhton/logging/logger_interface.h"
#include "ns3/bridge-helper.h"
#include "ns3/csma-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "path_planning/consensus/consensus_types.h"
#include "path_planning/path_planning_application.h"

namespace daisi::path_planning {

//! Manager that sets up and runs the path planning use case
class PathPlanningManager final : public Manager<PathPlanningApplication> {
public:
  explicit PathPlanningManager(const std::string &scenario_config_file);
  void setup() final;
  void initAGV(uint32_t index);

  void initPickupStation(uint32_t index);
  void postInitPickupStation(uint32_t index);

  void initDeliveryStation(uint32_t index);
  void postInitDeliveryStation(uint32_t index);

  void initCentralConsensus();

  void registerAGVByAuthority(uint32_t index);
  void connect(int index);

private:
  void setupNodes();

  /**
   * Calculate all routes/intersections and inject them into the applications
   */
  void setRouteInfo();

  void scheduleSpawnTO(uint32_t pickup_index);

  /**
   * Set permanent ARP entries for all devices in \param devices to all other devices
   * The containing IPs in \param ips must be in the same order as the NetDevices in \param devices
   */
  void setupArp(const ns3::NetDeviceContainer &devices, const ns3::Ipv4InterfaceContainer &ips);

  void setupNetworkEthernet();
  void setupNetworkWifi();
  void scheduleEvents() override;
  [[nodiscard]] uint64_t getNumberOfNodes() override;

  std::string getDatabaseFilename() override;

  void parse();
  void parseToSpawn(const std::shared_ptr<ScenariofileParser::Table> &spawn_description);
  void parseScenarioSequence();
  void parseConsensusSettings();

  /**
   * Check if all AMRs are returned to a pickup station and then stop the simulation
   */
  void checkSimulationFinished();

  [[nodiscard]] uint32_t getAGVInitialStationNumber(uint32_t agv_id) const;

  /**
   * Get a vector of information about all other pickup stations, which is needed if an AGV should
   * be handed off to another pickup station
   * @return vector with information about all other pickup stations
   */
  std::vector<PickupStationHandoverInfo> getPickupStationRegistry();

  uint32_t number_pickup_stations_ = 0;
  uint32_t number_delivery_stations_ = 0;
  uint64_t number_agvs_ = 0;
  cpps::Topology topology_;
  uint64_t to_spawn_duration_ms_ = 0;

  // TODO Pack to topology
  float left_border_ = 0;   // x
  float right_border_ = 0;  // x
  float upper_border_ = 0;  // y
  float lower_border_ = 0;  // y

  // Nodes / Network
  ns3::NodeContainer central_consensus_;
  ns3::NodeContainer agvs_;
  ns3::NodeContainer pickup_stations_;
  ns3::NodeContainer delivery_stations_;
  ns3::NodeContainer access_points_;

  const double delta_stations_ = 4.0;  //!< Delta distance (meters) between stations of same type

  consensus::ConsensusType consensus_type_ = consensus::ConsensusType::kNone;
  consensus::ConsensusSettings consensus_settings_;
  cpps::AmrKinematics kinematics_;  //!< Kinematics of the simulated AGV
};

}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_PATH_PLANNING_MANAGER_H_
