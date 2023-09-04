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

#include "minhton_manager.h"

#include <cmath>

#include "minhton_application.h"
#include "minhton_manager_scheduler.h"

using namespace minhton;
using namespace ns3;

namespace daisi::minhton_ns3 {

MinhtonManager::MinhtonManager(const std::string &scenariofile_path)
    : Manager<MinhtonApplication>(scenariofile_path), scenariofile_(scenariofile_path) {
  Manager::initLogger();
  scheduler_ = std::make_shared<MinhtonManager::Scheduler>(*this);
}

void MinhtonManager::setup() {
  Manager<MinhtonApplication>::setup();
  setupNodeConfigurations();
}

void MinhtonManager::initNode(uint32_t id, minhton::ConfigNode config) {
  Ptr<MinhtonApplication> app =
      this->node_container_.Get(id)->GetApplication(0)->GetObject<MinhtonApplication>();

  app->initializeNode(config);
}

void MinhtonManager::setupNodeConfigurations() {
  constexpr static uint32_t kRootIndex = 0;
  std::vector<ns3::Ipv4Address> addrs = getAddressesForNode(node_container_, kRootIndex);
  ns3::Ipv4Address addr = getNonLocalAddress(addrs);
  std::string root_addr_string = getIpv4AddressString(addr);

  for (uint32_t i = 0; i < getNumberOfNodes(); i++) {
    minhton::ConfigNode node_config;

    node_config.setIsRoot(i == 0);

    if (node_config.isRoot()) {
      node_config.setFanout(scenariofile_.fanout);
    } else {
      node_config.setJoinInfo({JoinInfo::kIp, root_addr_string, minhton::kDefaultIpPort});
    }

    node_config.setVerbose(true);
    node_config.setAlgorithmTypesContainer(scenariofile_.algorithms.convert());
    node_config.setTimeoutLengthsContainer(scenariofile_.timeouts.convert());

    // Init after starting simulation
    ns3::Simulator::ScheduleWithContext(node_container_.Get(i)->GetId(), ns3::MilliSeconds(1),
                                        &MinhtonManager::initNode, this, i, node_config);
  }
}

uint64_t MinhtonManager::getNumberOfNodes() {
  uint64_t current_number_of_nodes = 1;
  uint64_t max_num = current_number_of_nodes;
  uint64_t min_num = current_number_of_nodes;

  for (const MinhtonScenarioSequenceStep &step : scenariofile_.scenario_sequence) {
    if (auto join = std::get_if<JoinMany>(&step.step)) {
      current_number_of_nodes += join->number;

      max_num = std::max(max_num, current_number_of_nodes);
    } else if (auto static_build = std::get_if<StaticBuild>(&step.step)) {
      current_number_of_nodes += static_build->number;

      max_num = std::max(max_num, current_number_of_nodes);
    } else if ([[maybe_unused]] auto join = std::get_if<JoinOne>(&step.step)) {
      current_number_of_nodes++;
      max_num = std::max(max_num, current_number_of_nodes);
    } else if (auto leave = std::get_if<LeaveMany>(&step.step)) {
      current_number_of_nodes -= leave->number;

      min_num = std::min(min_num, current_number_of_nodes);
    } else if ([[maybe_unused]] auto leave = std::get_if<LeaveOne>(&step.step)) {
      current_number_of_nodes--;

      min_num = std::min(min_num, current_number_of_nodes);
    } else if (auto exec = std::get_if<MixedExecution>(&step.step)) {
      min_num = std::min(min_num, current_number_of_nodes - exec->leave_number);
      max_num = std::max(max_num, current_number_of_nodes - exec->join_number);

      current_number_of_nodes += exec->join_number;
      current_number_of_nodes -= exec->leave_number;
    }
  }

  if (min_num < 1) {
    throw std::invalid_argument("There would be less than 1 node in the network at some point. "
                                "This is not possible. Too many nodes are leaving.");
  }

  return max_num;
}

std::string MinhtonManager::getDatabaseFilename() {
  return generateDBNameWithMinhtonInfo("minhton", scenariofile_.fanout, this->getNumberOfNodes());
}

std::string MinhtonManager::getAdditionalParameters() {
  return "NumberOfNodes=" + std::to_string(getNumberOfNodes());
}

}  // namespace daisi::minhton_ns3
