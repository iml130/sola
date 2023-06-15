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

#include "cpps/logical/algorithms/disposition/disposition_initiator.h"

namespace daisi::cpps::logical {

MaterialFlowLogicalAgent::MaterialFlowLogicalAgent(uint32_t device_id,
                                                   const AlgorithmConfig &config_algo,
                                                   const bool first_node)
    : LogicalAgent(device_id, daisi::global_logger_manager->createTOLogger(device_id), config_algo,
                   first_node),
      waiting_for_start_(false) {}

void MaterialFlowLogicalAgent::init(const bool first_node) { initCommunication(); }


void MaterialFlowLogicalAgent::start() { initAlgorithms(); }

void MaterialFlowLogicalAgent::initAlgorithms() {
  for (const auto &algo_type : algorithm_config_.algorithm_types_) {
    switch (algo_type) {
      case AlgorithmType::k_disposition_initiator:
        algorithms_.push_back(std::make_unique<DispositionInitiator>(sola_));
        break;
      default:
        throw std::invalid_argument(
            "Algorithm Type cannot be initiated on Material Flow Logical Agent.");
    }
  }
}

void MaterialFlowLogicalAgent::messageReceiveFunction(const sola::Message &msg) {
  // TODO add logging of message
  this->LogicalAgent::messageReceiveFunction(msg);
}

void MaterialFlowLogicalAgent::topicMessageReceiveFunction(const sola::TopicMessage &msg) {
  // TODO add logging of topic message
  this->LogicalAgent::topicMessageReceiveFunction(msg);
}

void MaterialFlowLogicalAgent::setWaitingForStart() { waiting_for_start_ = true; }

bool MaterialFlowLogicalAgent::isBusy() { return !material_flows_.empty(); }

void MaterialFlowLogicalAgent::addMaterialFlow(std::string mfdl_program) {
  // TODO integrate wrapper and add to material_flow_ vector
}

}  // namespace daisi::cpps::logical
