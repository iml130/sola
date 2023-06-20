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
#include "cpps/agv/agv_logical.h"
#include "cpps/amr/physical/amr_mobility_model_ns3.h"
#include "cpps/amr/physical/amr_physical_asset.h"
#include "cpps/logical/algorithms/algorithm_config.h"
#include "cpps/model/agv_fleet.h"
#include "minhton-ns3/minhton_logger_ns3.h"
#include "minhton/logging/logger.h"
#include "ns3/core-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/wifi-module.h"
#include "utils/random_engine.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::cpps {

/// @brief Global ptr to AmrAssetConnector for ns-3 to pass mobility model
extern ns3::Ptr<daisi::cpps::AmrMobilityModelNs3> next_mobility_model;

CppsManager::CppsManager(const std::string &scenario_config_file)
    : Manager<CppsApplication>(scenario_config_file) {
  Manager::initLogger();

  parse();
}

void CppsManager::spawnAGV(uint32_t agv_index, const AgvDeviceProperties &properties,
                           const TopologyNs3 &topology) {
  const uint32_t device_id = agvs_.Get(agv_index)->GetId();

  // Create applications
  Topology amr_topology({topology.getWidth(), topology.getHeight(), 0});

  const Kinematics &kinematic = properties.kinematic;
  AmrKinematics kinematics(kinematic.getMaxVelocity(), kinematic.getMinVelocity(),
                           kinematic.getMaxAcceleration(), kinematic.getMinAcceleration());

  AmrProperties amr_properties(
      properties.manufacturer, properties.model_name, properties.model_number,
      properties.device_type, properties.friendly_name + std::to_string(device_id),
      {FunctionalityType::kLoad, FunctionalityType::kMoveTo, FunctionalityType::kUnload});

  AmrPhysicalProperties physical_properties(10, {0.0});

  // Kinematics uses miliseconds while AmrLoadHandlingUnit uses seconds
  AmrLoadHandlingUnit load_handling(properties.kinematic.getLoadTime() / 1000.0,
                                    properties.kinematic.getUnloadTime() / 1000.0,
                                    properties.ability);

  AmrDescription description(0, kinematics, amr_properties, physical_properties, load_handling);

  if (next_mobility_model != nullptr) {
    throw std::runtime_error("mobility model not empty");
  }

  next_mobility_model =
      DynamicCast<AmrMobilityModelNs3>(agvs_.Get(agv_index)->GetObject<MobilityModel>());

  AmrAssetConnector connector(description, amr_topology);

  if (next_mobility_model != nullptr) {
    throw std::runtime_error("mobility model not empty");
  }

  logical::AlgorithmConfig algorithm_config;
  algorithm_config.algorithm_types.push_back(
      logical::AlgorithmType::kIteartedAuctionDispositionParticipant);

  this->agvs_.Get(agv_index)->GetApplication(1)->GetObject<CppsApplication>()->application =
      std::make_shared<AmrPhysicalAsset>(std::move(connector));
  this->agvs_.Get(agv_index)->GetApplication(0)->GetObject<CppsApplication>()->application =
      std::make_shared<logical::AmrLogicalAgent>(device_id, algorithm_config, false);
}

void CppsManager::setup() {
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/root.yml");
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/join.yml");

  setupNodes();

  // Setup AGVs
  assert(this->nodeContainer_.GetN() == getNumberOfNodes());

  auto topology = TopologyNs3(ns3::Vector(width_, height_, depth_));

  ns3::MobilityHelper mob;
  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", ns3::DoubleValue(width_ * 0.2),
                           "MinY", ns3::DoubleValue(height_ * 0.3), "DeltaX", ns3::DoubleValue(2.5),
                           "DeltaY", ns3::DoubleValue(2), "GridWidth", ns3::UintegerValue(12),
                           "LayoutType", ns3::StringValue("RowFirst"));

  mob.SetMobilityModel("ns3::AmrMobilityModelNs3");
  mob.Install(agvs_);

  // Intial spawning
  {
    // Filtering
    std::vector<SpawnInfo> spawn_info;
    while (!spawn_info_.empty() && spawn_info_.top().start_time == 0) {  // TODO: Only use AGVs
      spawn_info.emplace_back(spawn_info_.top());
      if (spawn_info_.top().type == "to") schedule_info_.emplace(spawn_info_.top());
      spawn_info_.pop();
    }

    bool all_prob_dist =
        std::all_of(spawn_info.begin(), spawn_info.end(), [](const SpawnInfo &info) {
          if (info.type == "agv") {
            return std::holds_alternative<DistProb>(info.distribution);
          }
          return true;
        });

    bool all_abs_dist =
        std::all_of(spawn_info.begin(), spawn_info.end(), [](const SpawnInfo &info) {
          if (info.type == "agv") {
            return std::holds_alternative<DistAbs>(info.distribution);
          }
          return true;
        });

    assert(all_prob_dist || all_abs_dist);
    assert(!(all_prob_dist && all_abs_dist));

    std::vector<int> distribution;
    if (all_prob_dist) {
      double res = 0;
      uint32_t count_agvs = 0;
      for (auto &entry : spawn_info) {
        if (entry.type == "agv") {
          count_agvs++;
          res += std::get<DistProb>(entry.distribution).prob;
        }
      }

      assert(res > 0.99 && res < 1.01);  // HACK

      // Create AGV distribution
      distribution.resize(count_agvs);
      for (auto i = 0U; i < count_agvs; i++) {
        assert(spawn_info[i].type == "agv");
        int previous = (i == 0 ? 0 : distribution[i - 1]);
        distribution[i] = std::get<DistProb>(spawn_info[i].distribution).prob * 100 + previous;
      }

      for (auto i = 0U; i < count_agvs; i++) {
        std::cout << spawn_info[i].friendly_name << " with " << distribution[i] << std::endl;
      }

      std::uniform_int_distribution<int> dist(0, 99);

      for (auto i = 0U; i < number_agvs_initial_; i++) {
        int result = dist(daisi::global_random_engine);

        // Find matching index in distribution
        int index = -1;
        for (auto j = 0U; j < count_agvs; j++) {
          int previous = (j == 0 ? 0 : distribution[j - 1]);
          int current = distribution[j];
          if (result < current && result >= previous) {
            index = j;
            break;
          }
        }
        assert(index >= 0 && std::abs(index) < distribution.size());
        std::cout << "SPAWNING AGV " << spawn_info[index].friendly_name << std::endl;

        // Find matching properties
        auto properties_it =
            std::find_if(agv_device_properties_.begin(), agv_device_properties_.end(),
                         [&](const AgvDeviceProperties &prop) {
                           return prop.friendly_name == spawn_info[index].friendly_name;
                         });
        assert(properties_it != agv_device_properties_.end());
        spawnAGV(i, *properties_it, topology);
      }
    } else {
      uint32_t previous_index = 0;
      for (auto &entry : spawn_info) {
        if (entry.type == "agv") {
          auto current_number = std::get<DistAbs>(entry.distribution).abs;

          // Find matching properties
          auto properties_it =
              std::find_if(agv_device_properties_.begin(), agv_device_properties_.end(),
                           [&](const AgvDeviceProperties &prop) {
                             return prop.friendly_name == entry.friendly_name;
                           });

          for (auto i = previous_index; i < previous_index + current_number; i++) {
            std::cout << "SPAWNING AGV " << entry.friendly_name << std::endl;
            assert(properties_it != agv_device_properties_.end());
            spawnAGV(i, *properties_it, topology);
          }

          previous_index += current_number;
        }
      }
    }
  }
}

uint64_t CppsManager::getNumberOfNodes() {
  return number_agvs_initial_ + number_agvs_later_ + number_material_flow_nodes_;
}

void CppsManager::checkStarted(uint32_t index) {
  auto cpps_app_logical = std::get<std::shared_ptr<logical::AmrLogicalAgent>>(
      this->agvs_.Get(index)->GetApplication(0)->GetObject<CppsApplication>()->application);
  if (!cpps_app_logical->isRunning()) {
    throw std::runtime_error("storage instance not started yet");
  }
}

void CppsManager::initAGV(uint32_t index) {
  auto cpps_app_physical = this->agvs_.Get(index)->GetApplication(1)->GetObject<CppsApplication>();
  auto cpps_app_logical = this->agvs_.Get(index)->GetApplication(0)->GetObject<CppsApplication>();

  cpps_app_physical->start();
  cpps_app_logical->start();
}

void CppsManager::connect(int index) {
  auto agv = std::get<std::shared_ptr<AmrPhysicalAsset>>(
      this->agvs_.Get(index)->GetApplication(1)->GetObject<CppsApplication>()->application);
  auto cpps_app_logical = this->agvs_.Get(index)->GetApplication(0)->GetObject<CppsApplication>();

  ns3::Ipv4Address other_ip = cpps_app_logical->local_ip_address_tcp;
  uint16_t other_port = cpps_app_logical->listening_port_tcp;
  InetSocketAddress address(other_ip, other_port);
  agv->connect(address);
}

void CppsManager::setupNodes() {
  agvs_.Create(number_agvs_initial_ + number_agvs_later_);
  material_flows_.Create(number_material_flow_nodes_);
  setupNetworkEthernet();
  setupNetworkWifi();

  nodeContainer_ = NodeContainer(material_flows_, agvs_);

  // All AGVs have a loopback address for communication betwee logical and physical agent
  for (auto i = material_flows_.GetN(); i < nodeContainer_.GetN(); i++) {
    addresses_[i].push_back("127.0.0.1");
  }

  // Setting up TransportOrderApplications and one AGV application
  setupApplication();

  // Setup second applications for physical AGV
  for (uint32_t i = 0; i < agvs_.GetN(); i++) {
    SolaHelper<CppsApplication> helper({"127.0.0.1"}, 4000);
    helper.install(this->agvs_.Get(i));
  }
}

void CppsManager::setupNetworkEthernet() {
  // Use the manager to set up ethernet between specific nodes only
  // Rest of exclusive network setup for cpps is handled here.
  nodeContainer_ = NodeContainer(material_flows_);

  setupNetwork();
}

void CppsManager::setupNetworkWifi() {
  // Setup APs
  const uint32_t number_ap = std::ceil(agvs_.GetN() / (double)constants::kMaxNumberAgvsPerAp);
  access_points_.Create(number_ap);

  // Install APs all in middle of topology
  MobilityHelper mob;
  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(width_ / 2.0), "MinY",
                           DoubleValue(height_ / 2.0), "Z", DoubleValue(8.0), "DeltaX",
                           DoubleValue(0.0), "DeltaY", DoubleValue(0.0), "GridWidth",
                           UintegerValue(20), "LayoutType", StringValue("RowFirst"));
  mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");

  mob.Install(access_points_);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  YansWifiPhyHelper wifi_phy = YansWifiPhyHelper();
  wifi_phy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

  // Assign agvs to AP
  std::vector<NodeContainer> agvs_per_ap;
  agvs_per_ap.resize(number_ap);
  for (uint32_t i = 0; i < agvs_.GetN(); i++) {
    agvs_per_ap[i / constants::kMaxNumberAgvsPerAp].Add(agvs_.Get(i));
  }

  const uint32_t base_address = 3232235520;  // 192.168.0.0

  // Install internet
  InternetStackHelper stack;
  stack.Install(agvs_);

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

    // Setup wifi on AGV
    NetDeviceContainer agv_sta_dev_temp;
    wifi_phy.Set("TxPowerStart", DoubleValue(constants::kMaxPowerDBm));
    wifi_phy.Set("TxPowerEnd", DoubleValue(constants::kMaxPowerDBm));
    agv_sta_dev_temp = wifi.Install(wifi_phy, wifi_mac, agvs_per_ap[i]);
    agv_sta_dev_.Add(agv_sta_dev_temp);

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
    auto sta_interface = ip.Assign(agv_sta_dev_temp);

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
      auto current_router_ip = ip_container_[ip_container_.size() - 1].GetAddress(0);
      static_routing->AddNetworkRouteTo(Ipv4Address("0.0.0.0"), "0.0.0.0", current_router_ip, 1);
      interfaces_.Add(ip_container_[ip_container_.size() - 1].Get(j));
    }

    for (uint32_t j = 0; j < agvs_per_ap[i].GetN(); j++) {
      addresses_.push_back({sta_interface.GetAddress(j)});
    }

    // set up ARP caches for AGV and core router
    // Might not working for wifi devices. (Ref #100)
    // See https://gitlab.com/nsnam/ns-3-dev/-/issues/664
    NetDeviceContainer container(link.Get(1), agv_sta_dev_temp);
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
        uint8_t buff[6];
        Mac48Address::ConvertFrom(other_mac).CopyTo(buff);
        arp_entry->MarkPermanent();
      }
    }
  }

  installDefaultWifiTraces();
}

void CppsManager::executeMaterialFlow(int index, const std::string &friendly_name) {
  auto cpps_app = this->material_flows_.Get(index)->GetApplication(0)->GetObject<CppsApplication>();
  auto to_app = std::get<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(cpps_app->application);

  // TODO MaterialFlowInfo info = material_flow_models_[friendly_name];

  to_app->addMaterialFlow("todo");
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
        if (material_flow_nodes_leave_after_finish_ && to_app->canStop()) {
          found_running_matrial_flow_app = true;
          cpps_app->cleanup();

        } else if (to_app->isFinished()) {
          number_material_flows_finished_++;
          found_running_matrial_flow_app = true;

          if (material_flow_nodes_leave_after_finish_) {
            to_app->prepareStop();
          }
        }
      }
    }
  }

  bool all_material_flows_executed = number_material_flows_finished_ == number_material_flows_;

  if (!found_running_matrial_flow_app && all_material_flows_executed) {
    ns3::Simulator::Stop();
  } else {
    ns3::Simulator::Schedule(MilliSeconds(10000), &CppsManager::clearFinishedMaterialFlows, this);
  }
}

void CppsManager::scheduleMaterialFlow(const SpawnInfo &info) {
  if (number_material_flows_ == number_material_flows_scheduled_for_execution_) return;

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
    const uint32_t device_id = material_flows_.Get(i)->GetId();

    logical::AlgorithmConfig mf_algorithm_config;
    mf_algorithm_config.algorithm_types.push_back(
        logical::AlgorithmType::kIteratedAuctionDispositionInitiator);

    this->material_flows_.Get(i)->GetApplication(0)->GetObject<CppsApplication>()->application =
        std::make_shared<logical::MaterialFlowLogicalAgent>(device_id, mf_algorithm_config, false);

    this->material_flows_.Get(i)->GetApplication(0)->GetObject<CppsApplication>()->start();

    auto mf_app = std::get<std::shared_ptr<logical::MaterialFlowLogicalAgent>>(
        this->material_flows_.Get(i)->GetApplication(0)->GetObject<CppsApplication>()->application);
    mf_app->setWaitingForStart();
  }

  Simulator::Schedule(MilliSeconds(4000), &CppsManager::executeMaterialFlow, this, i,
                      info.friendly_name);

  // Schedule next call
  auto dist = std::get<DistGaussian>(info.distribution).dist;
  double next = dist(daisi::global_random_engine);
  next = std::max(0.0, next);

  std::cout << "[" << number_material_flows_scheduled_for_execution_ << "] SCHEDULE TO TYPE "
            << info.friendly_name << ". NEXT IN " << next << std::endl;

  Simulator::Schedule(MilliSeconds(next), &CppsManager::scheduleMaterialFlow, this, info);

  number_material_flows_scheduled_for_execution_++;
}

void CppsManager::scheduleEvents() {
  Simulator::Schedule(MilliSeconds(1000), &CppsManager::clearFinishedMaterialFlows, this);
  uint64_t current_time = Simulator::Now().GetMilliSeconds();
  uint64_t delay = parser_.getParsedContent()->getRequired<uint64_t>("defaultDelay");
  for (auto i = 0U; i < number_agvs_initial_; i++) {
    current_time += delay;
    Simulator::Schedule(MilliSeconds(current_time), &CppsManager::initAGV, this, i);
  }

  for (auto i = 0U; i < number_agvs_initial_; i++) {
    current_time += delay;
    Simulator::Schedule(MilliSeconds(current_time), &CppsManager::connect, this, i);
  }

  while (!schedule_info_.empty()) {
    assert(schedule_info_.top().type == "to");
    auto info = schedule_info_.top();
    schedule_info_.pop();
    current_time += delay;
    Simulator::Schedule(MilliSeconds(current_time + info.start_time),
                        &CppsManager::scheduleMaterialFlow, this, info);
  }
}

void CppsManager::parse() {
  number_agvs_initial_ =
      parser_.getParsedContent()->getRequired<uint64_t>("initialNumberNodesAGVs");
  number_material_flow_nodes_ =
      parser_.getParsedContent()->getRequired<uint64_t>("numberMaterialFlowNodes");

  parseMRTAConfiguration();
  number_material_flows_ = parser_.getParsedContent()->getRequired<uint64_t>("numberMaterialFlows");
  material_flow_nodes_leave_after_finish_ = parser_.getParsedContent()->getRequired<std::string>(
                                                "materialFlowNodesLeaveAfterFinish") == "on";

  parseAGVs();
  parseTOs();
  parseTopology();
  parseScenarioSequence();
}

std::unordered_map<std::string, InteractionProtocolType>
    CppsManager::interaction_protocol_types_map_{
        {"iterated_auction_pubsub_modified",
         InteractionProtocolType::kIteratedAuctionPubSubModified},
        {"iterated_auction_pubsub_tepssi", InteractionProtocolType::kIteratedAuctionPubSubTePSSI},
    };

std::unordered_map<std::string, TaskManagementType> CppsManager::task_management_types_map_{
    {"basic_list", TaskManagementType::kBasicList},
    {"greedy_tsp_list", TaskManagementType::kGreedyTSPList},
    {"simpletemporalnetwork", TaskManagementType::kSimpleTemporalNetwork}};

std::unordered_map<std::string, UtilityEvaluationComponents>
    CppsManager::utility_evaluation_components_map_{
        {"min_travel_time", UtilityEvaluationComponents::kMinTravelTime},
        {"min_travel_distance", UtilityEvaluationComponents::kMinTravelDistance},
        {"min_makespan", UtilityEvaluationComponents::kMinMakespan},
        {"min_delay", UtilityEvaluationComponents::kMinDelay},
    };

void CppsManager::parseMRTAConfiguration() {
  auto config =
      parser_.getParsedContent()->getRequired<std::shared_ptr<daisi::ScenariofileParser::Table>>(
          "mrta_configuration");

  std::string ip_str = config->getRequired<std::string>("interaction_protocol");
  std::string om_str = config->getRequired<std::string>("task_management");

  auto lower_lambda = [](unsigned char c) { return std::tolower(c); };

  std::transform(ip_str.begin(), ip_str.end(), ip_str.begin(), lower_lambda);

  std::transform(om_str.begin(), om_str.end(), om_str.begin(), lower_lambda);

  mrta_config_.interaction_protocol_type = interaction_protocol_types_map_[ip_str];
  mrta_config_.task_management_type = task_management_types_map_[om_str];

  auto utility_eval_config =
      config->getRequired<std::shared_ptr<daisi::ScenariofileParser::Table>>("utility_evaluation");

  for (auto &entry : utility_eval_config->content) {
    auto component_and_factor = std::get<std::shared_ptr<ScenariofileParser::Table>>(entry.second);
    float factor = component_and_factor->getRequired<float>("factor");

    std::string component_str = component_and_factor->getRequired<std::string>("component");
    std::transform(component_str.begin(), component_str.end(), component_str.begin(), lower_lambda);
    UtilityEvaluationComponents component = utility_evaluation_components_map_[component_str];

    mrta_config_.utility_evaluation_type.components.push_back(component);
    mrta_config_.utility_evaluation_type.factors.push_back(factor);
  }
}

void CppsManager::parseTopology() {
  auto topology =
      parser_.getParsedContent()->getRequired<std::shared_ptr<ScenariofileParser::Table>>(
          "topology");

  width_ = topology->getRequired<uint64_t>("width");
  height_ = topology->getRequired<uint64_t>("height");
  depth_ = topology->getRequired<uint64_t>("depth");
}

void CppsManager::parseAGVs() {
  std::vector<std::pair<amr::AmrStaticAbility, Kinematics>> agv_infos;

  auto agv_table =
      parser_.getParsedContent()
          ->getRequired<std::vector<std::shared_ptr<ScenariofileParser::Table>>>("AGVs");
  for (const auto &agv : agv_table) {
    auto agv_inner = agv->content.begin()->second;
    auto agv_description = *std::get_if<std::shared_ptr<ScenariofileParser::Table>>(&agv_inner);

    std::string device_type = agv_description->getRequired<std::string>("device_type");
    std::string friendly_name = agv_description->getRequired<std::string>("friendly_name");
    std::string model_name = agv_description->getRequired<std::string>("model_name");

    auto opt_manufacturer = agv_description->getOptional<std::string>("manufacturer");
    std::string manufacturer = opt_manufacturer ? opt_manufacturer.value() : "";

    auto opt_model_number = agv_description->getOptional<uint64_t>("model_number");
    uint32_t model_number = opt_model_number ? opt_model_number.value() : 0;

    auto kinematics_description =
        agv_description->getRequired<std::shared_ptr<ScenariofileParser::Table>>("kinematics");
    auto ability_description =
        agv_description->getRequired<std::shared_ptr<ScenariofileParser::Table>>("ability");

    auto kinematics = parseKinematics(kinematics_description);
    auto ability = parseAGVAbility(ability_description);

    agv_infos.push_back({ability, kinematics});

    agv_device_properties_.push_back(AgvDeviceProperties{
        manufacturer, model_name, model_number, device_type, friendly_name, kinematics, ability});
  }

  AGVFleet::init(agv_infos);
}

void CppsManager::parseTOs() {
  auto tos_table =
      parser_.getParsedContent()
          ->getRequired<std::vector<std::shared_ptr<ScenariofileParser::Table>>>("TOs");

  for (const auto &to : tos_table) {
    auto to_inner = to->content.begin()->second;
    auto to_description = *std::get_if<std::shared_ptr<ScenariofileParser::Table>>(&to_inner);

    if (to_description) {
      auto friendly_name = to_description->getRequired<std::string>("friendly_name");

      auto locations_description =
          to_description->getOptional<std::shared_ptr<ScenariofileParser::Table>>("locations");
      std::vector<Vector> locations;
      if (locations_description) {
        for (auto &loc : locations_description->get()->content) {
          auto inner_loc =
              locations_description->get()->getRequired<std::shared_ptr<ScenariofileParser::Table>>(
                  loc.first);
          Vector location_coordinates(inner_loc->getRequired<float>("x"),
                                      inner_loc->getRequired<float>("y"), 0.0);

          locations.push_back(location_coordinates);
        }
      }

      MaterialFlowInfo info;
      info.locations = locations;

      auto graphs_description =
          to_description->getRequired<std::shared_ptr<ScenariofileParser::Table>>("graphs");

      for (auto &graph : graphs_description->content) {
        auto graph_description =
            graphs_description->getRequired<std::shared_ptr<ScenariofileParser::Table>>(
                graph.first);
        info.descriptions.push_back(graph_description);
      }

      material_flow_models_[friendly_name] = info;
    }
  }
}

Boundaries CppsManager::parseBoundaries(
    const std::shared_ptr<ScenariofileParser::Table> &description) {
  Boundaries result;

  double min_x = static_cast<double>(description->getRequired<uint64_t>("widthMin"));
  double max_x = static_cast<double>(description->getRequired<uint64_t>("widthMax"));
  double min_y = static_cast<double>(description->getRequired<uint64_t>("heightMin"));
  double max_y = static_cast<double>(description->getRequired<uint64_t>("heightMax"));
  result = {{min_x, min_y}, {max_x, max_y}};

  return result;
}

void CppsManager::parseScenarioSequence() {
  auto scenario_sequence =
      parser_.getParsedContent()
          ->getRequired<std::vector<std::shared_ptr<ScenariofileParser::Table>>>(
              "scenarioSequence");

  for (const auto &spawn : scenario_sequence) {
    auto spawn_inner = spawn->content.begin()->second;
    auto spawn_description = *std::get_if<std::shared_ptr<ScenariofileParser::Table>>(&spawn_inner);

    std::string type = spawn_description->getRequired<std::string>("type");

    if (type == "agv") {
      parseAgvSpawn(spawn_description);
    } else if (type == "to") {
      parseToSpawn(spawn_description);
    } else {
      throw std::runtime_error("Encountered invalid type while parsing scenario sequence!");
    }
  }
}

void CppsManager::parseAgvSpawn(
    const std::shared_ptr<ScenariofileParser::Table> &spawn_description) {
  std::string friendly_name = spawn_description->getRequired<std::string>("friendly_name");
  uint64_t start_time = spawn_description->getRequired<uint64_t>("start_time");
  std::string distribution = spawn_description->getRequired<std::string>("distribution");

  SpawnInfo info{start_time, "agv", friendly_name};

  if (distribution == "prob" && start_time == 0) {
    float prob = spawn_description->getRequired<float>("prob");
    info.distribution = DistProb{prob};
  } else if (distribution == "abs") {
    uint64_t abs = spawn_description->getRequired<uint64_t>("abs");
    info.distribution = DistAbs{abs};
    number_agvs_later_ += abs;
  } else {
    throw std::runtime_error("Encountered invalid distribution while parsing scenario sequence!");
  }
  spawn_info_.emplace(info);
}

void CppsManager::parseToSpawn(
    const std::shared_ptr<ScenariofileParser::Table> &spawn_description) {
  std::string friendly_name = spawn_description->getRequired<std::string>("friendly_name");
  uint64_t start_time = spawn_description->getRequired<uint64_t>("start_time");

  auto spawn_behavior =
      spawn_description->getRequired<std::shared_ptr<ScenariofileParser::Table>>("spawn_behavior");
  std::string distribution = spawn_behavior->getRequired<std::string>("distribution");

  SpawnInfo info{start_time, "to", friendly_name};

  if (distribution == "gaussian") {
    uint64_t mean = spawn_behavior->getRequired<uint64_t>("mean");
    uint64_t sigma = spawn_behavior->getRequired<uint64_t>("sigma");
    info.distribution = DistGaussian{std::normal_distribution<>(mean, sigma)};
  } else {
    throw std::runtime_error("Encountered invalid distribution while parsing scenario sequence!");
  }
  spawn_info_.emplace(info);
}

Kinematics CppsManager::parseKinematics(std::shared_ptr<ScenariofileParser::Table> description) {
  float max_velo = description->getRequired<float>("max_velo");
  float min_velo = description->getRequired<float>("min_velo");
  float max_acc = description->getRequired<float>("max_acc");
  float min_acc = description->getRequired<float>("min_acc");

  uint64_t load_time = description->getRequired<uint64_t>("load_time");
  uint64_t unload_time = description->getRequired<uint64_t>("unload_time");

  return Kinematics(max_velo, min_velo, max_acc, min_acc, load_time, unload_time);
}

amr::AmrStaticAbility CppsManager::parseAGVAbility(
    std::shared_ptr<ScenariofileParser::Table> description) {
  std::string load_carrier_type_string = description->getRequired<std::string>("load_carrier_type");
  float max_payload = description->getRequired<float>("max_payload");

  amr::LoadCarrier load_carrier(load_carrier_type_string);
  return amr::AmrStaticAbility(load_carrier, max_payload);
}

std::string CppsManager::getDatabaseFilename() {
  return generateDBName("cpps", generateRandomString());
}

}  // namespace daisi::cpps
