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

#include "cpps/amr/model/amr_fleet.h"
#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/amr/physical/amr_mobility_model_ns3.h"
#include "cpps/amr/physical/amr_physical_asset.h"
#include "cpps/common/amr_logical_agent_application.h"
#include "cpps/common/amr_physical_asset_application.h"
#include "cpps/common/material_flow_logical_agent_application.h"
#include "cpps/logical/amr/amr_logical_agent.h"
#include "cpps/logical/material_flow/material_flow_logical_agent.h"
#include "logging/logger_manager.h"
#include "manager/sola_helper.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-helper.h"
#include "utils/random_engine.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::cpps {

/// @brief Global ptr to AmrAssetConnector for ns-3 to pass mobility model
extern ns3::Ptr<daisi::cpps::AmrMobilityModelNs3> next_mobility_model;

CppsManager::CppsManager(const std::string &scenario_config_file)
    : scenario_(scenario_config_file) {
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

  this->amrs_.Get(amr_index)
      ->GetApplication(1)
      ->GetObject<AmrPhysicalAssetApplication>()
      ->application = std::make_unique<AmrPhysicalAsset>(std::move(connector));
  this->amrs_.Get(amr_index)
      ->GetApplication(0)
      ->GetObject<AmrLogicalAgentApplication>()
      ->application = std::make_unique<logical::AmrLogicalAgent>(
      scenario_.algorithm.getParticipantAlgorithmConfig(), amr_index == 0);
}

void CppsManager::setupImpl() {
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/root.yml");
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/join.yml");

  setupNodes();

  initialSpawn();

  scheduleEvents();
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

uint64_t CppsManager::getNumberOfNodes() const {
  return scenario_.initial_number_of_amrs + scenario_.number_of_material_flow_agents;
}

void CppsManager::checkStarted(uint32_t index) {
  const auto &cpps_app_logical = this->amrs_.Get(index)
                                     ->GetApplication(0)
                                     ->GetObject<AmrLogicalAgentApplication>()
                                     ->application;
  if (!cpps_app_logical->isRunning()) {
    throw std::runtime_error("storage instance not started yet");
  }
}

void CppsManager::initAMR(uint32_t index) {
  std::cout << "Init AMR " << index << std::endl;

  amrs_.Get(index)->GetApplication(0)->GetObject<AmrLogicalAgentApplication>()->application->init();
  amrs_.Get(index)
      ->GetApplication(1)
      ->GetObject<AmrPhysicalAssetApplication>()
      ->application->init();
}

void CppsManager::connectAMR(uint32_t index) {
  std::cout << "Connect AMR " << index << std::endl;

  // Get address from logical
  const auto &logical_agent =
      amrs_.Get(index)->GetApplication(0)->GetObject<AmrLogicalAgentApplication>()->application;
  const ns3::InetSocketAddress logical_addr = logical_agent->getServerAddress();

  // Let physical connect to logical
  amrs_.Get(index)
      ->GetApplication(1)
      ->GetObject<AmrPhysicalAssetApplication>()
      ->application->connect(logical_addr);
}

void CppsManager::startAMR(uint32_t index) {
  std::cout << "Start AMR " << index << std::endl;

  amrs_.Get(index)
      ->GetApplication(0)
      ->GetObject<AmrLogicalAgentApplication>()
      ->application->start();
}

void CppsManager::initMF(uint32_t index) {
  std::cout << "Creating MF Logical Agent " << index << std::endl;

  material_flows_.Get(index)
      ->GetApplication(0)
      ->GetObject<MaterialFlowLogicalAgentApplication>()
      ->application = std::make_unique<logical::MaterialFlowLogicalAgent>(
      scenario_.algorithm.getInitiatorAlgorithmConfig(), false);

  std::cout << "Init MF Logical Agent " << index << std::endl;

  const auto &mf_app = material_flows_.Get(index)
                           ->GetApplication(0)
                           ->GetObject<MaterialFlowLogicalAgentApplication>()
                           ->application;

  mf_app->init();
  mf_app->setWaitingForStart();
}

void CppsManager::startMF(uint32_t index) {
  std::cout << "Start MF Logical Agent " << index << std::endl;

  material_flows_.Get(index)
      ->GetApplication(0)
      ->GetObject<MaterialFlowLogicalAgentApplication>()
      ->application->start();
}

void CppsManager::setupNodes() {
  amrs_.Create(scenario_.initial_number_of_amrs);
  material_flows_.Create(scenario_.number_of_material_flow_agents);

  core_network_.addNodesCSMA(material_flows_);
  core_network_.addNodesWifi(amrs_, scenario_.topology.width, scenario_.topology.height);

  daisi::registerNodes(material_flows_);
  daisi::registerNodes(amrs_);

  // Setup Applications
  for (int i = 0; i < amrs_.GetN(); i++) {
    installApplication<AmrLogicalAgentApplication>(amrs_.Get(i));
    amrs_.Get(i)->GetApplication(0)->SetStartTime(ns3::MilliSeconds(0));

    installApplication<AmrPhysicalAssetApplication>(amrs_.Get(i));
    amrs_.Get(i)->GetApplication(1)->SetStartTime(ns3::MilliSeconds(0));
  }

  for (int i = 0; i < material_flows_.GetN(); i++) {
    installApplication<MaterialFlowLogicalAgentApplication>(material_flows_.Get(i));
    material_flows_.Get(i)->GetApplication(0)->SetStartTime(ns3::MilliSeconds(0));
  }

  // Install MobilityModel
  ns3::MobilityHelper mob;
  mob.SetPositionAllocator("ns3::GridPositionAllocator", "MinX",
                           ns3::DoubleValue(scenario_.topology.width * 0.2), "MinY",
                           ns3::DoubleValue(scenario_.topology.height * 0.3), "DeltaX",
                           ns3::DoubleValue(2.5), "DeltaY", ns3::DoubleValue(2), "GridWidth",
                           ns3::UintegerValue(12), "LayoutType", ns3::StringValue("RowFirst"));

  mob.SetMobilityModel("ns3::AmrMobilityModelNs3");
  mob.Install(amrs_);
}

void CppsManager::executeMaterialFlow(int index, const std::string & /*friendly_name*/) {
  // TODO MaterialFlowDescriptionScenario info = material_flow_descriptions_[friendly_name];
  material_flows_.Get(index)
      ->GetApplication(0)
      ->GetObject<MaterialFlowLogicalAgentApplication>()
      ->application->addMaterialFlow("todo");
}

void CppsManager::clearFinishedMaterialFlows() {
  bool found_running_matrial_flow_app = false;

  for (uint32_t i = 0; i < material_flows_.GetN(); i++) {
    const auto &mf_app = material_flows_.Get(i)
                             ->GetApplication(0)
                             ->GetObject<MaterialFlowLogicalAgentApplication>()
                             ->application;

    if (mf_app && mf_app->isFinished()) {
      number_material_flows_finished_++;
      found_running_matrial_flow_app = true;
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
    const auto &mf_app = this->material_flows_.Get(i)
                             ->GetApplication(0)
                             ->GetObject<MaterialFlowLogicalAgentApplication>()
                             ->application;

    if (!mf_app) {
      init_application = true;
      break;
    }

    if (!mf_app->isBusy()) {
      init_application = false;
      break;
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

std::string CppsManager::getDatabaseFilename() const {
  return generateDBName("cpps", generateRandomString());
}

}  // namespace daisi::cpps
