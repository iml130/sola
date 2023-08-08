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

#ifndef DAISI_SOLA_NS3_CONFIG_HELPER_NS3_H_
#define DAISI_SOLA_NS3_CONFIG_HELPER_NS3_H_

#include "SOLA/event_dissemination_minhcast.h"
#include "SOLA/management_overlay_minhton.h"
#include "logging/logger_manager.h"
#include "ns3/simulator.h"
#include "utils/daisi_check.h"

namespace daisi::sola_ns3 {

inline void configureLogger(sola::ManagementOverlayMinhton::Config &config) {
  const uint64_t device_id = ns3::Simulator::GetContext();
  DAISI_CHECK(device_id != ns3::Simulator::NO_CONTEXT, "Context not set");

  config.setLogger(std::vector<sola::ManagementOverlayMinhton::Logger>{
      daisi::global_logger_manager->createMinhtonLogger(device_id, "MO")});
}

inline void configureLogger(sola::EventDisseminationMinhcast::Config &config) {
  const uint64_t device_id = ns3::Simulator::GetContext();
  DAISI_CHECK(device_id != ns3::Simulator::NO_CONTEXT, "Context not set");

  config.logger = std::vector<sola::EventDisseminationMinhcast::Logger>{
      daisi::global_logger_manager->createNatterLogger(device_id)};
  config.topic_tree_logger_create_fct = [device_id](const std::string &topic) {
    const std::string postfix = "ED:" + topic;
    return daisi::global_logger_manager->createMinhtonLogger(device_id, postfix);
  };
}

}  // namespace daisi::sola_ns3
#endif
