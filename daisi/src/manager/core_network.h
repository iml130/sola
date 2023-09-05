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

#ifndef DAISI_MANAGER_CORE_NETWORK_H_
#define DAISI_MANAGER_CORE_NETWORK_H_

#include <cstdint>
#include <vector>

#include "ns3/network-module.h"

// Forward declarations for private members/methods
namespace ns3 {
class Ipv4InterfaceContainer;
class CsmaHelper;
}  // namespace ns3

namespace daisi {

/// @brief Network structure with one central L3 router, forming a star topology
class CoreNetwork {
public:
  CoreNetwork();

  void addNodesCSMA(ns3::NodeContainer nodes);

  void addNodesWifi(ns3::NodeContainer nodes, double topology_width, double topology_height);

private:
  static void setGatewayForAppNodes(uint32_t number_of_subnets,
                                    std::vector<ns3::Ipv4InterfaceContainer> ip_container);

  ns3::CsmaHelper setupCSMA();

  std::vector<ns3::Ipv4InterfaceContainer> installIP(
      uint32_t number_of_subnets, ns3::NetDeviceContainer core_router_devices,
      std::vector<ns3::NetDeviceContainer> app_node_devices, uint32_t &base_address);

  ns3::Ptr<ns3::Node> core_router_;
  uint32_t next_base_address_csma_ = 16842752;    // 1.1.0.0
  uint32_t next_base_address_wifi_ = 3232235520;  // 192.168.0.0
};
}  // namespace daisi

#endif
