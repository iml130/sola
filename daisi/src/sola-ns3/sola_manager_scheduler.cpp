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

#define GET_VALUE(type, name) INNER_TABLE(it)->getRequired<type>(name);

namespace daisi::sola_ns3 {
void SolaManager::scheduleSOLAStart(scenario_it it, uint64_t &current_time) {
  auto nodes = GET_VALUE(std::string, "nodes");
  auto delay = GET_VALUE(uint64_t, "delay");
  assert(nodes == "all");

  for (uint32_t i = 0; i < number_nodes_; i++) {
    ns3::Simulator::Schedule(ns3::MilliSeconds(current_time), &SolaManager::startSOLA, this, i);
    current_time += delay;
  }
  current_time -= delay;  // No delay at end (should be default delay)
}

void SolaManager::scheduleSubscribeTopic(SolaManager::scenario_it it, uint64_t &current_time) {
  auto topic = GET_VALUE(std::string, "topic");
  auto wait_till_finished = GET_VALUE(std::string, "waitTillFinished");
  auto nodes = GET_VALUE(std::string, "nodes");
  auto delay = GET_VALUE(uint64_t, "delay");
  assert(nodes == "all");

  for (uint32_t i = 0; i < number_nodes_; i++) {
    ns3::Simulator::Schedule(ns3::MilliSeconds(current_time), &SolaManager::subscribeTopic, this,
                             topic, i);
    current_time += delay;
  }
  current_time -= delay;  // No delay at end (should be default delay)
}

void SolaManager::scheduleDelay(scenario_it it, uint64_t &current_time) {
  current_time += GET_VALUE(uint64_t, "delay");
}

void SolaManager::schedulePublish(scenario_it it, uint64_t &current_time) {
  auto topic = GET_VALUE(std::string, "topic");
  auto msg_size = GET_VALUE(uint64_t, "messageSize");
  auto nodes = GET_VALUE(uint64_t, "nodes");
  try {
    //    uint32_t node_number = std::stoi(nodes);
    uint32_t node_number = nodes;
    ns3::Simulator::Schedule(ns3::MilliSeconds(current_time), &SolaManager::publishTopic, this,
                             node_number, topic, msg_size);
  } catch (...) {
    std::cerr << "failed node conversion" << std::endl;
    throw;
  }
}

}  // namespace daisi::sola_ns3
