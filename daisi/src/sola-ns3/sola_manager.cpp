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

#include "sola_manager.h"

#include "../src/logging/logger.h"
#include "manager/sola_helper.h"
#include "minhton/logging/logger.h"
#include "utils/sola_utils.h"

using namespace ns3;

#ifndef NDEBUG
#define LOG(m) std::cout << m << std::endl
#else
#define LOG(m) static_assert(true)
#endif

namespace daisi::sola_ns3 {

SolaManager::SolaManager(const std::string &scenariofile_path) : scenariofile_(scenariofile_path) {}

uint64_t SolaManager::getNumberOfNodes() const { return scenariofile_.number_nodes; }

void SolaManager::setupImpl() {
  nodes_.Create(getNumberOfNodes());
  core_network_.addNodesCSMA(nodes_);

  daisi::registerNodes(nodes_);

  daisi::setupApplication<SolaApplication>(nodes_);

  daisi::global_logger_manager->logMinhtonConfigFile("configurations/root.yml");
  daisi::global_logger_manager->logMinhtonConfigFile("configurations/join.yml");

  // Set ID for all applications/nodes
  for (uint32_t i = 0; i < nodes_.GetN(); i++) {
    auto app = getApplication(i);
    app->SetAttribute("ID", ns3::UintegerValue(i));
  }

  scheduleEvents();
}

ns3::Ptr<SolaApplication> SolaManager::getApplication(uint32_t id) const {
  return nodes_.Get(id)->GetApplication(0)->GetObject<SolaApplication>();
}

void SolaManager::scheduleEvents() {
  ns3::Time current_time(0);

  for (const SolaScenarioSequenceStep &step : scenariofile_.scenario_sequence) {
    std::visit([this, &current_time](auto &&step) { schedule(step, current_time); }, step.step);
    current_time += scenariofile_.default_delay;
  }
}
void SolaManager::subscribeTopic(const std::string &topic, uint32_t id) {
  LOG("[" << Simulator::Now().GetMilliSeconds() << "] JOIN TOPIC " << topic << " ON NODE " << id);
  getApplication(id)->subscribeTopic(topic);
}

void SolaManager::publishTopic(uint32_t id, const std::string &topic, uint64_t msg_size) {
  LOG("[" << Simulator::Now().GetMilliSeconds() << "] PUBLISH ON TOPIC " << topic << " ON NODE "
          << id);
  std::string msg(msg_size, 'x');
  getApplication(id)->publishTopic(topic, msg);
}

void SolaManager::leaveTopic(uint32_t /*node_id*/) { throw std::runtime_error("not implemented"); }

void SolaManager::findService(uint32_t /*node_id*/) { throw std::runtime_error("not implemented"); }

void SolaManager::addService(uint32_t /*node_id*/) { throw std::runtime_error("not implemented"); }

void SolaManager::updateService(uint32_t /*node_id*/) {
  throw std::runtime_error("not implemented");
}

void SolaManager::removeService(uint32_t /*node_id*/) {
  throw std::runtime_error("not implemented");
}

void SolaManager::startSOLA(uint32_t node_id) {
  LOG("[" << Simulator::Now().GetMilliSeconds() << "] STARTING SOLA ON NODE " << node_id);
  getApplication(node_id)->startSOLA();
}

std::string SolaManager::getDatabaseFilename() const { return generateDBName("sola"); }

GeneralScenariofile SolaManager::getGeneralScenariofile() const { return scenariofile_; }

}  // namespace daisi::sola_ns3
