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
void SolaManager::schedule(StartSOLA start, ns3::Time &current_time) {
  const ns3::Time delay = start.delay;

  for (uint32_t i = 0; i < scenariofile_.number_nodes; i++) {
    ns3::Simulator::ScheduleWithContext(nodes_.Get(i)->GetId(), current_time,
                                        &SolaManager::startSOLA, this, i);
    current_time += delay;
  }
  current_time -= delay;  // No delay at end (should be default delay)
}

void SolaManager::schedule(SubscribeTopic subscribe, ns3::Time &current_time) {
  const ns3::Time delay = subscribe.delay;

  for (uint32_t i = 0; i < scenariofile_.number_nodes; i++) {
    ns3::Simulator::ScheduleWithContext(nodes_.Get(i)->GetId(), current_time,
                                        &SolaManager::subscribeTopic, this, subscribe.topic, i);
    current_time += delay;
  }
  current_time -= delay;  // No delay at end (should be default delay)
}

void SolaManager::schedule(Delay delay, ns3::Time &current_time) const {
  current_time += delay.delay;
}

void SolaManager::schedule(Publish publish, ns3::Time &current_time) {
  const uint32_t node_id = publish.node_id;
  ns3::Simulator::ScheduleWithContext(nodes_.Get(node_id)->GetId(), current_time,
                                      &SolaManager::publishTopic, this, node_id, publish.topic,
                                      publish.message_size);
}

}  // namespace daisi::sola_ns3
