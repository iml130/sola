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

#include "cpps/common/cpps_application.h"

#include "../src/logging/logger.h"  // WORKAROUND: private header
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::cpps {
ns3::TypeId CppsApplication::GetTypeId() {
  static TypeId tid =
      TypeId("CppsApplication")
          .SetParent<Application>()
          .AddConstructor<CppsApplication>()
          .AddAttribute("LocalIpAddress", "LocalIpAddress", Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&CppsApplication::local_ip_address),
                        MakeIpv4AddressChecker())
          .AddAttribute("ListeningPort", "ListeningPort", UintegerValue(0),
                        MakeUintegerAccessor(&CppsApplication::listening_port),
                        MakeUintegerChecker<uint16_t>())
          .AddAttribute("LocalIpAddressTCP", "LocalIpAddressTCP", Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&CppsApplication::local_ip_address_tcp),
                        MakeIpv4AddressChecker())
          .AddAttribute("ListeningPortTCP", "ListeningPortTCP", UintegerValue(0),
                        MakeUintegerAccessor(&CppsApplication::listening_port_tcp),
                        MakeUintegerChecker<uint16_t>());
  return tid;
}

void CppsApplication::generateUDPSockets() {
  SolaNetworkUtils::get().registerNode(local_ip_address, GetNode(), listening_port);
  SolaNetworkUtils::get().createSockets(getIpv4AddressString(local_ip_address), 3);
  // own_sockets_.push_back(socket);
}

ns3::Ptr<ns3::Socket> CppsApplication::generateTCPSocket() {
  TypeId tid = TypeId::LookupByName("ns3::TcpSocketFactory");
  auto socket = Socket::CreateSocket(GetNode(), tid);
  return socket;
}

void CppsApplication::cleanup() {
  application = std::monostate{};
  // We do not clear the IP address. IPs are reused
  SolaNetworkUtils::get().unregisterNode(local_ip_address);
}

void CppsApplication::init() {
  if (auto amr_logical_agent =
          std::get_if<std::shared_ptr<logical::AmrLogicalAgent>>(&application)) {
    generateUDPSockets();
    auto tcp_socket = generateTCPSocket();
    tcp_socket->Bind(InetSocketAddress(local_ip_address_tcp, listening_port_tcp));
    (*amr_logical_agent)->init(tcp_socket);
  } else if (auto mf_logical_agent =
                 std::get_if<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(&application)) {
    generateUDPSockets();
    (*mf_logical_agent)->init();
  } else if (auto amr_physical_asset =
                 std::get_if<std::shared_ptr<AmrPhysicalAsset>>(&application)) {
    auto tcp_socket = generateTCPSocket();
    tcp_socket->Bind(InetSocketAddress(
        local_ip_address, listening_port));  // Intentionally not using the address_tcp_ field
    (*amr_physical_asset)->init(tcp_socket);
  }
}

void CppsApplication::start() {
  if (auto amr_logical_agent =
          std::get_if<std::shared_ptr<logical::AmrLogicalAgent>>(&application)) {
    (*amr_logical_agent)->start();
  } else if (auto mf_logical_agent =
                 std::get_if<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(&application)) {
    (*mf_logical_agent)->start();
  }
}

}  // namespace daisi::cpps
