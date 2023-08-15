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

namespace daisi::sola_ns3 {

inline void configureLogger(sola::ManagementOverlayMinhton::Config &config) {
  config.setLogger(std::vector<sola::ManagementOverlayMinhton::Logger>{
      daisi::global_logger_manager->createMinhtonLogger("MO")});
}

inline void configureLogger(sola::EventDisseminationMinhcast::Config &config) {
  config.logger = std::vector<sola::EventDisseminationMinhcast::Logger>{
      daisi::global_logger_manager->createNatterLogger()};
  config.topic_tree_logger_create_fct = [](const std::string &topic) {
    const std::string postfix = "ED:" + topic;
    return daisi::global_logger_manager->createMinhtonLogger(postfix);
  };
}

}  // namespace daisi::sola_ns3
#endif
