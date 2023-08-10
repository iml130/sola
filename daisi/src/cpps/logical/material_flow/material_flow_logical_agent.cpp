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

#include "cpps/logical/algorithms/algorithm_interface.h"
#include "cpps/logical/algorithms/disposition/iterated_auction_disposition_initiator.h"
#include "cpps/logical/algorithms/disposition/round_robin_initiator.h"
#include "cpps/logical/message/material_flow_update.h"
#include "logging/logger_manager.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"

namespace daisi::cpps::logical {

class MaterialFlowStateLogger : public AlgorithmInterface {
public:
  MaterialFlowStateLogger(daisi::cpps::common::CppsCommunicatorPtr communicator,
                          std::shared_ptr<CppsLoggerNs3> logger)
      : AlgorithmInterface(std::move(communicator)), logger_(std::move(logger)) {}

  bool process(const MaterialFlowUpdate &msg) override {
    MaterialFlowOrderUpdateLoggingInfo logging_info;
    logging_info.amr_uuid = msg.amr_uuid;
    logging_info.task = msg.task;
    logging_info.position = msg.position;
    logging_info.order_state = msg.order_state;
    logging_info.order_index = msg.order_index;
    logger_->logMaterialFlowOrderUpdate(logging_info);

    return true;
  }

private:
  std::shared_ptr<CppsLoggerNs3> logger_;
};

MaterialFlowLogicalAgent::MaterialFlowLogicalAgent(const AlgorithmConfig &config_algo,
                                                   bool first_node)
    : LogicalAgent(daisi::global_logger_manager->createTOLogger(), config_algo, first_node) {}

void MaterialFlowLogicalAgent::init() { initCommunication(); }

void MaterialFlowLogicalAgent::start() {
  setServices();
  initAlgorithms();
}

void MaterialFlowLogicalAgent::initAlgorithms() {
  for (const auto &algo_type : algorithm_config_.algorithm_types) {
    switch (algo_type) {
      case AlgorithmType::kIteratedAuctionDispositionInitiator:
        algorithms_.push_back(
            std::make_unique<IteratedAuctionDispositionInitiator>(communicator_, logger_));
        break;
      case AlgorithmType::kRoundRobinInitiator:
        algorithms_.push_back(std::make_unique<RoundRobinInitiator>(communicator_, logger_));
        break;
      default:
        throw std::invalid_argument(
            "Algorithm Type cannot be initiated on Material Flow Logical Agent.");
    }
  }

  algorithms_.push_back(std::make_unique<MaterialFlowStateLogger>(communicator_, logger_));
}

void MaterialFlowLogicalAgent::messageReceiveFunction(const solanet::Message &msg) {
  // TODO add logging of message
  this->LogicalAgent::messageReceiveFunction(msg);
}

void MaterialFlowLogicalAgent::topicMessageReceiveFunction(const sola::TopicMessage &msg) {
  // TODO add logging of topic message
  this->LogicalAgent::topicMessageReceiveFunction(msg);
}

void MaterialFlowLogicalAgent::setWaitingForStart() { waiting_for_start_ = true; }

bool MaterialFlowLogicalAgent::isBusy() { return !material_flows_.empty(); }

bool MaterialFlowLogicalAgent::isFinished() const {
  return false;  // TODO
}

void MaterialFlowLogicalAgent::setServices() {
  sola::Service service;
  service.friendly_name = "service_material_flow_agent";
  service.uuid = solanet::uuidToString(solanet::generateUUID());

  service.key_values.insert({"servicetype", std::string("assignmentinitiator")});
  service.key_values.insert({"mfuuid", uuid_});
  service.key_values.insert({"endpoint", communicator_->network.getConnectionString()});
  communicator_->sola.addService(service);
}

void MaterialFlowLogicalAgent::addMaterialFlow(std::string mfdl_program) {
  // TODO save scheduler somewhere?
  auto scheduler = std::make_shared<material_flow::MFDLScheduler>(mfdl_program);

  // TODO there could be multiple algorithm interfaces in the future
  // TODO Currently only algorithm and logger
  assert(algorithms_.size() == 2);
  assert(dynamic_cast<DispositionInitiator *>(algorithms_[0].get()));
  assert(dynamic_cast<MaterialFlowStateLogger *>(algorithms_[1].get()));

  auto tmp = dynamic_cast<DispositionInitiator *>(algorithms_[0].get());
  tmp->addMaterialFlow(scheduler);

  const std::string ip = communicator_->network.getIP();
  const uint16_t port = communicator_->network.getPort();

  if (execution_counter_++ == 0) {
    logger_->logMaterialFlow(uuid_, ip, port, 0);
  } else {
    uuid_ = solanet::uuidToString(solanet::generateUUID());
    logger_->logMaterialFlow(uuid_, ip, port, 1);
  }
  tmp->logMaterialFlowContent(uuid_);
}

}  // namespace daisi::cpps::logical
