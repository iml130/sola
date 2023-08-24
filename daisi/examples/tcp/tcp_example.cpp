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
#include <iostream>

#include "network_tcp/client.h"
#include "network_tcp/server.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "utils/socket_manager.h"
#include "utils/sola_utils.h"

using namespace ns3;
using namespace daisi;

/// TCP Pong server
class Server : public Application {
public:
  static TypeId GetTypeId() {
    static TypeId tid = TypeId("Server").SetParent<Application>().AddConstructor<Server>();
    return tid;
  }

  Server() = default;

  ~Server() override = default;

  void StartApplication() override {
    auto new_msg = [this](network_tcp::TcpSocketHandle handle, const std::string &msg) {
      std::cout << "[SERVER] \"" << msg << "\" from handle " << handle << "\n";
      tcp_->send(handle, "PONG");
    };

    auto connected = [](network_tcp::TcpSocketHandle handle, const std::string &ip, uint16_t port) {
      std::cout << "[SERVER] New connection from " << ip << ":" << port << " WITH HANDLE " << handle
                << "\n";
    };

    auto disconnected = [](network_tcp::TcpSocketHandle /*handle*/) {};

    tcp_ = std::make_unique<network_tcp::Server>(
        network_tcp::ServerCallbacks{new_msg, connected, disconnected});
    std::cout << "[SERVER] Started on " << tcp_->getIP() << ":" << tcp_->getPort() << "\n";
  }

  void StopApplication() override { tcp_.reset(); }

private:
  std::unique_ptr<network_tcp::Server> tcp_;
};

/// TCP client
class Client : public Application {
public:
  static TypeId GetTypeId() {
    static TypeId tid = TypeId("Client").SetParent<Application>();
    return tid;
  }

  explicit Client(network_tcp::Endpoint endpoint) : endpoint_(std::move(endpoint)) {}

  ~Client() override = default;

  void StartApplication() override {
    auto new_msg = [](const std::string & /*msg*/) {};
    auto connected = []() {};
    auto disconnected = []() {};

    tcp_ = std::make_unique<network_tcp::Client>(
        network_tcp::ClientCallbacks{new_msg, connected, disconnected}, endpoint_);
  }

  void StopApplication() override { tcp_.reset(); }

  void sendData(const std::string &msg) { tcp_->send(msg); }

private:
  network_tcp::Endpoint endpoint_;

  std::unique_ptr<network_tcp::Client> tcp_;
};

#define SCHEDULE(number, function, type, seconds)                             \
  ns3::Simulator::ScheduleWithContext(                                        \
      node_container.Get(number)->GetId(), Seconds(seconds), &type::function, \
      node_container.Get(number)->GetApplication(0)->GetObject<type>());

#define SCHEDULE_WITH_ARG(number, function, type, seconds, arg)               \
  ns3::Simulator::ScheduleWithContext(                                        \
      node_container.Get(number)->GetId(), Seconds(seconds), &type::function, \
      node_container.Get(number)->GetApplication(0)->GetObject<type>(), arg);

int main(int /*argc*/, char * /*argv*/[]) {
  NodeContainer node_container;
  node_container.Create(2);

  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("10Gbps"));
  csma.SetChannelAttribute("Delay", StringValue("1ms"));

  NetDeviceContainer net_devices;
  net_devices = csma.Install(node_container);
  csma.EnablePcapAll("output");

  InternetStackHelper stack;
  stack.Install(node_container);

  Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign(net_devices);

  // Start one server and N-1 clients
  for (auto i = 0; i < node_container.GetN(); i++) {
    // Register node for automatic ns-3 socket creation
    std::vector<ns3::Ipv4Address> addresses = daisi::getAddressesForNode(node_container, i);
    daisi::SocketManager::get().registerNode(addresses, node_container.Get(i), 2000);

    if (i == 0) {
      Ptr<Server> app = CreateObject<Server>();
      app->SetStartTime(Seconds(i));
      app->SetStopTime(Seconds(9));

      node_container.Get(i)->AddApplication(app);
    } else {
      network_tcp::Endpoint a{"10.0.0.1", 2000};  // Server endpoint
      Ptr<Client> app = CreateObject<Client>(a);
      app->SetStartTime(Seconds(i));
      app->SetStopTime(Seconds(10));
      node_container.Get(i)->AddApplication(app);
    }
  }

  SCHEDULE_WITH_ARG(1, sendData, Client, 2, "Hello");

  Simulator::Stop(Seconds(20));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
