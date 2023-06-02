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

#ifndef DAISI_PATH_PLANNING_PATH_PLANNING_APPLICATION_H_
#define DAISI_PATH_PLANNING_PATH_PLANNING_APPLICATION_H_

#include <deque>
#include <utility>

#include "cpps/agv/agv_physical_basic.h"
#include "cpps/common/cpps_logger_ns3.h"
#include "delivery_station.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "path_planning/agv_logical.h"
#include "path_planning/consensus/central/central_server.h"
#include "path_planning/consensus/paxos/constants.h"
#include "pickup_station.h"
#include "station.h"

namespace daisi::path_planning {

//! ns-3 application, containing the real application (\c PickupStation, \c DeliveryStation, \c
//! AGVLogical, \c AGVPhysical or \c CentralServer) and helps setting up the ns-3 network components
//! for this application
struct PathPlanningApplication final : public ns3::Application {
  static ns3::TypeId GetTypeId();

  void DoDispose() final {}
  void StartApplication() final {}

  void initPickupStation(const PickupStationInfo &info,
                         const consensus::ConsensusSettings &settings, NextTOMode next_to_mode);
  void initDeliveryStation(const DeliveryStationInfo &info,
                           const consensus::ConsensusSettings &settings);
  void initAGVLogical(const consensus::ConsensusSettings &settings, bool first_node);
  void initAGVPhysical(const cpps::AgvDataModel &data_model, int id_friendly);
  void initConsensusCentralServer(const consensus::CentralSettings &settings);

  void postInit();

  void setTopology(cpps::TopologyNs3 topology) { topology_ = std::move(topology); };

  /**
   * Creating \p number sockets connected to the IP of this application and injects them into the
   * global SolaNet simulation variable
   * By default SOLA needs 3 sockets (SOLA itself, MINHTON and natter).
   * @param number number of sockets to create
   */
  void generateUDPSockets(uint32_t number = 3);
  ns3::Ptr<ns3::Socket> generateTCPSocket();

  ns3::Ipv4Address local_ip_address;      //!< ns-3 attribute with first IP of this application
  ns3::Ipv4Address local_ip_address_tcp;  //!< ns-3 attribute with second IP of this application
                                          //!< (only used for \c AGVLogical which connects to the
                                          //!< management overlay and the physical AGV)
  uint16_t listening_port;      //!< ns-3 attribute with first port used for this application
  uint16_t listening_port_tcp;  //!< ns-3 attribute with second port used for this application
  std::variant<std::monostate, cpps::AGVPhysicalBasic, AGVLogical, PickupStation, DeliveryStation,
               consensus::CentralServer>
      application;  //!< Variant that is holding the actual application

private:
  bool initialized_ = false;
  cpps::TopologyNs3 topology_;

  void checkInitialized();
};
}  // namespace daisi::path_planning
#endif  // DAISI_PATH_PLANNING_PATH_PLANNING_APPLICATION_H_
