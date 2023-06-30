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

#ifndef SOLA_NS3_CPPS_APPLICATION_H_
#define SOLA_NS3_CPPS_APPLICATION_H_

#include <deque>

#include "cpps/amr/physical/amr_physical_asset.h"
#include "cpps/common/cpps_logger_ns3.h"
#include "cpps/logical/amr/amr_logical_agent.h"
#include "cpps/logical/material_flow/material_flow_logical_agent.h"
#include "logging/logger_manager.h"
#include "minhton/logging/logger.h"
#include "minhton/logging/logger_interface.h"
#include "natter-ns3/natter_logger_ns3.h"
#include "natter/logger_interface.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"

namespace daisi::cpps {

extern std::deque<ns3::Ptr<ns3::Socket>> socket_global_;

struct CppsApplication final : public ns3::Application {
  static ns3::TypeId GetTypeId();

  void init();
  void start();
  void cleanup();

  void generateUDPSockets();
  ns3::Ptr<ns3::Socket> generateTCPSocket();
  ns3::Ipv4Address local_ip_address;
  ns3::Ipv4Address local_ip_address_tcp;
  uint16_t listening_port;
  uint16_t listening_port_tcp;
  std::shared_ptr<daisi::cpps::CppsLoggerNs3> logger;
  std::variant<std::monostate, std::shared_ptr<AmrPhysicalAsset>,
               std::shared_ptr<logical::MaterialFlowLogicalAgent>,
               std::shared_ptr<logical::AmrLogicalAgent>>
      application;
};
}  // namespace daisi::cpps
#endif
