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

#include "cpps/amr/physical/amr_physical_asset.h"

#include "cpps/amr/message/serializer.h"
#include "cpps/packet.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/object.h"
#include "utils/daisi_check.h"

namespace daisi::cpps {

static constexpr uint32_t kUpdateFrequencyHz = 10;

AmrPhysicalAsset::AmrPhysicalAsset(AmrAssetConnector connector, const Topology &topology)
    : connector_(std::move(connector)) {
  connector_.setTopology(topology);
}

AmrPhysicalAsset::AmrPhysicalAsset(AmrAssetConnector connector)
    : connector_(std::move(connector)) {}

void AmrPhysicalAsset::init(const ns3::Ptr<ns3::Socket> &socket) {
  socket_ = socket;
  socket_->SetRecvCallback(MakeCallback(&AmrPhysicalAsset::readSocket, this));
}

void AmrPhysicalAsset::connect(const ns3::InetSocketAddress &endpoint) {
  if (socket_->Connect(endpoint) != 0) throw std::runtime_error("failed");
  sendDescriptionNs3();
  sendVehicleStatusUpdateNs3(true);
}

void AmrPhysicalAsset::updateFunctionality(const FunctionalityVariant &functionality) {
  if (functionality.index() != functionality_queue_.front().index())
    throw std::runtime_error("functionalities don't match");
  functionality_queue_.pop();
  continueOrder();
}

// communication with Logical
/// @brief Get position and send it to corresponding logical agent
void AmrPhysicalAsset::sendVehicleStatusUpdateNs3(bool force) {
  util::Position position = connector_.getPosition();
  if (!force && last_sent_position_ == position) return;
  last_sent_position_ = position;
  AmrStatusUpdate status_update(position, amr_state_);
  daisi::cpps::CppsTCPMessage message;
  message.addMessage({amr::serialize(status_update), 0});
  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_->Send(packet);
}

/// @brief send current OrderState to corresponding logical agent
void AmrPhysicalAsset::sendOrderUpdateNs3() {
  AmrOrderUpdate task_update(current_order_.order_state, getPosition());
  daisi::cpps::CppsTCPMessage message;
  message.addMessage({amr::serialize(task_update), 0});
  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_->Send(packet);
}

void AmrPhysicalAsset::startVehicleStatusUpdates() { scheduleVehicleStatusUpdateNs3(); }

void AmrPhysicalAsset::stopVehicleStatusUpdatesNs3() { ns3::Simulator::Cancel(next_update_event_); }

void AmrPhysicalAsset::scheduleVehicleStatusUpdateNs3() {
  sendVehicleStatusUpdateNs3(false);
  next_update_event_ =
      ns3::Simulator::Schedule(ns3::Seconds(1.0 / kUpdateFrequencyHz),
                               &AmrPhysicalAsset::scheduleVehicleStatusUpdateNs3, this);
}

/// @brief receive a task
void AmrPhysicalAsset::readSocket(ns3::Ptr<ns3::Socket> socket) {
  ns3::Ptr<ns3::Packet> packet = socket->Recv();
  daisi::cpps::CppsTCPMessage header;
  packet->RemoveHeader(header);
  for (const auto &msg : header.getMessages()) {
    amr::Message m = amr::deserialize(msg.payload);
    if (auto topology = std::get_if<Topology>(&m))
      connector_.setTopology(*topology);
    else if (auto order_info = std::get_if<AmrOrderInfo>(&m))
      processMessageOrderInfo(*order_info);
    else
      throw std::runtime_error("invalid packet type for physical amr");
  }
}

/// @brief send description to corresponding logical agent during registration
void AmrPhysicalAsset::sendDescriptionNs3() {
  daisi::cpps::CppsTCPMessage message;
  message.addMessage({amr::serialize(connector_.getDescription()), 0});
  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_->Send(packet);
}

void AmrPhysicalAsset::processMessageOrderInfo(const AmrOrderInfo &order_info) {
  if (amr_state_ != AmrState::kIdle || ((current_order_.order_state != OrderStates::kInvalid) &&
                                        (current_order_.order_state != OrderStates::kFinished))) {
    throw std::runtime_error("Still processsing another task. ");
  }

  current_order_ = AmrOrder();
  current_order_.order_state = OrderStates::kCreated;

  for (const auto &func : order_info.getFunctionalities()) {
    functionality_queue_.push(func);
  }

  continueOrder();
}

void AmrPhysicalAsset::continueOrder() {
  // 1. update state
  // 2. send new state
  // 3. start doing stuff
  switch (current_order_.order_state) {
    case OrderStates::kCreated:
      current_order_.order_state = OrderStates::kQueued;
      continueOrder();
      break;
    case OrderStates::kQueued:
      amr_state_ = AmrState::kWorking;
      current_order_.order_state = OrderStates::kStarted;
      sendOrderUpdateNs3();
      startVehicleStatusUpdates();
      continueOrder();
      break;
    case OrderStates::kStarted:
      current_order_.order_state = OrderStates::kGoToPickUpLocation;
      sendOrderUpdateNs3();

      if (std::holds_alternative<MoveTo>(functionality_queue_.front())) {
        connector_.execute(functionality_queue_.front(),
                           [this](const FunctionalityVariant &f) { this->updateFunctionality(f); });
      } else {
        // case that we are already at the starting position
        // therefore we dont need to move and can load directly
        continueOrder();
      }
      return;
    case OrderStates::kGoToPickUpLocation:
      current_order_.order_state = OrderStates::kReachedPickUpLocation;
      sendOrderUpdateNs3();
      continueOrder();
      break;
    case OrderStates::kReachedPickUpLocation:
      current_order_.order_state = OrderStates::kLoad;
      sendOrderUpdateNs3();
      DAISI_CHECK(std::holds_alternative<Load>(functionality_queue_.front()),
                  "unexpected functionality, expected Load");
      connector_.execute(functionality_queue_.front(),
                         [this](const FunctionalityVariant &f) { this->updateFunctionality(f); });
      return;
    case OrderStates::kLoad: {
      current_order_.order_state = OrderStates::kLoaded;
      sendOrderUpdateNs3();
      continueOrder();
      break;
    }
    case OrderStates::kLoaded:
      current_order_.order_state = OrderStates::kGoToDeliveryLocation;
      sendOrderUpdateNs3();
      DAISI_CHECK(std::holds_alternative<MoveTo>(functionality_queue_.front()),
                  "unexpected functionality, expected MoveTo");
      connector_.execute(functionality_queue_.front(),
                         [this](const FunctionalityVariant &f) { this->updateFunctionality(f); });
      return;
    case OrderStates::kGoToDeliveryLocation:
      current_order_.order_state = OrderStates::kReachedDeliveryLocation;
      sendOrderUpdateNs3();
      continueOrder();
      break;
    case OrderStates::kReachedDeliveryLocation:
      current_order_.order_state = OrderStates::kUnload;
      sendOrderUpdateNs3();
      DAISI_CHECK(std::holds_alternative<Unload>(functionality_queue_.front()),
                  "unexpected functionality, expected Unload");
      connector_.execute(functionality_queue_.front(),
                         [this](const FunctionalityVariant &f) { this->updateFunctionality(f); });
      return;
    case OrderStates::kUnload: {
      current_order_.order_state = OrderStates::kUnloaded;
      sendOrderUpdateNs3();
      continueOrder();
      break;
    }
    case OrderStates::kUnloaded:
      current_order_.order_state = OrderStates::kFinished;
      sendOrderUpdateNs3();
      continueOrder();
      break;
    case OrderStates::kFinished:
      stopVehicleStatusUpdatesNs3();
      amr_state_ = AmrState::kIdle;
      sendVehicleStatusUpdateNs3(true);
      return;
    default:
      break;
  }
}

util::Position AmrPhysicalAsset::getPosition() const { return connector_.getPosition(); }
}  // namespace daisi::cpps
