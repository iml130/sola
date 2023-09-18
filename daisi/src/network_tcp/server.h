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

#ifndef DAISI_NETWORK_TCP_SERVER_H_
#define DAISI_NETWORK_TCP_SERVER_H_

#include <cstdint>
#include <functional>
#include <string>

#include "network_tcp/definitions.h"
#include "network_tcp/framing_manager.h"
#include "ns3/socket.h"

namespace daisi::network_tcp {

struct ServerCallbacks {
  std::function<void(TcpSocketHandle, const std::string &msg)> new_msg_cb;
  std::function<void(TcpSocketHandle, const Ipv4 &ip, uint16_t port)> client_connected_cb;
  std::function<void(TcpSocketHandle)> client_disconnected_cb;
};

/**
 * Class representing a ns-3 server listens for incoming ns-3 TCP connections and send/recv data
 */
class Server {
public:
  explicit Server(ServerCallbacks callbacks);

  Server(const Ipv4 &ip, ServerCallbacks callbacks);

  ~Server();

  // Forbid copy/move operations
  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;
  Server(const Server &&) = delete;
  Server &operator=(Server &&) = delete;

  /**
   * Send message
   * @param receiver Receiver to send msg to
   * @param msg message to send
   */
  void send(TcpSocketHandle receiver, const std::string &msg);

  uint16_t getPort() const;

  Ipv4 getIP() const;

private:
  ns3::Ptr<ns3::Socket> listening_socket_;

  std::string ip_;
  uint16_t port_;

  const ServerCallbacks callbacks_;

  struct TcpConnection {
    ns3::Ptr<ns3::Socket> socket;
    FramingManager manager;
  };

  // Handle number for next incoming connection
  TcpSocketHandle next_handle_ = 1;

  // Map of all currently open connections
  std::unordered_map<TcpSocketHandle, TcpConnection> connections_;

  void readFromSocket(TcpSocketHandle handle, ns3::Ptr<ns3::Socket> socket);
  void processPacket(ns3::Ptr<ns3::Packet> packet, TcpSocketHandle sender);

  void handleClose(TcpSocketHandle handle, ns3::Ptr<ns3::Socket> socket);

  bool connectionRequest(ns3::Ptr<ns3::Socket>, const ns3::Address &);
  void newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);

  void successfulConnect(ns3::Ptr<ns3::Packet>);
  void failedConnect(ns3::Ptr<ns3::Packet>);
};
}  // namespace daisi::network_tcp

#endif  // DAISI_NETWORK_TCP_SERVER_H_
