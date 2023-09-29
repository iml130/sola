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

#include "core_network.h"

#include <cassert>

#include "manager/sola_constants.h"
#include "ns3/bridge-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "utils/daisi_check.h"

namespace daisi {

// WIFI
static void handleWifiMacTxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (MacTxDrop at Wifi)!");
}

static void handleWifiPhyTxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (PhyTxDrop at Wifi)!");
}

static void handleWifiMacRxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (MacRxDrop at Wifi)!");
}

static void handleWifiDeassociated(ns3::Mac48Address) {
  throw std::runtime_error("wifi connection lost (deassociated from access point)");
}

static void installDefaultWifiTraces() {
  ns3::Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop",
                                     MakeCallback(&handleWifiMacTxDrop));
  ns3::Config::ConnectWithoutContext(
      "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::StaWifiMac/DeAssoc",
      MakeCallback(&handleWifiDeassociated));
  ns3::Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRxDrop",
                                     MakeCallback(&handleWifiMacRxDrop));
  ns3::Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop",
                                     MakeCallback(&handleWifiPhyTxDrop));
}

// NODE
static void handleNodeMacTxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (MacTxDrop at Node)!");
}
static void handleNodePhyTxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (PhyTxDrop at Node)!");
}
static void handleNodePhyRxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (PhyRxDrop at Node)!");
}

// SWITCH
static void handleSwitchMacTxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (MacTxDrop at Switch)!");
}

static void handleSwitchPhyTxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (PhyTxDrop at Switch)!");
}

static void handleSwitchPhyRxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (PhyRxDrop at Switch)!");
}

// ROUTER
static void handleRouterMacTxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (MacTxDrop at Router)!");
}

static void handleRouterPhyTxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (PhyTxDrop at Router)!");
}

static void handleRouterPhyRxDrop(ns3::Ptr<const ns3::Packet>) {
  throw std::runtime_error("Packet was dropped (PhyRxDrop at Router)!");
}

CoreNetwork::CoreNetwork() : core_router_(ns3::CreateObject<ns3::Node>()) {
  ns3::InternetStackHelper internet;
  internet.Install(core_router_);
}

/// @brief Get all NetDevices attached to \p node
ns3::NetDeviceContainer getNetDevices(const ns3::Ptr<ns3::Node> &node) {
  ns3::NetDeviceContainer devices;
  for (uint32_t j = 0; j < node->GetNDevices(); j++) {
    devices.Add(node->GetDevice(j));
  }

  return devices;
}

void CoreNetwork::addNodesCSMA(ns3::NodeContainer nodes) {
  using namespace ns3;

  const uint64_t number_of_app_nodes = nodes.GetN();
  const auto number_of_subnets = static_cast<uint64_t>(
      std::ceil((float)number_of_app_nodes / constants::kMaxNumberNodesPerCsmaSubnet));
  assert(number_of_subnets < 255 * 256 - 2);  // Approx. maximum restricted by subnet

  // Create network components/nodes
  ns3::NodeContainer switch_container;
  switch_container.Create(number_of_subnets);

  // Install internet
  InternetStackHelper internet;
  internet.Install(nodes);

  CsmaHelper csma = setupCSMA();

  NetDeviceContainer core_router_devices;
  std::vector<NetDeviceContainer> app_node_devices(number_of_subnets);

  // Connect app nodes to switchs
  for (uint64_t i = 0; i < nodes.GetN(); i++) {
    const uint32_t network_index = i / constants::kMaxNumberNodesPerCsmaSubnet;
    auto link = csma.Install(NodeContainer(nodes.Get(i), switch_container.Get(network_index)));
    app_node_devices[network_index].Add(link.Get(0));
    link.Get(0)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleNodeMacTxDrop));
    link.Get(0)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleNodePhyRxDrop));
    link.Get(0)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleNodePhyTxDrop));
    link.Get(1)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleSwitchMacTxDrop));
    link.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleSwitchPhyRxDrop));
    link.Get(1)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleSwitchPhyTxDrop));
  }

  // Connect switch to core router
  for (uint32_t i = 0; i < number_of_subnets; i++) {
    auto link = csma.Install(NodeContainer(switch_container.Get(i), core_router_));
    core_router_devices.Add(link.Get(1));
    link.Get(0)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleSwitchMacTxDrop));
    link.Get(0)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleSwitchPhyRxDrop));
    link.Get(0)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleSwitchPhyTxDrop));
    link.Get(1)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleRouterMacTxDrop));
    link.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleRouterPhyRxDrop));
    link.Get(1)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleRouterPhyTxDrop));
  }

  // // Install bridge on each switch
  BridgeHelper bridge;
  for (uint32_t i = 0; i < number_of_subnets; i++) {
    const ns3::Ptr<ns3::Node> switch_node = switch_container.Get(i);
    bridge.Install(switch_node, getNetDevices(switch_node));
  }

  std::vector<ns3::Ipv4InterfaceContainer> ip_container =
      installIP(number_of_subnets, core_router_devices, app_node_devices, next_base_address_csma_);

  // Enable logging of network traffic in pcap files
  // csma.EnablePcapAll("wireshark");

  setGatewayForAppNodes(number_of_subnets, ip_container);
}

void CoreNetwork::addNodesWifi(ns3::NodeContainer nodes, double topology_width,
                               double topology_height) {
  using namespace ns3;

  // Setup APs
  const uint32_t number_ap = std::ceil(nodes.GetN() / (double)constants::kMaxNumberNodesPerAp);

  ns3::NodeContainer access_points;
  access_points.Create(number_ap);

  // Install APs all in middle of topology
  MobilityHelper mob;
  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(topology_width / 2.0),
                           "MinY", DoubleValue(topology_height / 2.0), "Z", DoubleValue(8.0),
                           "DeltaX", DoubleValue(0.0), "DeltaY", DoubleValue(0.0), "GridWidth",
                           UintegerValue(20), "LayoutType", StringValue("RowFirst"));
  mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");

  mob.Install(access_points);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper wifi_phy;
  wifi_phy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

  // Assign nodes to AP
  std::vector<NodeContainer> nodes_per_ap_vec;
  nodes_per_ap_vec.resize(number_ap);
  for (uint32_t i = 0; i < nodes.GetN(); i++) {
    nodes_per_ap_vec[i / constants::kMaxNumberNodesPerAp].Add(nodes.Get(i));
  }

  // Install internet
  InternetStackHelper stack;
  stack.Install(nodes);

  NetDeviceContainer router_net_devices;
  std::vector<NetDeviceContainer> sta_net_device;

  for (uint32_t i = 0; i < access_points.GetN(); i++) {
    wifi_phy.SetChannel(channel.Create());
    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::IdealWifiManager");
    wifi.SetRemoteStationManager("ns3::IdealWifiManager", "RtsCtsThreshold", UintegerValue(100));
    wifi.SetStandard(WifiStandard::WIFI_STANDARD_80211ac);

    // As long as we do not use handover of a STA between APs, each
    // STA is always attached to the same AP.
    // As the number of STAs can be large and only a small number of STAs will be attached
    // to a single AP, we need many APs. Hence we (unrealistically) increase the frequency
    // for every AP by 1. This works as ns-3 does not model radio interferences yet.
    wifi_phy.Set("Frequency", UintegerValue(i + 1));
    wifi_phy.Set("ChannelWidth", UintegerValue(20));
    wifi_phy.Set("TxPowerStart", DoubleValue(constants::kMaxPowerDBm));
    wifi_phy.Set("TxPowerEnd", DoubleValue(constants::kMaxPowerDBm));

    WifiMacHelper wifi_mac;
    Ssid ssid("IMLwifi" + std::to_string(i));
    wifi_mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing",
                     BooleanValue(false), "WaitBeaconTimeout", TimeValue(Seconds(2.56)));

    // Setup wifi on AMR
    NetDeviceContainer sta_dev_temp;
    sta_dev_temp = wifi.Install(wifi_phy, wifi_mac, nodes_per_ap_vec[i]);
    sta_net_device.push_back(sta_dev_temp);

    // set up the AP
    bool beacon_generation = true;
#ifdef DAISI_SOLANET_NS3_DISABLE_NETWORKING
    beacon_generation = false;
#endif

    wifi_mac.SetType("ns3::ApWifiMac", "QosSupported", BooleanValue(true), "Ssid", SsidValue(ssid),
                     "BeaconInterval", TimeValue(Seconds(2.56)), "BeaconGeneration",
                     ns3::BooleanValue(beacon_generation));
    auto ap_sta_dev_temp = wifi.Install(wifi_phy, wifi_mac, access_points.Get(i));

    // Connection between AP and core router
    CsmaHelper csma = setupCSMA();

    auto link = csma.Install(NodeContainer(access_points.Get(i), core_router_));
    router_net_devices.Add(link.Get(1));

    link.Get(0)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleSwitchMacTxDrop));
    link.Get(0)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleSwitchPhyRxDrop));
    link.Get(0)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleSwitchPhyTxDrop));
    link.Get(1)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleRouterMacTxDrop));
    link.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleRouterPhyRxDrop));
    link.Get(1)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleRouterPhyTxDrop));

    // Bridge between cable and wireless on AP
    BridgeHelper bridge;
    bridge.Install(access_points.Get(i), ns3::NetDeviceContainer(ap_sta_dev_temp, link.Get(0)));
  }

  // Assign IPs
  std::vector<ns3::Ipv4InterfaceContainer> ip_container =
      installIP(access_points.GetN(), router_net_devices, sta_net_device, next_base_address_wifi_);

  setGatewayForAppNodes(access_points.GetN(), ip_container);

  installDefaultWifiTraces();
}

std::vector<ns3::Ipv4InterfaceContainer> CoreNetwork::installIP(
    uint32_t number_of_subnets, ns3::NetDeviceContainer core_router_devices,
    std::vector<ns3::NetDeviceContainer> app_node_devices, uint32_t &base_address) {
  ns3::Ipv4AddressHelper ipv4;
  std::vector<ns3::Ipv4InterfaceContainer> ip_container;

  for (uint32_t i = 0; i < number_of_subnets; i++) {
    ipv4.SetBase(ns3::Ipv4Address(base_address), "255.255.0.0");
    ip_container.push_back(
        ipv4.Assign(ns3::NetDeviceContainer(core_router_devices.Get(i), app_node_devices[i])));
    base_address += 65536;
  }

  return ip_container;
}

ns3::CsmaHelper CoreNetwork::setupCSMA() {
  using namespace ns3;
  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue("10Gbps"));

  // // Speed of light delay
  // // https://en.wikipedia.org/wiki/Velocity_factor#Typical_velocity_factors
  // // assuming 10cm/optical fiber: 10cm/0.67c = 498ps
  csma.SetChannelAttribute("Delay", TimeValue(PicoSeconds(498)));
  return csma;
}

void CoreNetwork::setGatewayForAppNodes(uint32_t number_of_subnets,
                                        std::vector<ns3::Ipv4InterfaceContainer> ip_container) {
  ns3::Ipv4StaticRoutingHelper ipv4_routing_helper;

  for (uint32_t i = 0; i < number_of_subnets; i++) {
    const ns3::Ipv4Address router_ip = ip_container[i].GetAddress(0);
    for (uint32_t j = 1; j < ip_container[i].GetN(); j++) {
      auto static_routing = ipv4_routing_helper.GetStaticRouting(ip_container[i].Get(j).first);

      // Default route/gateway
      static_routing->AddNetworkRouteTo(ns3::Ipv4Address("0.0.0.0"), "0.0.0.0", router_ip, 1);
    }
  }
}

}  // namespace daisi
