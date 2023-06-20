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

#include "path_planning/path_planning_manager.h"

#include <cassert>

#include "cpps/amr/physical/amr_mobility_model_ns3.h"
#include "cpps/common/uuid_generator.h"
#include "delivery_station.h"
#include "destination.h"
#include "intersection_set.h"
#include "minhton/logging/logger.h"
#include "ns3/core-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/wifi-module.h"
#include "station.h"
#include "utils/random_engine.h"
#include "utils/sola_utils.h"

#define getParsed(TYPE, NAME) parser_.getParsedContent()->getRequired<TYPE>(NAME)

using namespace ns3;

namespace daisi::path_planning {

PathPlanningManager::PathPlanningManager(const std::string &scenario_config_file)
    : Manager<PathPlanningApplication>(scenario_config_file) {
  // Setup logging
  Manager::initLogger();

  parse();

  topology_ = cpps::TopologyNs3(
      {6 + 6 + static_cast<double>(number_delivery_stations_) / 2 * delta_stations_,
       1 + 5 + (static_cast<double>(number_pickup_stations_) / 2 * delta_stations_) + 1 + 5, 0});

  // TODO Workaround - Simple task for logging should not depend on possible AGV kinematics
  cpps::amr::AmrStaticAbility ability(
      cpps::amr::LoadCarrier(cpps::amr::LoadCarrier::Types::kEuroBox), 100.0F);
  cpps::AGVFleet::init({{ability, kinematics_}});
}

void PathPlanningManager::setup() {
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/root.yml");
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/join.yml");

  setupNodes();

  // Setup AGVs
  assert(this->nodeContainer_.GetN() == getNumberOfNodes());

  if (number_pickup_stations_ % 2 != 0 || number_delivery_stations_ % 2 != 0) {
    throw std::runtime_error("number stations must be even");
  }

  // 4m between stations of same type
  ns3::MobilityHelper mob;
  const double delta_x_pickup =
      5 + 5 + (static_cast<double>(number_delivery_stations_) / 2 - 1) * delta_stations_;
  left_border_ = 1;
  right_border_ = left_border_ + delta_x_pickup;

  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", ns3::DoubleValue(left_border_),
                           "MinY", ns3::DoubleValue(6.0), "DeltaX",
                           ns3::DoubleValue(delta_x_pickup), "DeltaY",
                           ns3::DoubleValue(delta_stations_), "GridWidth",
                           ns3::UintegerValue(number_pickup_stations_ / 2), "LayoutType",
                           ns3::StringValue("ColumnFirst"));

  mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mob.Install(pickup_stations_);

  const double delta_y_delivery =
      5 + 5 + (static_cast<double>(number_pickup_stations_) / 2 - 1) * delta_stations_;
  lower_border_ = 1;
  upper_border_ = lower_border_ + delta_y_delivery;

  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", ns3::DoubleValue(6), "MinY",
                           ns3::DoubleValue(lower_border_), "DeltaX",
                           ns3::DoubleValue(delta_stations_), "DeltaY",
                           ns3::DoubleValue(delta_y_delivery), "GridWidth",
                           ns3::UintegerValue(number_delivery_stations_ / 2), "LayoutType",
                           ns3::StringValue("RowFirst"));

  mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mob.Install(delivery_stations_);

  // Spawn AGVs
  // AMRs are initially filled to increasing pickup station numbers
  // Dummy, will be overwritten later
  mob.SetPositionAllocator("ns3::RandomBoxPositionAllocator", "X",
                           StringValue("ns3::UniformRandomVariable[Min=0|Max=1]"), "Y",
                           StringValue("ns3::UniformRandomVariable[Min=0|Max=1]"), "Z",
                           StringValue("ns3::UniformRandomVariable[Min=0|Max=1]"));

  mob.SetMobilityModel("ns3::AmrMobilityModelNs3");
  mob.Install(agvs_);
  for (int i = 0; i < agvs_.GetN(); i++) {
    // Set position to station queue
    ns3::Vector3D pickup_queue_pos = pickup_stations_.Get(getAGVInitialStationNumber(i))
                                         ->GetObject<MobilityModel>()
                                         ->GetPosition();
    std::cout << "SET AGV POS " << pickup_queue_pos.x << ":" << pickup_queue_pos.y << std::endl;
    agvs_.Get(i)->GetObject<MobilityModel>()->SetPosition(pickup_queue_pos);
  }

  // Install general information for application
  std::for_each(nodeContainer_.Begin(), nodeContainer_.End(), [this](ns3::Ptr<Node> node) {
    for (int i = 0; i < node->GetNApplications(); i++) {
      node->GetApplication(i)->GetObject<PathPlanningApplication>()->setTopology(topology_);
    }
  });
}

uint64_t PathPlanningManager::getNumberOfNodes() {
  const uint32_t number_central_consensus =
      std::holds_alternative<consensus::CentralSettings>(consensus_settings_) ? 1 : 0;
  return number_agvs_ + number_delivery_stations_ + number_pickup_stations_ +
         number_central_consensus;
}

void PathPlanningManager::initAGV(uint32_t index) {
  cpps::AgvDeviceDescription desc;
  desc.serial_number = index;
  auto mobility =
      DynamicCast<cpps::AmrMobilityModelNs3>(agvs_.Get(index)->GetObject<ns3::MobilityModel>());
  desc.mobility = mobility;

  cpps::AgvDeviceProperties properties;
  properties.device_type = "AGV";
  properties.model_name = "Mk42";
  properties.model_number = 0;
  properties.manufacturer = "FhG";

  // Set dummy ability
  properties.ability = cpps::amr::AmrStaticAbility(cpps::amr::LoadCarrier("package"), 100);

  properties.kinematic = kinematics_;

  this->agvs_.Get(index)->GetApplication(1)->GetObject<PathPlanningApplication>()->initAGVPhysical(
      cpps::AgvDataModel{desc, properties}, index);
  this->agvs_.Get(index)->GetApplication(0)->GetObject<PathPlanningApplication>()->initAGVLogical(
      consensus_settings_, index == 0);
}

void PathPlanningManager::initPickupStation(uint32_t index) {
  std::vector<PickupStationHandoverInfo> pickup_registry = getPickupStationRegistry();
  const NextTOMode next_to_mode = nextToModeFromString(getParsed(std::string, "nextTOMode"));

  ns3::Vector3D pos = pickup_stations_.Get(index)->GetObject<MobilityModel>()->GetPosition();
  std::cout << "PICKUP STATION " << pos.x << ":" << pos.y << std::endl;

  PickupStationInfo info;
  info.station_id = index;
  info.center_pos = {pos.x, pos.y};
  info.pickup_station_registry = pickup_registry;
  if (pos.x == left_border_) {  // "Left" side
    info.out_pickup = ns3::Vector2D(pos.x + 0.5, pos.y - 0.5);
    info.in_pickup = ns3::Vector2D(pos.x + 0.5, pos.y + 0.5);
  } else if (pos.x == right_border_) {  // "Right" side
    info.out_pickup = ns3::Vector2D(pos.x - 0.5, pos.y + 0.5);
    info.in_pickup = ns3::Vector2D(pos.x - 0.5, pos.y - 0.5);
  } else {
    throw std::runtime_error("pickup station mus be at left or right border");
  }
  pickup_stations_.Get(index)
      ->GetApplication(0)
      ->GetObject<PathPlanningApplication>()
      ->initPickupStation(info, consensus_settings_, next_to_mode);
}

void PathPlanningManager::postInitPickupStation(uint32_t index) {
  this->pickup_stations_.Get(index)
      ->GetApplication(0)
      ->GetObject<PathPlanningApplication>()
      ->postInit();
}

void PathPlanningManager::initDeliveryStation(uint32_t index) {
  ns3::Vector3D pos = delivery_stations_.Get(index)->GetObject<MobilityModel>()->GetPosition();
  std::cout << "DELIVERY STATION " << pos.x << ":" << pos.y << std::endl;
  DeliveryStationInfo info;
  info.station_id = pickup_stations_.GetN() + index;
  info.center_pos = {pos.x, pos.y};
  delivery_stations_.Get(index)
      ->GetApplication(0)
      ->GetObject<PathPlanningApplication>()
      ->initDeliveryStation(info, consensus_settings_);
}

void PathPlanningManager::postInitDeliveryStation(uint32_t index) {
  this->delivery_stations_.Get(index)
      ->GetApplication(0)
      ->GetObject<PathPlanningApplication>()
      ->postInit();
}

void PathPlanningManager::initCentralConsensus() {
  central_consensus_.Get(0)
      ->GetApplication(0)
      ->GetObject<PathPlanningApplication>()
      ->initConsensusCentralServer(std::get<consensus::CentralSettings>(consensus_settings_));
}

uint32_t PathPlanningManager::getAGVInitialStationNumber(uint32_t agv_id) const {
  return agv_id % pickup_stations_.GetN();
}

void PathPlanningManager::registerAGVByAuthority(uint32_t index) {
  const uint32_t pickup_station_number = getAGVInitialStationNumber(index);

  // Set position to station queue
  std::string station_connection =
      std::get<PickupStation>(this->pickup_stations_.Get(pickup_station_number)
                                  ->GetApplication(0)
                                  ->GetObject<PathPlanningApplication>()
                                  ->application)
          .getConnectionString();

  std::get<AGVLogical>(
      agvs_.Get(index)->GetApplication(0)->GetObject<PathPlanningApplication>()->application)
      .registerByAuthority(station_connection);
}

void PathPlanningManager::connect(int index) {
  auto agv = std::get<cpps::AGVPhysicalBasic>(
      this->agvs_.Get(index)->GetApplication(1)->GetObject<PathPlanningApplication>()->application);
  auto cpps_app_logical =
      this->agvs_.Get(index)->GetApplication(0)->GetObject<PathPlanningApplication>();

  ns3::Ipv4Address other_ip = cpps_app_logical->local_ip_address_tcp;
  uint16_t other_port = cpps_app_logical->listening_port_tcp;
  InetSocketAddress address(other_ip, other_port);
  agv.connect(address);
}

void PathPlanningManager::setupNodes() {
  if (std::holds_alternative<consensus::CentralSettings>(consensus_settings_)) {
    central_consensus_.Create(1);
  }
  agvs_.Create(number_agvs_);
  pickup_stations_.Create(number_pickup_stations_);
  delivery_stations_.Create(number_delivery_stations_);
  setupNetworkEthernet();
  setupNetworkWifi();

  nodeContainer_ =
      NodeContainer(NodeContainer(pickup_stations_, delivery_stations_, central_consensus_), agvs_);
  for (auto i = pickup_stations_.GetN() + delivery_stations_.GetN() + central_consensus_.GetN();
       i < nodeContainer_.GetN(); i++) {
    addresses_[i].push_back("127.0.0.1");
  }

  // Setting up TransportOrderApplications and one AGV application
  setupApplication();

  // Setup second applications for physical AGV
  for (uint32_t i = 0; i < agvs_.GetN(); i++) {
    SolaHelper<PathPlanningApplication> helper({"127.0.0.1"}, 4000);
    helper.install(this->agvs_.Get(i));
  }
}

void PathPlanningManager::setupNetworkEthernet() {
  // Use the manager to set up ethernet between specific nodes only
  // Rest of exclusive network setup for cpps is handled here.
  nodeContainer_ = NodeContainer(pickup_stations_, delivery_stations_, central_consensus_);

  setupNetwork();

  if (central_consensus_.GetN() != 0) {
    MobilityHelper mob;
    mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(0), "MinY",
                             DoubleValue(0), "Z", DoubleValue(8.0), "DeltaX", DoubleValue(0.0),
                             "DeltaY", DoubleValue(0.0), "GridWidth", UintegerValue(20),
                             "LayoutType", StringValue("RowFirst"));
    mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    mob.Install(central_consensus_);

    auto ip = ip_container_[ip_container_.size() - 1].Get(
        ip_container_[ip_container_.size() - 1].GetN() - 1);
    auto ip_addr = ip.first->GetAddress(1, 0);
    std::string central_ip = daisi::getIpv4AddressString(ip_addr.GetAddress());
    std::get<consensus::CentralSettings>(consensus_settings_).server_ip = central_ip;
    std::get<consensus::CentralSettings>(consensus_settings_).server_port = 2000;
  }
}

void PathPlanningManager::setupNetworkWifi() {
  NodeContainer wifi_participants = NodeContainer(agvs_);

  // Create APs
  const uint32_t number_ap =
      std::ceil(wifi_participants.GetN() / (double)constants::kMaxNumberAgvsPerAp);
  access_points_.Create(number_ap);

  // Install APs all in middle of topology
  MobilityHelper mob;
  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX",
                           DoubleValue(topology_.getWidth() / 2.0), "MinY",
                           DoubleValue(topology_.getHeight() / 2.0), "Z", DoubleValue(8.0),
                           "DeltaX", DoubleValue(0.0), "DeltaY", DoubleValue(0.0), "GridWidth",
                           UintegerValue(20), "LayoutType", StringValue("RowFirst"));
  mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");

  mob.Install(access_points_);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper wifi_phy = YansWifiPhyHelper();
  wifi_phy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  Ipv4AddressHelper ip;
  ip.SetBase("192.168.0.0", "255.255.0.0");

  // Assign wifi_participants to AP
  std::vector<NodeContainer> participants_per_ap;
  participants_per_ap.resize(number_ap);
  for (uint32_t i = 0; i < wifi_participants.GetN(); i++) {
    participants_per_ap[i / constants::kMaxNumberAgvsPerAp].Add(wifi_participants.Get(i));
  }

  const uint32_t base_address = 3232235520;  // 192.168.0.0

  // Install internet
  InternetStackHelper stack;
  stack.Install(wifi_participants);

  for (uint32_t i = 0; i < access_points_.GetN(); i++) {
    wifi_phy.SetChannel(channel.Create());
    WifiHelper wifi = WifiHelper();
    wifi.SetRemoteStationManager("ns3::IdealWifiManager");
    wifi.SetRemoteStationManager("ns3::IdealWifiManager", "RtsCtsThreshold", UintegerValue(100));
    wifi.SetStandard(WifiStandard::WIFI_STANDARD_80211ac);

    // As long as we do not use handover of an STA between APs, each
    // STA is always attached to the same AP.
    // As the number of STAs can be large and only a small number of STAs will be attached
    // to a single AP, we need many APs. Hence we (unrealistically) increase the frequency
    // for every AP by 1. This works as ns-3 does not model radio interferences yet.
    wifi_phy.Set("Frequency", UintegerValue(i + 1));
    wifi_phy.Set("ChannelWidth", UintegerValue(20));
    WifiMacHelper wifi_mac;
    Ssid ssid = Ssid("IMLwifi" + std::to_string(i));
    wifi_mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing",
                     BooleanValue(false));

    // Setup Wi-Fi on AGV
    NetDeviceContainer participants_sta_dev_temp;
    wifi_phy.Set("TxPowerStart", DoubleValue(constants::kMaxPowerDBm));
    wifi_phy.Set("TxPowerEnd", DoubleValue(constants::kMaxPowerDBm));
    participants_sta_dev_temp = wifi.Install(wifi_phy, wifi_mac, participants_per_ap[i]);

    // set up the AP
    bool beacon_generation = true;
#ifdef DAISI_SOLANET_NS3_DISABLE_NETWORKING
    beacon_generation = false;
#endif

    wifi_mac.SetType("ns3::ApWifiMac", "QosSupported", BooleanValue(true), "Ssid", SsidValue(ssid),
                     "BeaconInterval", TimeValue(Seconds(2.56)), "BeaconGeneration",
                     ns3::BooleanValue(beacon_generation));
    auto temp_ap_sta_dev = wifi.Install(wifi_phy, wifi_mac, access_points_.Get(i));

    wifi_phy.Set("TxPowerStart", DoubleValue(0));
    wifi_phy.Set("TxPowerEnd", DoubleValue(constants::kMaxPowerDBm));
    wifi_phy.Set("TxPowerLevels", UintegerValue(constants::kMaxPowerDBm + 1.0));

    // Connection between AP and core router
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("10Gbps"));
    // Speed of light delay
    // https://en.wikipedia.org/wiki/Velocity_factor#Typical_velocity_factors
    // assuming 10cm/optical fiber: 10cm/0.67c = 498ps
    csma.SetChannelAttribute("Delay", TimeValue(PicoSeconds(498)));
    auto link = csma.Install(NodeContainer(access_points_.Get(i), core_router_.Get(0)));

    // Bridge between cable and wireless on AP
    BridgeHelper bridge;
    bridge.Install(access_points_.Get(i), ns3::NetDeviceContainer(temp_ap_sta_dev, link.Get(0)));

    // Assign IPs
    const uint32_t ip_address = base_address + 65536 * i;
    ip.SetBase(Ipv4Address(ip_address), "255.255.0.0");
    auto router_ip = ip.Assign(link.Get(1));
    auto sta_interface = ip.Assign(participants_sta_dev_temp);

    ns3::Ipv4InterfaceContainer ips;
    ips.Add(router_ip);
    ips.Add(sta_interface);
    ip_container_.push_back(ips);

    // Add routing
    const uint32_t port_on_router = switchContainer_.GetN() + i;

    // Add AP as destination for routing to all other router ports
    Ipv4StaticRoutingHelper ipv4_routing_helper;
    for (uint32_t j = 0; j < ip_container_.size() - 1; j++) {
      Ptr<Ipv4StaticRouting> static_routing =
          ipv4_routing_helper.GetStaticRouting(ip_container_[j].Get(0).first);
      static_routing->AddNetworkRouteTo(
          Ipv4Address(ip_address), "255.255.0.0",
          port_on_router + 1);  // +1 to not send on loopback (index=0)
    }

    // Setup app routing
    for (uint32_t j = 1; j < ip_container_[ip_container_.size() - 1].GetN(); j++) {
      auto static_routing = ipv4_routing_helper.GetStaticRouting(
          ip_container_[ip_container_.size() - 1].Get(j).first);
      auto current_router_ip = ip_container_[ip_container_.size() - 1].GetAddress(0);
      static_routing->AddNetworkRouteTo(Ipv4Address("0.0.0.0"), "0.0.0.0", current_router_ip, 1);
      interfaces_.Add(ip_container_[ip_container_.size() - 1].Get(j));
    }

    for (uint32_t j = 0; j < participants_per_ap[i].GetN(); j++) {
      addresses_.push_back({sta_interface.GetAddress(j)});
    }

    // set up ARP caches for AGV and core router
    // Need to do this after starting the simulation
    // TODO: That all wifi connections are set up after 1000 ms is just a wild guess and might not
    // work in all situations.
    //  We have the same problem when scheduling the normal events as we do not know when the device
    //  is connected. Might schedule the events only after all nodes confirmed network connection in
    //  the future.
    // See https://gitlab.com/nsnam/ns-3-dev/-/issues/664
    ns3::Simulator::Schedule(ns3::MilliSeconds(1000), &PathPlanningManager::setupArp, this,
                             ns3::NetDeviceContainer(link.Get(1), participants_sta_dev_temp), ips);
  }

  daisi::installDefaultWifiTraces();
}

// TODO Remove Ipv4InterfaceContainer (should directly get IP from Node)
void PathPlanningManager::setupArp(const ns3::NetDeviceContainer &devices,
                                   const ns3::Ipv4InterfaceContainer &ips) {
  assert(ips.GetN() == devices.GetN());
  for (int j = 0; j < devices.GetN(); j++) {
    auto ip = ips.Get(j);
    auto arpcache = ip.first->GetObject<Ipv4L3Protocol>()->GetInterface(ip.second)->GetArpCache();

    for (int k = 0; k < ips.GetN(); k++) {
      if (j == k) continue;
      auto other_ip = ips.GetAddress(k);
      auto other_mac = devices.Get(k)->GetAddress();
      ns3::ArpCache::Entry *arp_entry = arpcache->Add(other_ip);
      arp_entry->SetMacAddress(Mac48Address::ConvertFrom(other_mac));
      uint8_t buff[6];
      Mac48Address::ConvertFrom(other_mac).CopyTo(buff);
      arp_entry->MarkPermanent();
    }
  }
}

void PathPlanningManager::setRouteInfo() { throw std::runtime_error("not implemented"); }

void PathPlanningManager::checkSimulationFinished() {
  uint32_t agvs_at_pickup = std::accumulate(
      pickup_stations_.Begin(), pickup_stations_.End(), 0,
      [](const uint32_t &cur_val, const ns3::Ptr<Node> &node) {
        return cur_val +
               std::get<PickupStation>(
                   node->GetApplication(0)->GetObject<PathPlanningApplication>()->application)
                   .numberAGVAtStation();
      });
  if (agvs_at_pickup == agvs_.GetN()) {
    // All back at station
    ns3::Simulator::Stop();
  } else {
    Simulator::Schedule(MilliSeconds(500), &PathPlanningManager::checkSimulationFinished, this);
  }
}

void PathPlanningManager::scheduleEvents() {
  uint64_t current_time = Simulator::Now().GetMilliSeconds();
  auto delay = parser_.getParsedContent()->getRequired<uint64_t>("defaultDelay");
  for (auto i = 0U; i < number_agvs_; i++) {
    current_time += delay;
    Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::initAGV, this, i);
  }

  for (auto i = 0U; i < number_agvs_; i++) {
    current_time += delay;
    Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::connect, this, i);
  }

  // Start stations
  for (auto i = 0U; i < pickup_stations_.GetN(); i++) {
    current_time += delay;

    Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::initPickupStation, this,
                        i);
  }

  for (auto i = 0U; i < delivery_stations_.GetN(); i++) {
    current_time += delay;

    Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::initDeliveryStation, this,
                        i);
  }

  // Start central consensus server
  if (central_consensus_.GetN() != 0) {
    current_time += delay;
    Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::initCentralConsensus,
                        this);
  }

  // Calculate and set route info
  current_time += delay;
  Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::setRouteInfo, this);

  // Register AGVs with parking manager
  for (auto i = 0U; i < number_agvs_; i++) {
    current_time += delay;
    Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::registerAGVByAuthority,
                        this, i);
  }

  // Start consensus
  for (auto i = 0U; i < pickup_stations_.GetN(); i++) {
    current_time += 5000;

    Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::postInitPickupStation,
                        this, i);
  }

  for (auto i = 0U; i < delivery_stations_.GetN(); i++) {
    current_time += 5000;

    Simulator::Schedule(MilliSeconds(current_time), &PathPlanningManager::postInitDeliveryStation,
                        this, i);
  }

  current_time += 5000;

  // Schedule TO spawn
  for (int i = 0; i < pickup_stations_.GetN(); i++) {
    current_time += 1000;
    ns3::Simulator::Schedule(ns3::MilliSeconds(current_time), &PathPlanningManager::scheduleSpawnTO,
                             this, i);
  }

  // Wait 1 second extra to check if all AMRs returned, and we can stop
  ns3::Simulator::Schedule(MilliSeconds(current_time + to_spawn_duration_ms_ + 1000),
                           &PathPlanningManager::checkSimulationFinished, this);
}

void PathPlanningManager::scheduleSpawnTO(uint32_t pickup_index) {
  const int64_t current_time = ns3::Simulator::Now().GetMilliSeconds();
  std::get<PickupStation>(pickup_stations_.Get(pickup_index)
                              ->GetApplication(0)
                              ->GetObject<PathPlanningApplication>()
                              ->application)
      .scheduleSpawnTO(current_time, current_time + to_spawn_duration_ms_, [this]() {
        TransportOrderInfo info;
        info.state = TOState::kIntake;
        info.uuid = UUIDGenerator::get()();
        std::uniform_int_distribution<uint32_t> delivery_station_dist(
            0, number_delivery_stations_ - 1);
        info.delivery_station =
            pickup_stations_.GetN() + delivery_station_dist(daisi::global_random_engine);
        return info;
      });
}

void PathPlanningManager::parse() {
  // TODO Check that parsed arguments fit into uint32_t
  number_pickup_stations_ = getParsed(uint64_t, "pickupStations");
  number_delivery_stations_ = getParsed(uint64_t, "deliveryStations");

  number_agvs_ = getParsed(uint64_t, "agvs");

  auto max_velo = getParsed(float, "max_velo");
  auto min_velo = getParsed(float, "min_velo");
  auto max_acc = getParsed(float, "max_acc");
  auto min_acc = getParsed(float, "min_acc");
  auto load_time_s = getParsed(float, "load_time_s");
  auto unload_time_s = getParsed(float, "unload_time_s");
  kinematics_ = cpps::Kinematics(max_velo, min_velo, max_acc, min_acc, load_time_s, unload_time_s);

  parseConsensusSettings();
  parseScenarioSequence();
}

void PathPlanningManager::parseConsensusSettings() {
  consensus_type_ = consensus::consensusTypeFromString(getParsed(std::string, "consensus"));
  double time_between_intersects = getParsed(float, "timeBetweenIntersections");
  double max_preplanning_time = getParsed(float, "maxPreplanningTimeBeforeReject");
  if (consensus_type_ == consensus::ConsensusType::kPaxos) {
    bool replication = getParsed(uint64_t, "paxosReplication");
    consensus_settings_ =
        consensus::PaxosSettings{.pickup_active_participate = true,
                                 .delivery_active_participate = false,
                                 .amr_active_participate = false,
                                 .replication = replication,
                                 .number_paxos_participants = number_pickup_stations_,
                                 .time_delta_intersections = time_between_intersects,
                                 .max_preplanning_time = max_preplanning_time};
  } else if (consensus_type_ == consensus::ConsensusType::kCentral) {
    consensus_settings_ =
        consensus::CentralSettings{"N/A", 0, time_between_intersects, max_preplanning_time};
  }
}

void PathPlanningManager::parseScenarioSequence() {
  auto scenario_sequence =
      parser_.getParsedContent()
          ->getRequired<std::vector<std::shared_ptr<ScenariofileParser::Table>>>(
              "scenarioSequence");

  for (const auto &spawn : scenario_sequence) {
    auto spawn_inner = spawn->content.begin()->second;
    auto spawn_description = *std::get_if<std::shared_ptr<ScenariofileParser::Table>>(&spawn_inner);

    auto type = spawn_description->getRequired<std::string>("type");

    if (type == "to") {
      parseToSpawn(spawn_description);
    } else {
      throw std::runtime_error("Encountered invalid type while parsing scenario sequence!");
    }
  }
}

void PathPlanningManager::parseToSpawn(
    const std::shared_ptr<ScenariofileParser::Table> &spawn_description) {
  to_spawn_duration_ms_ = spawn_description->getRequired<uint64_t>("duration");
}

std::vector<PickupStationHandoverInfo> PathPlanningManager::getPickupStationRegistry() {
  std::vector<PickupStationHandoverInfo> registry;

  for (int i = 0; i < pickup_stations_.GetN(); i++) {
    // Get aggregate protocol object (installed with InternetStackHelper)
    Ptr<Ipv4L3Protocol> ip = pickup_stations_.Get(i)->GetObject<Ipv4L3Protocol>();
    // We only have 2 interfaces (each with own IP)
    // first (0) is always loopback -> take second (1)
    Ipv4InterfaceAddress addr = ip->GetAddress(1, 0);
    // TODO Save IP always as int
    std::string ip_str = daisi::getIpv4AddressString(addr.GetAddress());
    ns3::Vector3D pos = pickup_stations_.Get(i)->GetObject<MobilityModel>()->GetPosition();
    registry.push_back({static_cast<uint32_t>(i), pos, ip_str});
  }

  return registry;
}

std::string PathPlanningManager::getDatabaseFilename() {
  return daisi::generateDBName("path_planning");
}

}  // namespace daisi::path_planning
