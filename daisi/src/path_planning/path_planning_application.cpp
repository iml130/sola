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

#include "path_planning/path_planning_application.h"

#include "utils/daisi_check.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::path_planning {
ns3::TypeId PathPlanningApplication::GetTypeId() {
  static TypeId tid =
      TypeId("PathPlanningApplication")
          .SetParent<Application>()
          .AddConstructor<PathPlanningApplication>()
          .AddAttribute("LocalIpAddress", "LocalIpAddress", Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&PathPlanningApplication::local_ip_address),
                        MakeIpv4AddressChecker())
          .AddAttribute("ListeningPort", "ListeningPort", UintegerValue(0),
                        MakeUintegerAccessor(&PathPlanningApplication::listening_port),
                        MakeUintegerChecker<uint16_t>())
          .AddAttribute("LocalIpAddressTCP", "LocalIpAddressTCP", Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&PathPlanningApplication::local_ip_address_tcp),
                        MakeIpv4AddressChecker())
          .AddAttribute("ListeningPortTCP", "ListeningPortTCP", UintegerValue(0),
                        MakeUintegerAccessor(&PathPlanningApplication::listening_port_tcp),
                        MakeUintegerChecker<uint16_t>());
  return tid;
}

void PathPlanningApplication::generateUDPSockets(uint32_t number) {
  SolaNetworkUtils::get().registerNode(local_ip_address, GetNode(), listening_port);
  SolaNetworkUtils::get().createSockets(daisi::getIpv4AddressString(local_ip_address), number);
  // own_sockets_.push_back(socket);
}

ns3::Ptr<ns3::Socket> PathPlanningApplication::generateTCPSocket() {
  TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
  auto socket = Socket::CreateSocket(GetNode(), tid);
  return socket;
}

void PathPlanningApplication::postInit() {
  if (std::holds_alternative<PickupStation>(application)) {
    std::get<PickupStation>(application).postInit();
  } else if (std::holds_alternative<DeliveryStation>(application)) {
    std::get<DeliveryStation>(application).postInit();
  } else {
    throw std::runtime_error("post init not available");
  }
}

void PathPlanningApplication::initPickupStation(const PickupStationInfo &info,
                                                const consensus::ConsensusSettings &settings,
                                                NextTOMode next_to_mode) {
  checkInitialized();
  generateUDPSockets();
  const uint32_t device_id = GetNode()->GetId();
  application.emplace<PickupStation>(
      info, daisi::global_logger_manager->createPathPlanningLogger(device_id, "PickupStation"),
      settings, next_to_mode, device_id);
}

void PathPlanningApplication::initDeliveryStation(const DeliveryStationInfo &info,
                                                  const consensus::ConsensusSettings &settings) {
  checkInitialized();
  generateUDPSockets();
  const uint32_t device_id = GetNode()->GetId();
  application.emplace<DeliveryStation>(
      info, settings,
      daisi::global_logger_manager->createPathPlanningLogger(device_id, "DeliveryStation"),
      device_id);
}

void PathPlanningApplication::initAGVLogical(const consensus::ConsensusSettings &settings,
                                             bool first_node) {
  checkInitialized();
  generateUDPSockets();
  auto tcp_socket = generateTCPSocket();
  tcp_socket->Bind(InetSocketAddress(local_ip_address_tcp, listening_port_tcp));
  application.emplace<AGVLogical>(topology_, settings, first_node, tcp_socket, GetNode()->GetId());
}

void PathPlanningApplication::checkInitialized() {
  DAISI_CHECK(!initialized_, "Application already initialized");
  initialized_ = true;
}

void PathPlanningApplication::initAGVPhysical(const cpps::AgvDataModel &data_model,
                                              int id_friendly) {
  checkInitialized();
  auto tcp_socket = generateTCPSocket();

  // Intentionally not using the address_tcp_ field!
  tcp_socket->Bind(InetSocketAddress(local_ip_address, listening_port));

  application.emplace<cpps::AGVPhysicalBasic>(data_model, topology_, id_friendly, tcp_socket);
}

void PathPlanningApplication::initConsensusCentralServer(
    const consensus::CentralSettings &settings) {
  checkInitialized();
  generateUDPSockets(1);
  application.emplace<consensus::CentralServer>(
      settings,
      daisi::global_logger_manager->createPathPlanningLogger(GetNode()->GetId(), "CentralServer"));
}
}  // namespace daisi::path_planning
