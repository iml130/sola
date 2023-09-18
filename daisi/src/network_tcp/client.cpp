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

#include "client.h"

#include "utils/daisi_check.h"
#include "utils/socket_manager.h"
#include "utils/sola_utils.h"

namespace daisi::network_tcp {

Client::Client(ClientCallbacks callbacks, const Endpoint &endpoint)
    : Client("", callbacks, endpoint) {}

Client::Client(const Ipv4 &ip, ClientCallbacks callbacks, const Endpoint &endpoint)
    : socket_(daisi::SocketManager::get().createSocket(daisi::SocketType::kTCP)),
      callbacks_(std::move(callbacks)) {
  socket_->SetConnectCallback(MakeCallback(&Client::connectedSuccessful, this),
                              MakeCallback(&Client::connectionFailed, this));
  socket_->SetCloseCallbacks(MakeCallback(&Client::closedSocket, this),
                             MakeCallback(&Client::closedSocket, this));
  socket_->SetRecvCallback(MakeCallback(&Client::readFromSocket, this));

  // https://groups.google.com/g/ns-3-users/c/tZmjq_KoCfo/m/x1xBvn-H31gJ
  ns3::Address addr;
  socket_->GetSockName(addr);
  auto iaddr = ns3::InetSocketAddress::ConvertFrom(addr);

  ip_ = daisi::getIpv4AddressString(iaddr.GetIpv4());
  port_ = iaddr.GetPort();

  if (!ip.empty() && ip != ip_) {
    throw std::runtime_error("IP requested from application does not match socket IP");
  }

  // Connect to remote
  ns3::InetSocketAddress remote_addr(endpoint.ip.c_str(), endpoint.port);
  socket_->Connect(remote_addr);
}

Client::~Client() {
  if (socket_) {
    socket_->SetRecvCallback(ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>>());
    socket_->SetCloseCallbacks(ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>>(),
                               ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>>());
    socket_->Close();

    if (callbacks_.disconnected_cb) {
      callbacks_.disconnected_cb();
    }
  }
}

void Client::send(const std::string &msg) {
  DAISI_CHECK(connected_, "Not connected");
  DAISI_CHECK(socket_, "Socket not available");

  std::string framed_msg = FramingManager::frameMsg(msg);

  DAISI_CHECK(connected_, "Not connected");
  const auto res =
      socket_->Send(reinterpret_cast<const uint8_t *>(framed_msg.data()), framed_msg.size(), 0);

  if (res != framed_msg.size()) {
    throw std::runtime_error("sending failed");
  }
}

uint16_t Client::getPort() const { return port_; }

Ipv4 Client::getIP() const { return ip_; }

void Client::readFromSocket(ns3::Ptr<ns3::Socket> socket) {
  DAISI_CHECK(socket == socket_, "Reading from invalid socket");
  ns3::Ptr<ns3::Packet> packet = socket_->Recv();
  processPacket(packet);
}

void Client::processPacket(ns3::Ptr<ns3::Packet> packet) {
  DAISI_CHECK(packet != nullptr, "Invalid packet");

  std::string data;
  data.resize(packet->GetSize());
  packet->CopyData(reinterpret_cast<uint8_t *>(data.data()), packet->GetSize());

  manager_.processNewData(data);

  if (!callbacks_.new_msg_cb) return;

  while (manager_.hasPackets()) {
    callbacks_.new_msg_cb(manager_.nextPacket());
  }
}

void Client::closedSocket(ns3::Ptr<ns3::Socket>) {
  connected_ = false;
  socket_->SetRecvCallback(ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket>>());
  socket_->Close();
  socket_ = nullptr;

  if (callbacks_.disconnected_cb) {
    callbacks_.disconnected_cb();
  }
}

void Client::connectedSuccessful(ns3::Ptr<ns3::Socket>) {
  connected_ = true;

  if (callbacks_.connected_cb) {
    callbacks_.connected_cb();
  }
}

void Client::connectionFailed(ns3::Ptr<ns3::Socket>) {
  throw std::runtime_error("failed to connect");
}

}  // namespace daisi::network_tcp
