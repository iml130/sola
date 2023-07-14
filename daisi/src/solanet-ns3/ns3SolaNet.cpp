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

#include <cassert>
#include <deque>

#include "ns3/ptr.h"
#include "ns3/socket.h"
#include "sola_message_ns3.h"
#include "solanet/network_udp/network_udp.h"
#include "utils/sola_utils.h"

#ifdef DAISI_SOLANET_NS3_DISABLE_NETWORKING
#include <set>

#include "ns3/simulator.h"
#endif

namespace daisi::solanet_ns3 {
// HACK: For passing socket to network interface without letting application know
std::deque<ns3::Ptr<ns3::Socket>> socket_global_;
}  // namespace daisi::solanet_ns3

using namespace solanet;
using namespace ns3;

std::unordered_map<std::string, int> map;  // HACK See Network::Impl ctor
class Network::Impl {
public:
  Impl(const std::string &ip, std::function<void(const Message &)> callback);

  ~Impl();

  void send(const Message &msg);

  std::string getIP() const;
  uint16_t getPort() const;

private:
  Ptr<Socket> socket_;
  std::string ip_;
  uint16_t port_;
  void processUdpPacket(Ptr<Socket> socket);
  void processPacket(Ptr<Packet> packet);

  std::function<void(const Message &)> callback_;

#ifdef DAISI_SOLANET_NS3_DISABLE_NETWORKING
  static std::set<Network::Impl *> network_interfaces_;
#endif
};

#ifdef DAISI_SOLANET_NS3_DISABLE_NETWORKING
// Definition required outside of class declaration
std::set<Network::Impl *> Network::Impl::network_interfaces_;
#endif

Network::Impl::Impl(const std::string &ip, std::function<void(const Message &)> callback)
    : callback_(std::move(callback)) {
  assert(!daisi::solanet_ns3::socket_global_.empty());

  // Hacky way to get the next free ns3 socket
  // This problem only arises in our ns3 simulation because the application can freely fetch new
  // sockets (e.g. by starting topic trees). But the low-level application (minhton, natter, sola,
  // ...) does not know that it is running on a ns3 node. Therefore we need to pass the ns3 socket
  // "around" the application to the ns3 network interface. And the network interface needs to
  // choose the correct socket.

  if (ip.empty()) {
    // Case 1 - Fetching next free socket

    int i = 0;
    for (; i < daisi::solanet_ns3::socket_global_.size(); i++) {
      auto ptr = daisi::solanet_ns3::socket_global_[i];
      // https://groups.google.com/g/ns-3-users/c/tZmjq_KoCfo/m/x1xBvn-H31gJ
      Address addr;
      daisi::solanet_ns3::socket_global_[i]->GetSockName(addr);
      InetSocketAddress iaddr = InetSocketAddress::ConvertFrom(addr);
      std::string socket_ip = daisi::getIpv4AddressString(iaddr.GetIpv4());

      // Only use socket is it is used less than 3 times already
      // If it is used >=3 times this IP is already in use for another SOLA instance
      if (map[socket_ip] < 3) {
        map[socket_ip]++;
        break;
      }
    }

    assert(i < daisi::solanet_ns3::socket_global_.size());
    socket_ = daisi::solanet_ns3::socket_global_[i];
    daisi::solanet_ns3::socket_global_.erase(daisi::solanet_ns3::socket_global_.begin() + i);
  } else {
    // Case 2 - Fetching socket with same IP

    int i = 0;
    for (; i < daisi::solanet_ns3::socket_global_.size(); i++) {
      auto ptr = daisi::solanet_ns3::socket_global_[i];
      // https://groups.google.com/g/ns-3-users/c/tZmjq_KoCfo/m/x1xBvn-H31gJ
      Address addr;
      daisi::solanet_ns3::socket_global_[i]->GetSockName(addr);
      InetSocketAddress iaddr = InetSocketAddress::ConvertFrom(addr);
      std::string socket_ip = daisi::getIpv4AddressString(iaddr.GetIpv4());
      if (socket_ip == ip) {
        break;
      }
    }
    map.at(ip)++;

    assert(i < daisi::solanet_ns3::socket_global_.size());
    socket_ = daisi::solanet_ns3::socket_global_[i];
    daisi::solanet_ns3::socket_global_.erase(daisi::solanet_ns3::socket_global_.begin() + i);
  }

  socket_->SetRecvCallback(MakeCallback(&Impl::processUdpPacket, this));

  // https://groups.google.com/g/ns-3-users/c/tZmjq_KoCfo/m/x1xBvn-H31gJ
  Address addr;
  socket_->GetSockName(addr);
  InetSocketAddress iaddr = InetSocketAddress::ConvertFrom(addr);

  ip_ = daisi::getIpv4AddressString(iaddr.GetIpv4());
  port_ = iaddr.GetPort();

#ifdef DAISI_SOLANET_NS3_DISABLE_NETWORKING
  Network::Impl::network_interfaces_.insert(this);
#endif
}

Network::Impl::~Impl() {
  socket_->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
  socket_->Close();
  socket_ = nullptr;

  map.at(ip_)--;
  if (map.at(ip_) == 0) {
    map.erase(ip_);
  }
}

void Network::Impl::processPacket(Ptr<Packet> packet) {
  // forwarding the message within the packet to the receiver
  daisi::solanet_ns3::SolaMessageNs3 sola_msg;
  packet->RemoveHeader(sola_msg);
  Message m{sola_msg.getIp(), sola_msg.getPort(), sola_msg.getPayload()};
  callback_(m);
}

void Network::Impl::processUdpPacket(Ptr<Socket> socket) {
  Ptr<Packet> packet;
  Address from;

  while ((packet = socket->RecvFrom(from))) {
    if (InetSocketAddress::IsMatchingType(from)) {
      processPacket(packet);
    }
  }
}

void Network::Impl::send(const Message &msg) {
  daisi::solanet_ns3::SolaMessageNs3 header;
  header.setPayload(msg.getMessage());
  header.setIp(ip_);
  header.setPort(port_);

  Ptr<Packet> packet = Create<Packet>();
  packet->AddHeader(header);

#ifdef DAISI_SOLANET_NS3_DISABLE_NETWORKING
  // Find network interface and schedule receive
  auto interface = std::find_if(
      Network::Impl::network_interfaces_.begin(), Network::Impl::network_interfaces_.end(),
      [msg](const Network::Impl *interface) {
        return msg.getIp() == interface->getIP() && msg.getPort() == interface->getPort();
      });
  if (interface == Network::Impl::network_interfaces_.end()) {
    throw std::runtime_error("no interface found for message");
  }
  ns3::Simulator::Schedule(ns3::MilliSeconds(2), &Network::Impl::processPacket, *interface, packet);
#else
  socket_->SendTo(packet, 0, InetSocketAddress(Ipv4Address(msg.getIp().c_str()), msg.getPort()));
#endif
}

std::string Network::Impl::getIP() const { return ip_; }

uint16_t Network::Impl::getPort() const { return port_; }

////////////////////////////////////
////// PIMP IMPLEMENTATION /////////
////////////////////////////////////

Network::Network(const std::function<void(const Message &)> &callback)
    : pimpl_(std::make_unique<Impl>("", callback)) {}

Network::Network(const std::string &ip, const std::function<void(const Message &)> &callback)
    : pimpl_(std::make_unique<Impl>(ip, callback)) {}

Network::~Network() = default;  // Declared as default here (and not in header) because otherwise
                                // class Impl has incomplete type

void Network::send(const Message &msg) { pimpl_->send(msg); }

uint16_t Network::getPort() const { return pimpl_->getPort(); }

std::string Network::getIP() const { return pimpl_->getIP(); }
