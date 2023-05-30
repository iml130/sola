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

#include "material_flow_logical_agent.h"

namespace daisi::cpps::logical {

void MaterialFlowLogicalAgent::setWaitingForStart() { waiting_for_start_ = true; }

MaterialFlowLogicalAgent::MaterialFlowLogicalAgent(uint32_t device_id)
    : LogicalAgent(device_id), logger_(daisi::global_logger_manager->createTOLogger(device_id)) {}

void messageReceiveFunction(const sola::Message &msg) {
  // TODO add logging of message
  LogicalAgent::messageReceiveFunction(msg);
}

void topicMessageReceiveFunction(const sola::TopicMessage &msg) {
  // TODO add logging of topic message
  LogicalAgent::topicMessageReceiveFunction(msg);
}

}  // namespace daisi::cpps::logical