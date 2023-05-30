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

#include "logical_agent.h"

#include "minhton/utils/config_reader.h"

namespace daisi::cpps::logical {

void LogicalAgent::initSola(const std::string &_config_file, uint32_t _device_id) {
  sola::ManagementOverlayMinhton::Config config_mo = minhton::config::readConfig(config_file);

  bool first_node = false;  // TODO

  if (!first_node_) {
    sola_ns3::SOLAWrapperNs3::setJoinIp(config_mo);
  }

  // Nothing to configure (yet)
  sola::EventDisseminationMinhcast::Config config_ed;

  uuid_ = UUIDGenerator::get()();
  //   logger_->setApplicationUUID(uuid_);

  sola_ = std::make_unique<sola_ns3::SOLAWrapperNs3>(config_mo, config_ed, messageReceiveFunction,
                                                     topicMessageReceiveFunction,
                                                     nullptr,  // TODO logger
                                                     uuid_, device_id);
}

void LogicalAgent::processMessage(const LogicalMessage &_message) {
  for (auto &algorithm : algorithms_) {
    bool processed = std::visit(algorithm, message);
    if (processed) {
      return;
    }
  }

  throw std::runtime_error("Failed to process message");
}

}  // namespace daisi::cpps::logical