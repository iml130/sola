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

#include "cpps/common/cpps_manager.h"

#include <cassert>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../src/logging/logger.h"
#include "cpps/amr/model/amr_fleet.h"
#include "cpps/amr/physical/amr_mobility_model_ns3.h"
#include "cpps/amr/physical/amr_physical_asset.h"
#include "cpps/logical/algorithms/algorithm_config.h"
#include "cpps/logical/amr/amr_logical_agent.h"
#include "ns3/core-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/wifi-module.h"
#include "scenariofile/cpps_scenariofile.h"
#include "utils/random_engine.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::cpps {

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

/// @brief Global ptr to AmrAssetConnector for ns-3 to pass mobility model
extern ns3::Ptr<daisi::cpps::AmrMobilityModelNs3> next_mobility_model;

CppsManager::CppsManager(const std::string &scenario_config_file)
    : ManagerOld<CppsApplication>(scenario_config_file), scenario_(scenario_config_file) {
  ManagerOld::initLogger();

  amr_descriptions_ = scenario_.getAmrDescriptions();
  material_flow_descriptions_ = scenario_.getMaterialFlowDescriptions();

  for (const auto &info : scenario_.scenario_sequence) {
    if (info.isAmr()) {
      spawn_info_.push(info);
    } else if (info.isMaterialFlow()) {
      schedule_info_.push(info);
    }
  }

  std::vector<std::pair<amr::AmrStaticAbility, AmrKinematics>> amr_infos;
  std::transform(amr_descriptions_.begin(), amr_descriptions_.end(), std::back_inserter(amr_infos),
                 [](const auto &pair) {
                   return std::make_pair(pair.second.getLoadHandling().getAbility(),
                                         pair.second.getKinematics());
                 });
  AmrFleet::init(amr_infos);
}

void CppsManager::spawnAMR(uint32_t amr_index, const AmrDescription &description,
                           const Topology &topology) {
  std::cout << "Creating AMR " << description.getProperties().getFriendlyName() << std::endl;

  if (next_mobility_model != nullptr) {
    throw std::runtime_error("mobility model not empty");
  }

  next_mobility_model =
      DynamicCast<AmrMobilityModelNs3>(amrs_.Get(amr_index)->GetObject<MobilityModel>());

  AmrAssetConnector connector(description, topology);

  if (next_mobility_model != nullptr) {
    throw std::runtime_error("mobility model not empty");
  }

  this->amrs_.Get(amr_index)->GetApplication(1)->GetObject<CppsApplication>()->application =
      std::make_shared<AmrPhysicalAsset>(std::move(connector));
  this->amrs_.Get(amr_index)->GetApplication(0)->GetObject<CppsApplication>()->application =
      std::make_shared<logical::AmrLogicalAgent>(
          scenario_.algorithm.getParticipantAlgorithmConfig(), amr_index == 0);
}

void CppsManager::setup() {
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/root.yml");
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/join.yml");

  setupNodes();

  // Setup AMRs
  assert(this->node_container_.GetN() == getNumberOfNodes());

  ns3::MobilityHelper mob;
  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX",
                           ns3::DoubleValue(scenario_.topology.width * 0.2), "MinY",
                           ns3::DoubleValue(scenario_.topology.height * 0.3), "DeltaX",
                           ns3::DoubleValue(2.5), "DeltaY", ns3::DoubleValue(2), "GridWidth",
                           ns3::UintegerValue(12), "LayoutType", ns3::StringValue("RowFirst"));

  mob.SetMobilityModel("ns3::AmrMobilityModelNs3");
  mob.Install(amrs_);

  initialSpawn();
}

void CppsManager::initialSpawn() {
  uint32_t previous_index = 0;

  while (!spawn_info_.empty() && spawn_info_.top().start_time == ns3::Time(0)) {
    auto info = spawn_info_.top();
    spawn_info_.pop();

    if (info.isAmr()) {
      auto description = amr_descriptions_[info.friendly_name];
      for (auto i = previous_index; i < previous_index + info.spawn_distribution.number; i++) {
        auto topology = scenario_.topology.getTopology();
        spawnAMR(i, description, topology);
      }

      previous_index += info.spawn_distribution.number;
    }
  }
}

uint64_t CppsManager::getNumberOfNodes() {
  return scenario_.initial_number_of_amrs + scenario_.number_of_material_flow_agents;
}

void CppsManager::checkStarted(uint32_t index) {
  auto cpps_app_logical = std::get<std::shared_ptr<logical::AmrLogicalAgent>>(
      this->amrs_.Get(index)->GetApplication(0)->GetObject<CppsApplication>()->application);
  if (!cpps_app_logical->isRunning()) {
    throw std::runtime_error("storage instance not started yet");
  }
}

void CppsManager::initAMR(uint32_t index) {
  std::cout << "Init AMR " << index << std::endl;

  auto cpps_app_logical = this->amrs_.Get(index)->GetApplication(0)->GetObject<CppsApplication>();
  auto cpps_app_physical = this->amrs_.Get(index)->GetApplication(1)->GetObject<CppsApplication>();

  cpps_app_logical->init();
  cpps_app_physical->init();
}

void CppsManager::connectAMR(uint32_t index) {
  std::cout << "Connect AMR " << index << std::endl;

  // Get address from logical
  auto logical_agent = std::get<std::shared_ptr<logical::AmrLogicalAgent>>(
      this->amrs_.Get(index)->GetApplication(0)->GetObject<CppsApplication>()->application);
  const ns3::InetSocketAddress logical_addr = logical_agent->getServerAddress();

  // Let physical connect to logical
  auto cpps_app_physical = this->amrs_.Get(index)->GetApplication(1)->GetObject<CppsApplication>();
  auto amr_physical_asset =
      std::get<std::shared_ptr<AmrPhysicalAsset>>(cpps_app_physical->application);
  amr_physical_asset->connect(logical_addr);
}

void CppsManager::startAMR(uint32_t index) {
  std::cout << "Start AMR " << index << std::endl;

  auto cpps_app_logical = this->amrs_.Get(index)->GetApplication(0)->GetObject<CppsApplication>();
  auto cpps_app_physical = this->amrs_.Get(index)->GetApplication(1)->GetObject<CppsApplication>();

  cpps_app_logical->start();
  cpps_app_physical->start();
}

void CppsManager::initMF(uint32_t index) {
  std::cout << "Creating MF Logical Agent " << index << std::endl;

  this->material_flows_.Get(index)->GetApplication(0)->GetObject<CppsApplication>()->application =
      std::make_shared<logical::MaterialFlowLogicalAgent>(
          scenario_.algorithm.getInitiatorAlgorithmConfig(), false);

  std::cout << "Init MF Logical Agent " << index << std::endl;

  this->material_flows_.Get(index)->GetApplication(0)->GetObject<CppsApplication>()->init();
  auto mf_app = std::get<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(
      this->material_flows_.Get(index)
          ->GetApplication(0)
          ->GetObject<CppsApplication>()
          ->application);

  mf_app->setWaitingForStart();
}

void CppsManager::startMF(uint32_t index) {
  std::cout << "Start MF Logical Agent " << index << std::endl;

  this->material_flows_.Get(index)->GetApplication(0)->GetObject<CppsApplication>()->start();
}

void CppsManager::setupNodes() {
  amrs_.Create(scenario_.initial_number_of_amrs);
  material_flows_.Create(scenario_.number_of_material_flow_agents);
  setupNetworkEthernet();
  setupNetworkWifi();

  node_container_ = NodeContainer(material_flows_, amrs_);

  // Setting up TransportOrderApplications and one AMR application
  setupApplication();

  // Setup second applications for physical AMR
  for (uint32_t i = 0; i < amrs_.GetN(); i++) {
    installApplication<CppsApplication>(this->amrs_.Get(i));
  }
}

void CppsManager::setupNetworkEthernet() {
  // Use the manager to set up ethernet between specific nodes only
  // Rest of exclusive network setup for cpps is handled here.
  node_container_ = NodeContainer(material_flows_);

  setupNetwork();
}

void CppsManager::setupNetworkWifi() {
  // Setup APs
  const uint32_t number_ap = std::ceil(amrs_.GetN() / (double)constants::kMaxNumberAmrsPerAp);
  access_points_.Create(number_ap);

  // Install APs all in middle of topology
  MobilityHelper mob;
  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX",
                           DoubleValue(scenario_.topology.width / 2.0), "MinY",
                           DoubleValue(scenario_.topology.height / 2.0), "Z", DoubleValue(8.0),
                           "DeltaX", DoubleValue(0.0), "DeltaY", DoubleValue(0.0), "GridWidth",
                           UintegerValue(20), "LayoutType", StringValue("RowFirst"));
  mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");

  mob.Install(access_points_);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper wifi_phy = YansWifiPhyHelper();
  wifi_phy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

  // Assign amrs to AP
  std::vector<NodeContainer> amrs_per_ap;
  amrs_per_ap.resize(number_ap);
  for (uint32_t i = 0; i < amrs_.GetN(); i++) {
    amrs_per_ap[i / constants::kMaxNumberAmrsPerAp].Add(amrs_.Get(i));
  }

  const uint32_t base_address = 3232235520;  // 192.168.0.0

  // Install internet
  InternetStackHelper stack;
  stack.Install(amrs_);

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
                     BooleanValue(false), "WaitBeaconTimeout", TimeValue(Seconds(2.56)));

    // Setup wifi on AMR
    NetDeviceContainer amr_sta_dev_temp;
    wifi_phy.Set("TxPowerStart", DoubleValue(constants::kMaxPowerDBm));
    wifi_phy.Set("TxPowerEnd", DoubleValue(constants::kMaxPowerDBm));
    amr_sta_dev_temp = wifi.Install(wifi_phy, wifi_mac, amrs_per_ap[i]);
    amr_sta_dev_.Add(amr_sta_dev_temp);

    // set up the AP
    bool beacon_generation = true;
#ifdef DAISI_SOLANET_NS3_DISABLE_NETWORKING
    beacon_generation = false;
#endif

    wifi_mac.SetType("ns3::ApWifiMac", "QosSupported", BooleanValue(true), "Ssid", SsidValue(ssid),
                     "BeaconInterval", TimeValue(Seconds(2.56)), "BeaconGeneration",
                     ns3::BooleanValue(beacon_generation));
    auto ap_sta_dev_temp = wifi.Install(wifi_phy, wifi_mac, access_points_.Get(i));

    // Connection between AP and core router
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("10Gbps"));
    // Speed of light delay
    // https://en.wikipedia.org/wiki/Velocity_factor#Typical_velocity_factors
    // assuming 10cm/optical fiber: 10cm/0.67c = 498ps
    csma.SetChannelAttribute("Delay", TimeValue(PicoSeconds(498)));
    auto link = csma.Install(NodeContainer(access_points_.Get(i), core_router_.Get(0)));

    link.Get(0)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleSwitchMacTxDrop));
    link.Get(0)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleSwitchPhyRxDrop));
    link.Get(0)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleSwitchPhyTxDrop));
    link.Get(1)->TraceConnectWithoutContext("MacTxDrop", MakeCallback(&handleRouterMacTxDrop));
    link.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeCallback(&handleRouterPhyRxDrop));
    link.Get(1)->TraceConnectWithoutContext("PhyTxDrop", MakeCallback(&handleRouterPhyTxDrop));

    // Bridge between cable and wireless on AP
    BridgeHelper bridge;
    bridge.Install(access_points_.Get(i), ns3::NetDeviceContainer(ap_sta_dev_temp, link.Get(0)));

    // Assign IPs
    Ipv4AddressHelper ip;
    const uint32_t ip_address = base_address + 65536 * i;
    ip.SetBase(Ipv4Address(ip_address), "255.255.0.0");
    auto router_ip = ip.Assign(link.Get(1));
    auto sta_interface = ip.Assign(amr_sta_dev_temp);

    ns3::Ipv4InterfaceContainer ips;
    ips.Add(router_ip);
    ips.Add(sta_interface);
    ip_container_.push_back(ips);

    // Add routing
    uint32_t port_on_router = switchContainer_.GetN() + i;

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

      // Default route/gateways
      auto current_router_ip = ip_container_[ip_container_.size() - 1].GetAddress(0);
      static_routing->AddNetworkRouteTo(Ipv4Address("0.0.0.0"), "0.0.0.0", current_router_ip, 1);
    }

    // set up ARP caches for AMR and core router
    // Might not work for Wi-Fi devices.
    // See https://gitlab.com/nsnam/ns-3-dev/-/issues/664
    NetDeviceContainer container(link.Get(1), amr_sta_dev_temp);
    assert(ips.GetN() == container.GetN());
    for (int j = 0; j < container.GetN(); j++) {
      auto ip_j = ips.Get(j);
      auto arpcache =
          ip_j.first->GetObject<Ipv4L3Protocol>()->GetInterface(ip_j.second)->GetArpCache();

      for (int k = 0; k < ips.GetN(); k++) {
        if (j == k) continue;
        auto other_ip = ips.GetAddress(k);
        auto other_mac = container.Get(k)->GetAddress();
        ns3::ArpCache::Entry *arp_entry = arpcache->Add(other_ip);
        arp_entry->SetMacAddress(Mac48Address::ConvertFrom(other_mac));
        arp_entry->MarkPermanent();
      }
    }
  }

  installDefaultWifiTraces();
}

void CppsManager::executeMaterialFlow(int index, const std::string & /*friendly_name*/) {
  auto cpps_app = this->material_flows_.Get(index)->GetApplication(0)->GetObject<CppsApplication>();
  auto mf_app = std::get<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(cpps_app->application);

  // TODO MaterialFlowDescriptionScenario info = material_flow_descriptions_[friendly_name];

  mf_app->addMaterialFlow("todo");
}

void CppsManager::clearFinishedMaterialFlows() {
  bool found_running_matrial_flow_app = false;

  for (uint32_t i = 0; i < material_flows_.GetN(); i++) {
    auto cpps_app = this->material_flows_.Get(i)->GetApplication(0)->GetObject<CppsApplication>();
    if (std::holds_alternative<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(
            cpps_app->application)) {
      auto to_app =
          std::get<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(cpps_app->application);
      if (to_app) {
        if (scenario_.do_material_flow_agents_leave_after_finish && to_app->canStop()) {
          found_running_matrial_flow_app = true;
          cpps_app->cleanup();

        } else if (to_app->isFinished()) {
          number_material_flows_finished_++;
          found_running_matrial_flow_app = true;

          if (scenario_.do_material_flow_agents_leave_after_finish) {
            to_app->prepareStop();
          }
        }
      }
    }
  }

  bool all_material_flows_executed =
      number_material_flows_finished_ == scenario_.number_of_material_flow_agents;

  if (!found_running_matrial_flow_app && all_material_flows_executed) {
    ns3::Simulator::Stop();
  } else {
    ns3::Simulator::Schedule(Seconds(10), &CppsManager::clearFinishedMaterialFlows, this);
  }
}

void CppsManager::scheduleMaterialFlow(const SpawnInfoScenario &info) {
  if (scenario_.number_of_material_flow_agents == number_material_flows_scheduled_for_execution_)
    return;

  // Search for next free index
  uint32_t i = 0;
  bool init_application = true;
  for (; i < material_flows_.GetN(); i++) {
    auto cpps_app = this->material_flows_.Get(i)->GetApplication(0)->GetObject<CppsApplication>();

    if (std::holds_alternative<std::monostate>(cpps_app->application)) {
      init_application = true;
      break;
    }

    if (std::holds_alternative<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(
            cpps_app->application)) {
      auto mf_app =
          std::get<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(cpps_app->application);
      if (!mf_app->isBusy()) {
        init_application = false;
        break;
      }
    }

    if (i + 1 == material_flows_.GetN())
      throw std::runtime_error("unable to find free node for TO");
  }

  if (init_application) {
    Simulator::ScheduleWithContext(material_flows_.Get(i)->GetId(), Seconds(0),
                                   &CppsManager::initMF, this, i);

    Simulator::ScheduleWithContext(material_flows_.Get(i)->GetId(), Seconds(2),
                                   &CppsManager::startMF, this, i);
  }

  Simulator::ScheduleWithContext(material_flows_.Get(i)->GetId(), Seconds(4),
                                 &CppsManager::executeMaterialFlow, this, i, info.friendly_name);

  // Schedule next call

  if (!info.spawn_distribution.isGaussian()) {
    throw std::invalid_argument("Only gaussian distribution supported.");
  }

  auto dist =
      std::normal_distribution<>(info.spawn_distribution.mean, info.spawn_distribution.sigma);
  double next = dist(daisi::global_random_engine);
  next = std::max(0.0, next);

  std::cout << "[" << number_material_flows_scheduled_for_execution_ << "] SCHEDULE TO TYPE "
            << info.friendly_name << ". NEXT IN " << next << std::endl;

  Simulator::Schedule(Seconds(next), &CppsManager::scheduleMaterialFlow, this, info);

  number_material_flows_scheduled_for_execution_++;
}

void CppsManager::scheduleEvents() {
  Simulator::Schedule(Seconds(1), &CppsManager::clearFinishedMaterialFlows, this);
  ns3::Time current_time = Simulator::Now();
  const ns3::Time delay = scenario_.default_delay;

  for (auto i = 0U; i < scenario_.initial_number_of_amrs; i++) {
    current_time += delay;
    Simulator::ScheduleWithContext(this->amrs_.Get(i)->GetId(), current_time, &CppsManager::initAMR,
                                   this, i);
  }

  for (auto i = 0U; i < scenario_.initial_number_of_amrs; i++) {
    current_time += delay;
    Simulator::ScheduleWithContext(this->amrs_.Get(i)->GetId(), current_time,
                                   &CppsManager::connectAMR, this, i);
  }

  for (auto i = 0U; i < scenario_.initial_number_of_amrs; i++) {
    current_time += delay;
    Simulator::ScheduleWithContext(this->amrs_.Get(i)->GetId(), current_time,
                                   &CppsManager::startAMR, this, i);
  }

  while (!schedule_info_.empty()) {
    assert(schedule_info_.top().isMaterialFlow());
    auto info = schedule_info_.top();
    schedule_info_.pop();
    current_time += delay;
    Simulator::Schedule(current_time + info.start_time, &CppsManager::scheduleMaterialFlow, this,
                        info);
  }
}

std::string CppsManager::getDatabaseFilename() {
  return generateDBName("cpps", generateRandomString());
}

}  // namespace daisi::cpps
