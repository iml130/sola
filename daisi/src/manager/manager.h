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

#ifndef DAISI_MANAGER_MANAGER_H_
#define DAISI_MANAGER_MANAGER_H_

#include <cassert>
#include <memory>
#include <random>
#include <string>

#include "logging/logger_manager.h"
#include "ns3/arp-cache.h"
#include "ns3/bridge-helper.h"
#include "ns3/config.h"
#include "ns3/csma-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/net-device-container.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "scenariofileparser.h"
#include "sola_constants.h"
#include "sola_helper.h"
#include "utils/random_engine.h"
#include "utils/socket_manager.h"
#include "utils/sola_utils.h"

namespace daisi {

// NODE
static void handleNodeMacTxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (MacTxDrop at Node)!");
}
static void handleNodePhyTxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (PhyTxDrop at Node)!");
}
static void handleNodePhyRxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (PhyRxDrop at Node)!");
}

// SWITCH
static void handleSwitchMacTxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (MacTxDrop at Switch)!");
}

static void handleSwitchPhyTxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (PhyTxDrop at Switch)!");
}

static void handleSwitchPhyRxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (PhyRxDrop at Switch)!");
}

// ROUTER
static void handleRouterMacTxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (MacTxDrop at Router)!");
}

static void handleRouterPhyTxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (PhyTxDrop at Router)!");
}

static void handleRouterPhyRxDrop(ns3::Ptr<const ns3::Packet> packet) {
  throw std::runtime_error("Packet was dropped (PhyRxDrop at Router)!");
}

template <typename ConcreteApplication> class Manager {
public:
  explicit Manager(const std::string &scenariofile_path) : parser_(scenariofile_path) {
    daisi::global_random_engine = std::mt19937_64(parser_.getRandomSeed());
  }

  void initLogger() {
    // Unfortunately the logger cannot be initialized in Manager::Manager(...) as
    // it calls a virtual function.
    daisi::global_logger_manager =
        std::make_unique<daisi::LoggerManager>(parser_.getOutputPath(), getDatabaseFilename());
  }

  ~Manager() {
    using namespace ns3;
    daisi::global_logger_manager->updateTestSetupTime();

    const uint64_t event_count = Simulator::GetEventCount();
    daisi::global_logger_manager->updateTestSetupEventCount(event_count);
    std::cout << "Events executed: " << event_count << std::endl;

    Simulator::Destroy();

    daisi::global_logger_manager.reset();
  }

  virtual void setup() {
    createNodes();
    setupNetwork();
    setupApplication();
  }

  void markAsFailed(const char *exception) { daisi::global_logger_manager->setFailed(exception); }

  void run() {
    if (!daisi::global_logger_manager) {
      throw std::runtime_error("initLogger() must be called before run()!");
    }
    using namespace ns3;
    scheduleEvents();

    uint16_t max_size = 4023;
    std::string content = parser_.getScenariofileContent();
    if (content.size() > max_size) content.resize(max_size);

    uint32_t fanout = 0;
    try {
      fanout = (uint32_t)parser_.getFanout();
    } catch (const std::invalid_argument &e) {
      fanout = 0;
    }

    daisi::LoggerInfoTestSetup info{this->getNumberOfNodes(), fanout,
                                    parser_.getScenariofileContent()};
    daisi::global_logger_manager->logTestSetup(info);

    Simulator::Stop(MilliSeconds(parser_.getStopTime()));

    std::cout << "Simulation Start" << std::endl;
    Simulator::Run();
    std::cout << "Simulation Finished" << std::endl;
  }

protected:
  //! Setup network with static routing
  void setupNetwork() {
    using namespace ns3;

    uint64_t number_of_app_nodes = node_container_.GetN();
    auto number_of_subnets = static_cast<uint64_t>(
        std::ceil((float)number_of_app_nodes / constants::kNumberOfAppNodesPerSwitch));
    assert(number_of_subnets < 255 * 256 - 2);  // Approx. maximum restricted by subnet

    // Create network components/nodes
    switchContainer_.Create(number_of_subnets);
    core_router_.Create(1);

    // Install internet
    InternetStackHelper internet;
    internet.Install(node_container_);
    internet.Install(core_router_);

    // Create switches

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("10Gbps"));

    // Speed of light delay
    // https://en.wikipedia.org/wiki/Velocity_factor#Typical_velocity_factors
    // assuming 10cm/optical fiber: 10cm/0.67c = 498ps
    csma.SetChannelAttribute("Delay", TimeValue(PicoSeconds(498)));

    NetDeviceContainer core_router_devices;
    std::vector<NetDeviceContainer> app_node_devices(number_of_subnets);
    std::vector<NetDeviceContainer> switch_devices(number_of_subnets);

    // Connect app nodes to switchs
    for (uint64_t i = 0; i < number_of_app_nodes; i++) {
      uint32_t network_index = i / constants::kNumberOfAppNodesPerSwitch;
      auto link =
          csma.Install(NodeContainer(node_container_.Get(i), switchContainer_.Get(network_index)));
      app_node_devices[network_index].Add(link.Get(0));
      link.Get(0)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleNodeMacTxDrop));
      link.Get(0)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleNodePhyRxDrop));
      link.Get(0)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleNodePhyTxDrop));
      switch_devices[network_index].Add(link.Get(1));
      link.Get(1)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleSwitchMacTxDrop));
      link.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleSwitchPhyRxDrop));
      link.Get(1)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleSwitchPhyTxDrop));
    }

    // Connect switch to core router
    for (uint32_t i = 0; i < number_of_subnets; i++) {
      auto link = csma.Install(NodeContainer(switchContainer_.Get(i), core_router_.Get(0)));
      switch_devices[i].Add(link.Get(0));
      core_router_devices.Add(link.Get(1));
      link.Get(0)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleSwitchMacTxDrop));
      link.Get(0)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleSwitchPhyRxDrop));
      link.Get(0)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleSwitchPhyTxDrop));
      link.Get(1)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleRouterMacTxDrop));
      link.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleRouterPhyRxDrop));
      link.Get(1)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleRouterPhyTxDrop));
    }

    // Install bridge on each switch
    BridgeHelper bridge;
    for (uint32_t i = 0; i < number_of_subnets; i++) {
      bridge.Install(switchContainer_.Get(i), switch_devices[i]);
    }

    // Install IP
    Ipv4AddressHelper ipv4;
    const uint32_t base_address = 16842752;  // 1.1.0.0
    for (uint32_t i = 0; i < number_of_subnets; i++) {
      uint32_t ip_address = base_address + 65536 * i;
      ipv4.SetBase(Ipv4Address(ip_address), "255.255.0.0");
      ip_container_.push_back(
          ipv4.Assign(NetDeviceContainer(core_router_devices.Get(i), app_node_devices[i])));
    }
    // Routing
    Ipv4StaticRoutingHelper ipv4_routing_helper;

    // Inter-Router routing
    for (uint32_t i = 0; i < number_of_subnets; i++) {
      Ptr<Ipv4StaticRouting> static_routing =
          ipv4_routing_helper.GetStaticRouting(ip_container_[i].Get(0).first);
      for (uint32_t j = 0; j < number_of_subnets; j++) {
        if (i == j) continue;
        Ipv4Address other_subnet_address(base_address + 65536 * j);
        static_routing->AddNetworkRouteTo(other_subnet_address, "255.255.0.0",
                                          j + 1);  // +1 to not send on loopback (index=0)
      }
    }

    // Setup app routing
    for (uint32_t i = 0; i < number_of_subnets; i++) {
      for (uint32_t j = 1; j < ip_container_[i].GetN(); j++) {
        auto static_routing = ipv4_routing_helper.GetStaticRouting(ip_container_[i].Get(j).first);
        auto router_ip = ip_container_[i].GetAddress(0);
        static_routing->AddNetworkRouteTo(Ipv4Address("0.0.0.0"), "0.0.0.0", router_ip, 1);
        interfaces_.Add(ip_container_[i].Get(j));
      }
    }

    // Enable logging of network traffic in pcap files
    // csma.EnablePcapAll("wireshark");

    // Setup ARP caches
    for (int i = 0; i < number_of_subnets; i++) {
      const ns3::Ipv4InterfaceContainer &ip_interfaces = ip_container_[i];
      const ns3::NetDeviceContainer &app_devices = app_node_devices[i];

      ns3::NetDeviceContainer container(core_router_devices.Get(i), app_devices);

      assert(container.GetN() == ip_interfaces.GetN());

      for (int j = 0; j < ip_interfaces.GetN(); j++) {
        auto ip = ip_interfaces.Get(j);
        auto arpcache =
            ip.first->GetObject<Ipv4L3Protocol>()->GetInterface(ip.second)->GetArpCache();

        for (int k = 0; k < ip_interfaces.GetN(); k++) {
          if (j == k) continue;
          auto other_ip = ip_interfaces.GetAddress(k);
          auto other_mac = container.Get(k)->GetAddress();
          ns3::ArpCache::Entry *arp_entry = arpcache->Add(other_ip);
          arp_entry->SetMacAddress(Mac48Address::ConvertFrom(other_mac));
          arp_entry->MarkPermanent();
        }
      }
    }
  }

  void setupApplication() {
    for (uint64_t i = 0; i < node_container_.GetN(); i++) {
      std::vector<ns3::Ipv4Address> addresses = daisi::getAddressesForNode(node_container_, i);

      SocketManager::get().registerNode(addresses, node_container_.Get(i), 2000);

      installApplication<ConcreteApplication>(this->node_container_.Get(i));
      this->node_container_.Get(i)->GetApplication(0)->SetStartTime(ns3::MilliSeconds(0));
    }
  }

  virtual void scheduleEvents() = 0;
  virtual uint64_t getNumberOfNodes() = 0;
  virtual std::string getDatabaseFilename() = 0;

  ScenariofileParser parser_;

  ns3::NodeContainer node_container_{};
  ns3::NodeContainer switchContainer_{};
  ns3::NodeContainer core_router_{};
  ns3::Ipv4InterfaceContainer interfaces_{};
  std::vector<ns3::Ipv4InterfaceContainer> ip_container_;

private:
  void createNodes() {
    node_container_.Create(getNumberOfNodes());
    // for (auto it = node_container_.Begin(); it != node_container_.End(); it++) {
    //   daisi::global_logger_manager->logDevice((*it)->GetId());
    // }
  }
};

}  // namespace daisi

#endif
