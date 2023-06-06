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

#include "cpps/logical/algorithms/disposition/disposition_participant.h"
#include "cpps/logical/order_management/stn_order_management.h"
#include "cpps/packet.h"
#include "utils/sola_utils.h"

namespace daisi::cpps::logical {

AmrLogicalAgent::AmrLogicalAgent(uint32_t device_id, const AlgorithmConfig &_config,
                                 const bool first_node)
    : LogicalAgent(device_id, daisi::global_logger_manager->createAMRLogger(device_id), _config,
                   first_node),
      description_set_(false),
      topology_(daisi::util::Dimensions(50, 20, 0))  // TODO placeholder
{}

void AmrLogicalAgent::init(ns3::Ptr<ns3::Socket> tcp_socket) {
  initCommunication();

  socket_to_physical_ = tcp_socket;
  socket_to_physical_->Listen();

  socket_to_physical_->SetAcceptCallback(
      ns3::MakeCallback(&AmrLogicalAgent::connectionRequest, this),
      ns3::MakeCallback(&AmrLogicalAgent::newConnectionCreated, this));
}

void AmrLogicalAgent::initAlgorithms() {
  if (!description_set_) {
    throw std::runtime_error("Initialization not finished yet.");
  }

  // TODO decide about chosen order management depending on algorithm config
  order_management_ = std::make_shared<order_management::StnOrderManagement>(
      description_, topology_, daisi::util::Pose{current_position_});

  for (const auto &algo_type : algorithm_config_.algorithm_types_) {
    switch (algo_type) {
      case AlgorithmType::k_disposition_participant:
        algorithms_.push_back(std::make_unique<DispositionParticipant>(sola_));
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
  // TODO add logging of topic message
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

    if (auto amr_description = std::get_if<AmrDescription>(&amr_msg)) {
      if (!description_set_) {
        description_ = *amr_description;
      } else {
        throw std::runtime_error("AmrDescription attempted to be set twice.");
      }
    } else {
    }
  }
}

void AmrLogicalAgent::processMessageAmrDescription(const AmrDescription &description) {
  if (!description_set_) {
    description_ = description;
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

void AmrLogicalAgent::sendTopologyToPhysical() {
  auto topology_msg = amr::serialize(topology_);

  daisi::cpps::CppsTCPMessage message;
  message.addMessage({topology_msg, 0});

  auto packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_to_physical_->Send(packet);
}

void AmrLogicalAgent::sendTaskToPhysical() {
  material_flow::Task task = order_management_->getCurrentTask();

  // TODO convert orders to functionalities
}

bool AmrLogicalAgent::connectionRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  // Accept all requests
  return true;
}

void AmrLogicalAgent::newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  socket_of_physical_ = socket;
  socket_of_physical_->SetRecvCallback(
      MakeCallback(&AmrLogicalAgent::readFromPhysicalSocket, this));

  logAmrInfos();
}

void AmrLogicalAgent::checkSendingNextTask() {
  bool still_busy = true;  // TODO check depending on AmrState and OrderState

  if (!still_busy) {
    sendTaskToPhysical();
  }
}

void AmrLogicalAgent::logAmrInfos() {
  // physical asset
  ns3::Address physical_address;
  socket_of_physical_->GetSockName(physical_address);

  ns3::InetSocketAddress i_physical_address = ns3::InetSocketAddress::ConvertFrom(physical_address);
  std::string physical_asset_ip = daisi::getIpv4AddressString(i_physical_address.GetIpv4());
  uint16_t physical_asset_port = i_physical_address.GetPort();

  // logical
  ns3::Address logical_address;
  socket_of_physical_->GetSockName(logical_address);

  ns3::InetSocketAddress i_logical_address = ns3::InetSocketAddress::ConvertFrom(logical_address);
  std::string logical_asset_ip = daisi::getIpv4AddressString(i_logical_address.GetIpv4());
  uint16_t logical_asset_port = i_logical_address.GetPort();

  // TODO log
  // description, kinematics, load carrier etc
}

}  // namespace daisi::cpps::logical
