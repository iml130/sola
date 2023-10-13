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
#include "cpps/logical/algorithms/assignment/iterated_auction_assignment_initiator.h"
#include "cpps/logical/algorithms/assignment/round_robin_initiator.h"
#include "cpps/logical/message/material_flow_update.h"
#include "logging/logger_manager.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"

namespace daisi::cpps::logical {

class MaterialFlowStateLogger : public AlgorithmInterface {
public:
  explicit MaterialFlowStateLogger(daisi::cpps::common::CppsCommunicatorPtr communicator)
      : AlgorithmInterface(std::move(communicator)) {}

  void setMaterialflow(std::shared_ptr<daisi::material_flow::MFDLScheduler> material_flow) {
    material_flow_ = material_flow;
  }

  bool process(const MaterialFlowUpdate &msg) override {
    assert(material_flow_);
    material_flow_->processOrderUpdate(msg);
    return true;
  }

private:
  std::shared_ptr<daisi::material_flow::MFDLScheduler> material_flow_;
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
      case AlgorithmType::kIteratedAuctionAssignmentInitiator:
        algorithms_.push_back(
            std::make_unique<IteratedAuctionAssignmentInitiator>(communicator_, logger_));
        break;
      case AlgorithmType::kRoundRobinInitiator:
        algorithms_.push_back(std::make_unique<RoundRobinInitiator>(communicator_, logger_));
        break;
      default:
        throw std::invalid_argument(
            "Algorithm Type cannot be initiated on Material Flow Logical Agent.");
    }
  }

  algorithms_.push_back(std::make_unique<MaterialFlowStateLogger>(communicator_));
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

bool MaterialFlowLogicalAgent::isBusy() const {
  return material_flow_ != nullptr && !material_flow_->isFinished();
}

bool MaterialFlowLogicalAgent::isFinished() const {
  return material_flow_ != nullptr && material_flow_->isFinished();
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
  if (material_flow_ && !material_flow_->isFinished()) {
    throw std::runtime_error("Material flow still/already in progress");
  }

  material_flow_ = std::make_shared<material_flow::MFDLScheduler>(
      mfdl_program, [this](const MaterialFlowUpdate &update) {
        this->logger_->logMaterialFlowOrderUpdate(update);
      });

  // TODO there could be multiple algorithm interfaces in the future
  // TODO Currently only algorithm and logger
  assert(algorithms_.size() == 2);
  assert(dynamic_cast<AssignmentInitiator *>(algorithms_[0].get()));
  assert(dynamic_cast<MaterialFlowStateLogger *>(algorithms_[1].get()));

  auto tmp = dynamic_cast<AssignmentInitiator *>(algorithms_[0].get());
  tmp->addMaterialFlow(material_flow_);

  dynamic_cast<MaterialFlowStateLogger *>(algorithms_[1].get())->setMaterialflow(material_flow_);

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
