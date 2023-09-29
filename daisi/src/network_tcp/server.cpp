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

#include "server.h"

#include "utils/daisi_check.h"
#include "utils/socket_manager.h"
#include "utils/sola_utils.h"

namespace daisi::network_tcp {

Server::Server(ServerCallbacks callbacks) : Server("", std::move(callbacks)) {}

Server::Server(const Ipv4 &ip, ServerCallbacks callbacks)
    : listening_socket_(daisi::SocketManager::get().createSocket(daisi::SocketType::kTCP)),
      callbacks_(std::move(callbacks)) {
  listening_socket_->SetAcceptCallback(ns3::MakeCallback(&Server::connectionRequest, this),
                                       ns3::MakeCallback(&Server::newConnectionCreated, this));

  // https://groups.google.com/g/ns-3-users/c/tZmjq_KoCfo/m/x1xBvn-H31gJ
  ns3::Address addr;
  listening_socket_->GetSockName(addr);
  auto iaddr = ns3::InetSocketAddress::ConvertFrom(addr);

  ip_ = daisi::getIpv4AddressString(iaddr.GetIpv4());
  port_ = iaddr.GetPort();

  if (!ip.empty() && ip != ip_) {
    throw std::runtime_error("IP requested from application does not match socket IP");
  }

  const int res = listening_socket_->Listen();
  DAISI_CHECK(res == 0, "Failed to Listen()");
}

Server::~Server() {
  listening_socket_->SetAcceptCallback(
      ns3::MakeNullCallback<bool, ns3::Ptr<ns3::Socket>, const ns3::Address &>(),
      ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>, const ns3::Address &>());
  listening_socket_->Close();

  for (const auto &[_, connection] : connections_) {
    connection.socket->SetRecvCallback(ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>>());
    connection.socket->SetCloseCallbacks(ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>>(),
                                         ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>>());

    connection.socket->Close();
  }
}

void Server::send(TcpSocketHandle receiver, const std::string &msg) {
  std::string framed_msg = FramingManager::frameMsg(msg);

  DAISI_CHECK(connections_.count(receiver) == 1, "Handle not valid");
  const int res = connections_[receiver].socket->Send(
      reinterpret_cast<const uint8_t *>(framed_msg.data()), framed_msg.size(), 0);

  if (res != framed_msg.size()) {
    throw std::runtime_error("sending failed");
  }
}

uint16_t Server::getPort() const { return port_; }

Ipv4 Server::getIP() const { return ip_; }

void Server::readFromSocket(TcpSocketHandle handle, ns3::Ptr<ns3::Socket> socket) {
  DAISI_CHECK(socket == connections_[handle].socket, "Reading from wrong socket");
  ns3::Ptr<ns3::Packet> packet = socket->Recv();
  processPacket(packet, handle);
}

void Server::processPacket(ns3::Ptr<ns3::Packet> packet, TcpSocketHandle sender) {
  std::string data;
  data.resize(packet->GetSize());
  packet->CopyData(reinterpret_cast<uint8_t *>(data.data()), packet->GetSize());

  DAISI_CHECK(connections_.count(sender) == 1, "Invalid TCP connection");

  FramingManager &manager = connections_[sender].manager;
  manager.processNewData(data);

  if (!callbacks_.new_msg_cb) return;

  while (manager.hasPackets()) {
    callbacks_.new_msg_cb(sender, manager.nextPacket());
  }
}

bool Server::connectionRequest(ns3::Ptr<ns3::Socket>, const ns3::Address &) {
  // Always accept
  return true;
}

void Server::newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  const auto inet_addr = ns3::InetSocketAddress::ConvertFrom(addr);
  const std::string ip = daisi::getIpv4AddressString(inet_addr.GetIpv4());
  const uint16_t port = inet_addr.GetPort();
  if (callbacks_.client_connected_cb) {
    callbacks_.client_connected_cb(next_handle_, ip, port);
  }

  socket->SetCloseCallbacks(ns3::MakeCallback(&Server::handleClose, this, next_handle_),
                            ns3::MakeCallback(&Server::handleClose, this, next_handle_));
  socket->SetRecvCallback(ns3::MakeCallback(&Server::readFromSocket, this, next_handle_));
  connections_[next_handle_++] = {socket, {}};
}

void Server::handleClose(TcpSocketHandle handle, ns3::Ptr<ns3::Socket> socket) {
  if (callbacks_.client_disconnected_cb) {
    callbacks_.client_disconnected_cb(handle);
  }

  socket->Close();
  socket->SetRecvCallback(ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>>());
  connections_.erase(handle);
}

}  // namespace daisi::network_tcp
