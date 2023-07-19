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

#ifndef DAISI_UTILS_SOCKET_MANAGER_H_
#define DAISI_UTILS_SOCKET_MANAGER_H_

#include <string>
#include <vector>

#include "ns3/core-module.h"
#include "ns3/network-module.h"

namespace daisi {

enum class SocketType { kUDP, kTCP };

class SocketManager {
public:
  /// Getter for singleton
  static SocketManager &get() {
    static SocketManager utils;
    return utils;
  }

  ns3::Ptr<ns3::Socket> createSocket(SocketType type, bool localhost = false);

  void registerNode(std::vector<ns3::Ipv4Address> addresses, const ns3::Ptr<ns3::Node> &node,
                    uint16_t first_free_port);

  // Unregister as active node
  void unregisterNode(const ns3::Ptr<ns3::Node> &node);

private:
  SocketManager() = default;

  struct NetworkInfo {
    ns3::Ptr<ns3::Node> node;
    ns3::Ipv4Address address;
    uint16_t next_free_port;
    std::string tag;
  };

  using Context = uint32_t;  // ns-3 context

  std::unordered_map<Context, std::vector<NetworkInfo>> nodes_;
};

}  // namespace daisi

#endif
