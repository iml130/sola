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

#ifndef DAISI_NETWORK_TCP_CLIENT_H_
#define DAISI_NETWORK_TCP_CLIENT_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "network_tcp/definitions.h"
#include "network_tcp/framing_manager.h"
#include "ns3/socket.h"

namespace daisi::network_tcp {

struct ClientCallbacks {
  std::function<void(const std::string &msg)> new_msg_cb;
  std::function<void()> connected_cb;
  std::function<void()> disconnected_cb;
};

/**
 * ns-3 client to connect to a single listening ns-3 TCP sockets and send/recv data
 */
class Client {
public:
  Client(ClientCallbacks callbacks, const Endpoint &endpoint);

  Client(const Ipv4 &own_ip, ClientCallbacks callbacks, const Endpoint &endpoint);

  ~Client();

  // Forbid copy/move operations
  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;
  Client(const Client &&) = delete;
  Client &operator=(Client &&) = delete;

  /**
   * Send message
   * @param msg message to send
   */
  void send(const std::string &msg);

  uint16_t getPort() const;

  Ipv4 getIP() const;

  bool isConnected() const;

private:
  void readFromSocket(ns3::Ptr<ns3::Socket> socket);
  void processPacket(ns3::Ptr<ns3::Packet> packet);

  void closedSocket(ns3::Ptr<ns3::Socket>);
  void connectedSuccessful(ns3::Ptr<ns3::Socket>);
  void connectionFailed(ns3::Ptr<ns3::Socket>);

  ns3::Ptr<ns3::Socket> socket_;

  bool connected_ = false;

  FramingManager manager_;

  Ipv4 ip_;
  uint16_t port_;

  const ClientCallbacks callbacks_;
};
}  // namespace daisi::network_tcp

#endif  // DAISI_NETWORK_TCP_CLIENT_H_
