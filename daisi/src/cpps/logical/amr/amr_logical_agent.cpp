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

#include "amr_logical_agent.h"

#include "cpps/amr/physical/material_flow_functionality_mapping.h"
#include "cpps/logical/algorithms/disposition/centralized_participant.h"
#include "cpps/logical/algorithms/disposition/iterated_auction_disposition_participant.h"
#include "cpps/logical/message/material_flow_update.h"
#include "cpps/logical/order_management/simple_order_management.h"
#include "cpps/logical/order_management/stn_order_management.h"
#include "cpps/packet.h"
#include "solanet/uuid_generator.h"
#include "utils/socket_manager.h"
#include "utils/sola_utils.h"

namespace daisi::cpps::logical {

AmrLogicalAgent::AmrLogicalAgent(const AlgorithmConfig &config, bool first_node)
    : LogicalAgent(daisi::global_logger_manager->createAMRLogger(), config, first_node),
      topology_(daisi::util::Dimensions(50, 20, 0))  // TODO placeholder
{}

void AmrLogicalAgent::init() {
  initCommunication();

  server_socket_ = SocketManager::get().createSocket(SocketType::kTCP, true);
  server_socket_->Listen();

  server_socket_->SetAcceptCallback(
      ns3::MakeCallback(&AmrLogicalAgent::connectionRequest, this),
      ns3::MakeCallback(&AmrLogicalAgent::newConnectionCreated, this));
}

void AmrLogicalAgent::initAlgorithms() {
  if (!description_set_) {
    throw std::runtime_error("Initialization not finished yet.");
  }

  for (const auto &algo_type : algorithm_config_.algorithm_types) {
    switch (algo_type) {
      case AlgorithmType::kIteratedAuctionDispositionParticipant: {
        auto stn_order_management = std::make_shared<StnOrderManagement>(
            description_, topology_, daisi::util::Pose{execution_state_.getPosition()});
        order_management_ = stn_order_management;

        algorithms_.push_back(std::make_unique<IteratedAuctionDispositionParticipant>(
            sola_, stn_order_management, description_));

        order_management_->addNotifyTaskAssignmentCallback(
            [this]() { this->notifyTaskAssigned(); });

        break;
      }
      case AlgorithmType::kRoundRobinParticipant: {
        auto simple_order_management = std::make_shared<SimpleOrderManagement>(
            description_, topology_, daisi::util::Pose{execution_state_.getPosition()});
        order_management_ = simple_order_management;

        algorithms_.push_back(
            std::make_unique<CentralizedParticipant>(sola_, simple_order_management));

        order_management_->addNotifyTaskAssignmentCallback(
            [this]() { this->notifyTaskAssigned(); });

        break;
      }
      default:
        throw std::invalid_argument("Algorithm Type cannot be initiated on Amr Logical Agent.");
    }
  }
}

void AmrLogicalAgent::start() { initAlgorithms(); }

void AmrLogicalAgent::messageReceiveFunction(const sola::Message &msg) {
  // TODO add logging of message
  this->LogicalAgent::messageReceiveFunction(msg);
}

void AmrLogicalAgent::topicMessageReceiveFunction(const sola::TopicMessage &msg) {
  // TODO add logging of topic message
  this->LogicalAgent::topicMessageReceiveFunction(msg);
}

void AmrLogicalAgent::readFromPhysicalSocket(ns3::Ptr<ns3::Socket> socket) {
  // TODO add logging

  auto packet = socket->Recv();
  daisi::cpps::CppsTCPMessage header;
  packet->RemoveHeader(header);

  for (const auto &msg : header.getMessages()) {
    auto amr_msg = amr::deserialize(msg.payload);

    if (auto amr_description = std::get_if<AmrDescription>(&amr_msg)) {
      processMessageAmrDescription(*amr_description);

    } else if (auto amr_status_update = std::get_if<AmrStatusUpdate>(&amr_msg)) {
      processMessageAmrStatusUpdate(*amr_status_update);

    } else if (auto amr_order_update = std::get_if<AmrOrderUpdate>(&amr_msg)) {
      processMessageAmrOrderUpdate(*amr_order_update);
    }
  }
}

void AmrLogicalAgent::processMessageAmrDescription(const AmrDescription &description) {
  if (!description_set_) {
    description_ = description;
    description_set_ = true;

    setServices();
  } else {
    throw std::runtime_error("AmrDescription attempted to be set twice.");
  }
}

void AmrLogicalAgent::processMessageAmrStatusUpdate(const AmrStatusUpdate &status_update) {
  execution_state_.processAmrStatusUpdate(status_update);
  logPositionUpdate();
  checkSendingNextTaskToPhysical();
}

void AmrLogicalAgent::processMessageAmrOrderUpdate(const AmrOrderUpdate &order_update) {
  execution_state_.processAmrOrderUpdate(order_update);
  forwardOrderUpdate();
  checkSendingNextTaskToPhysical();
}

void AmrLogicalAgent::sendTaskToPhysical() {
  order_management_->setNextTask();
  if (order_management_->hasTasks()) {
    material_flow::Task task = order_management_->getCurrentTask();
    execution_state_.setNextTask(task);

    AmrOrderInfo amr_order_info(
        materialFlowToFunctionalities(task.getOrders(), execution_state_.getPosition()),
        description_.getLoadHandling().getAbility());
    auto order_msg = amr::serialize(amr_order_info);

    sendToPhysical(order_msg);
  }
}

void AmrLogicalAgent::checkSendingNextTaskToPhysical() {
  if (execution_state_.shouldSendNextTaskToPhysical()) {
    if (order_management_) {
      sendTaskToPhysical();
    }
  }
}

void AmrLogicalAgent::sendToPhysical(std::string payload) {
  daisi::cpps::CppsTCPMessage message;
  message.addMessage({payload, 0});

  auto packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  physical_socket_->Send(packet);
}

void AmrLogicalAgent::sendTopologyToPhysical() {
  auto topology_msg = amr::serialize(topology_);
  sendToPhysical(topology_msg);
}

void AmrLogicalAgent::notifyTaskAssigned() { checkSendingNextTaskToPhysical(); }

bool AmrLogicalAgent::connectionRequest(ns3::Ptr<ns3::Socket>, const ns3::Address &) {
  // Accept all requests
  return true;
}

void AmrLogicalAgent::newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  physical_socket_ = socket;
  physical_socket_->SetRecvCallback(MakeCallback(&AmrLogicalAgent::readFromPhysicalSocket, this));

  physical_address_ = addr;

  logAmrInfos();
}

void AmrLogicalAgent::logAmrInfos() {
  daisi::cpps::AmrLoggingInfo info;

  // general
  info.friendly_name = description_.getProperties().getFriendlyName();
  info.serial_number = description_.getSerialNumber();
  info.manufacturer = description_.getProperties().getManufacturer();
  info.model_name = description_.getProperties().getModelName();
  info.model_number = description_.getProperties().getModelNumber();

  // kinematics
  info.max_velocity = description_.getKinematics().getMaxVelocity();
  info.min_velocity = description_.getKinematics().getMinVelocity();
  info.max_acceleration = description_.getKinematics().getMaxAcceleration();
  info.min_acceleration = description_.getKinematics().getMaxDeceleration();

  // load handling unit
  info.load_time = description_.getLoadHandling().getLoadTime();
  info.unload_time = description_.getLoadHandling().getUnloadTime();
  info.load_carrier_type =
      description_.getLoadHandling().getAbility().getLoadCarrier().getTypeAsString();
  info.max_weight = description_.getLoadHandling().getAbility().getMaxPayloadWeight();

  // network

  // retrieve physical info
  ns3::InetSocketAddress i_physical_address =
      ns3::InetSocketAddress::ConvertFrom(physical_address_);
  std::string physical_asset_ip = daisi::getIpv4AddressString(i_physical_address.GetIpv4());
  uint16_t physical_asset_port = i_physical_address.GetPort();

  // retrieve logical info
  ns3::Address logical_address;
  server_socket_->GetSockName(logical_address);

  ns3::InetSocketAddress i_logical_address = ns3::InetSocketAddress::ConvertFrom(logical_address);
  std::string logical_asset_ip = daisi::getIpv4AddressString(i_logical_address.GetIpv4());
  uint16_t logical_asset_port = i_logical_address.GetPort();

  info.ip_logical_core = sola_->getIP();
  info.port_logical_core = sola_->getPort();
  info.ip_logical_asset = logical_asset_ip;
  info.port_logical_asset = logical_asset_port;
  info.ip_physical = physical_asset_ip;
  info.port_physical = physical_asset_port;

  logger_->logAMR(info);
}

void AmrLogicalAgent::logPositionUpdate() {
  AmrPositionLoggingInfo position_logging_info;
  position_logging_info.uuid = uuid_;
  position_logging_info.x = execution_state_.getPosition().x;
  position_logging_info.y = execution_state_.getPosition().y;
  position_logging_info.z = 0;
  position_logging_info.state = static_cast<uint8_t>(execution_state_.getAmrState());
  logger_->logPositionUpdate(position_logging_info);
}

void AmrLogicalAgent::forwardOrderUpdate() {
  MaterialFlowUpdate update;
  update.amr_uuid = uuid_;
  update.task = execution_state_.getTask();
  update.position = execution_state_.getPosition();

  if (execution_state_.getOrderIndex() == -1) {
    update.order_state = OrderStates::kFinished;
    update.order_index = execution_state_.getTask().getOrders().size() - 1;
  } else {
    update.order_state = execution_state_.getOrderState();
    update.order_index = execution_state_.getOrderIndex();
  }

  sola_->sendData(serialize(update), sola::Endpoint(update.task.getConnectionString()));
}

void AmrLogicalAgent::setServices() {
  sola::Service service;
  service.friendly_name = "service_" + description_.getProperties().getFriendlyName();
  service.uuid = solanet::uuidToString(solanet::generateUUID());

  service.key_values.insert({"servicetype", std::string("transport")});

  service.key_values.insert(
      {"maxpayload", description_.getLoadHandling().getAbility().getMaxPayloadWeight()});

  service.key_values.insert(
      {"loadcarriertype",
       description_.getLoadHandling().getAbility().getLoadCarrier().getTypeAsString()});

  service.key_values.insert({"amruuid", uuid_});

  logger_->logTransportService(service, true);
  sola_->addService(service);
}

}  // namespace daisi::cpps::logical
