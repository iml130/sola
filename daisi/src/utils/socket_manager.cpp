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

#include "utils/socket_manager.h"

#include "utils/daisi_check.h"

namespace daisi {
ns3::Ptr<ns3::Socket> SocketManager::createSocket(SocketType type, bool localhost) {
  const uint32_t context = ns3::Simulator::GetContext();
  DAISI_CHECK(context != ns3::Simulator::NO_CONTEXT, "Context not set");

  static std::unordered_map<SocketType, std::string> socket_factory_map{
      {SocketType::kUDP, "ns3::UdpSocketFactory"}, {SocketType::kTCP, "ns3::TcpSocketFactory"}};

  ns3::TypeId tid = ns3::TypeId::LookupByName(socket_factory_map.at(type));

  std::vector<NetworkInfo> &infos = nodes_.at(context);

  // Find info with correct tag
  auto it = std::find_if(infos.begin(), infos.end(), [localhost](const NetworkInfo &info) {
    return info.tag == (localhost ? "localhost" : "real");
  });
  DAISI_CHECK(it != infos.end(), "No network info found with required tag");

  NetworkInfo &info = *it;

  auto socket = ns3::Socket::CreateSocket(info.node, tid);
  ns3::InetSocketAddress local_address(info.address, info.next_free_port);
  info.next_free_port++;
  socket->Bind(local_address);

  if (socket->GetErrno() != ns3::Socket::ERROR_NOTERROR) {
    throw std::runtime_error("failed to open socket");
  }
  return socket;
}

void SocketManager::registerNode(std::vector<ns3::Ipv4Address> addresses,
                                 const ns3::Ptr<ns3::Node> &node, uint16_t first_free_port) {
  DAISI_CHECK(nodes_.find(node->GetId()) == nodes_.end(), "Node already registered");
  std::vector<NetworkInfo> infos;
  for (const auto &address : addresses) {
    std::string tag = "real";
    if (address.Get() == 2130706433) {  // 127.0.0.1
      tag = "localhost";
    }

    infos.push_back({node, address, first_free_port, tag});
  }
  nodes_[node->GetId()] = infos;
}

void SocketManager::unregisterNode(const ns3::Ptr<ns3::Node> &node) { nodes_.erase(node->GetId()); }

}  // namespace daisi
