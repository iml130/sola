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
#include "utils/socket_manager.h"

namespace daisi::cpps {

static constexpr uint32_t kUpdateFrequencyHz = 10;

AmrPhysicalAsset::AmrPhysicalAsset(AmrAssetConnector connector, const Topology &topology)
    : fsm(OrderStates::kFinished),
      connector_(std::move(connector)) {  // always initialize to kFinished since it is
                                          // equivalent to having no task/being idle
  connector_.setTopology(topology);
}

AmrPhysicalAsset::AmrPhysicalAsset(AmrAssetConnector connector)
    : fsm(OrderStates::kFinished),
      connector_(std::move(connector)) {
}  // always initialize to kFinished since it is equivalent to having no task/being idle

void AmrPhysicalAsset::init() {
  socket_ = SocketManager::get().createSocket(SocketType::kTCP, true);
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
  functionality_queue_.pop_front();
  if (holdsMoveType(functionality))
    process_event(ReachedTarget());
  else if (std::holds_alternative<Load>(functionality))
    process_event(LoadedPayload());
  else if (std::holds_alternative<Unload>(functionality))
    process_event(UnloadedPayload());
  sendOrderUpdateNs3();
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
  daisi::cpps::CppsTCPMessage message;
  AmrOrderUpdate task_update(current_state(), getPosition());
  message.addMessage({amr::serialize(task_update), 0});
  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_->Send(packet);
}

void AmrPhysicalAsset::startVehicleStatusUpdates() { scheduleVehicleStatusUpdateNs3(); }

void AmrPhysicalAsset::stopVehicleStatusUpdatesNs3() const {
  ns3::Simulator::Cancel(next_update_event_);
}

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
    if (const auto topology = std::get_if<Topology>(&m))
      connector_.setTopology(*topology);
    else if (const auto order_info = std::get_if<AmrOrderInfo>(&m))
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
  DAISI_CHECK(functionality_queue_.empty(), "Should not get new task before last task is finished");
  for (const auto &functionality : order_info.getFunctionalities())
    functionality_queue_.push_back(functionality);
  process_event(ReceivedOrder());
  sendOrderUpdateNs3();
}

util::Position AmrPhysicalAsset::getPosition() const { return connector_.getPosition(); }

/////////////////////
// fsmlite helpers //
/////////////////////

void AmrPhysicalAsset::executeFrontFunctionality() {
  connector_.execute(functionality_queue_.front(), [this](FunctionalityVariant f) {
    ns3::Simulator::Schedule(ns3::Seconds(0), &AmrPhysicalAsset::updateFunctionality, this, f);
  });
}

bool AmrPhysicalAsset::holdsMoveType(const FunctionalityVariant &f) const {
  return (std::holds_alternative<MoveTo>(f) || std::holds_alternative<Navigate>(f));
}

/////////////////////
// fsmlite actions //
/////////////////////

template <typename T> void AmrPhysicalAsset::execute(const T &) {
  amr_state_ = AmrState::kWorking;
  executeFrontFunctionality();
  if constexpr (std::is_same_v<ReceivedOrder, T>) {
    startVehicleStatusUpdates();
  }
}

template <typename T> void AmrPhysicalAsset::finish(const T &) {
  if constexpr (std::is_same_v<ReceivedOrder, T>) {
    throw std::invalid_argument("empty task");
  }
  stopVehicleStatusUpdatesNs3();
  amr_state_ = AmrState::kIdle;
  sendVehicleStatusUpdateNs3(true);
}

////////////////////
// fsmlite guards //
////////////////////

template <typename T> bool AmrPhysicalAsset::isMoveToLoad(const T &) const {
  return !functionality_queue_.empty() && holdsMoveType(functionality_queue_.front()) &&
         std::holds_alternative<Load>(functionality_queue_.at(1));
}

template <typename T> bool AmrPhysicalAsset::isMoveToUnload(const T &) const {
  return !functionality_queue_.empty() && holdsMoveType(functionality_queue_.front()) &&
         std::holds_alternative<Unload>(functionality_queue_.at(1));
}

template <typename T> bool AmrPhysicalAsset::isMove(const T &) const {
  return !functionality_queue_.empty() && holdsMoveType(functionality_queue_.front()) &&
         (functionality_queue_.size() == 1 || holdsMoveType(functionality_queue_.at(1)));
}

template <typename T> bool AmrPhysicalAsset::isLoad(const T &) const {
  return !functionality_queue_.empty() &&
         std::holds_alternative<Load>(functionality_queue_.front());
}

template <typename T> bool AmrPhysicalAsset::isUnload(const T &) const {
  return !functionality_queue_.empty() &&
         std::holds_alternative<Unload>(functionality_queue_.front());
}

template <typename T> bool AmrPhysicalAsset::isFinish(const T &) const {
  return functionality_queue_.empty();
}

}  // namespace daisi::cpps
