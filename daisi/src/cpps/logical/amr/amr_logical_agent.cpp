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
#include "cpps/logical/algorithms/disposition/iterated_auction_disposition_participant.h"
#include "cpps/logical/order_management/stn_order_management.h"
#include "cpps/packet.h"
#include "utils/sola_utils.h"

namespace daisi::cpps::logical {

AmrLogicalAgent::AmrLogicalAgent(uint32_t device_id, const AlgorithmConfig &config, bool first_node)
    : LogicalAgent(device_id, daisi::global_logger_manager->createAMRLogger(device_id), config,
                   first_node),
      description_set_(false),
      topology_(daisi::util::Dimensions(50, 20, 0)),
      current_state_(AmrState::kIdle)  // TODO placeholder
{}

void AmrLogicalAgent::init(ns3::Ptr<ns3::Socket> tcp_socket) {
  initCommunication();

  server_socket_ = tcp_socket;
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
      case AlgorithmType::kIteartedAuctionDispositionParticipant:

        algorithms_.push_back(std::make_unique<IteratedAuctionDispositionParticipant>(sola_));

        order_management_ = std::make_shared<StnOrderManagement>(
            description_, topology_, daisi::util::Pose{current_position_});
        break;
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
  // TODO add logging of topic message2
  this->LogicalAgent::topicMessageReceiveFunction(msg);
}

void AmrLogicalAgent::readFromPhysicalSocket(ns3::Ptr<ns3::Socket> socket) {
  // TODO add logging?
  // TODO outsource into algorithm; would that make sense?

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
  } else {
    throw std::runtime_error("AmrDescription attempted to be set twice.");
  }
}

void AmrLogicalAgent::processMessageAmrStatusUpdate(const AmrStatusUpdate &status_update) {
  current_position_ = status_update.getPosition();
  current_state_ = status_update.getState();

  checkSendingNextTask();
}

void AmrLogicalAgent::processMessageAmrOrderUpdate(const AmrOrderUpdate &order_update) {
  // TODO set current task state in order management

  checkSendingNextTask();
}

// helper
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

void AmrLogicalAgent::sendTaskToPhysical() {
  material_flow::Task task = order_management_->getCurrentTask();

  // TODO set ability
  amr::AmrStaticAbility ability;
  AmrOrderInfo amr_order_info(materialFlowToFunctionalities(task.getOrders(), current_position_),
                              ability);
  auto order_msg = amr::serialize(amr_order_info);

  sendToPhysical(order_msg);
}

bool AmrLogicalAgent::connectionRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  // Accept all requests
  return true;
}

void AmrLogicalAgent::newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  physical_socket_ = socket;
  physical_socket_->SetRecvCallback(MakeCallback(&AmrLogicalAgent::readFromPhysicalSocket, this));

  logAmrInfos();
}

void AmrLogicalAgent::checkSendingNextTask() {
  // TODO check depending on AmrState and OrderState whether we are still busy or not

  if (current_state_ == AmrState::kIdle && order_management_) {
    sendTaskToPhysical();
  }
}

void AmrLogicalAgent::logAmrInfos() {
  daisi::cpps::AGVLoggingInfo info;  // TODO rename to AMR

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
  ns3::Address physical_address;
  physical_socket_->GetSockName(physical_address);

  ns3::InetSocketAddress i_physical_address = ns3::InetSocketAddress::ConvertFrom(physical_address);
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

  logger_->logAGV(info);  // TODO rename to AMR
}

}  // namespace daisi::cpps::logical
