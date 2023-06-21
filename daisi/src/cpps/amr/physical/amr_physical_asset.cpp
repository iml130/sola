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
    : fsm(OrderStates::kFinished),
      last_order_states_(OrderStates::kFinished),
      connector_(std::move(connector)) {  // always initialize to kFinished since it is
                                          // equivalent to having no task/being idle
  connector_.setTopology(topology);
}

AmrPhysicalAsset::AmrPhysicalAsset(AmrAssetConnector connector)
    : fsm(OrderStates::kFinished),
      last_order_states_(OrderStates::kFinished),
      connector_(std::move(connector)) {
}  // always initialize to kFinished since it is equivalent to having no task/being idle

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
  functionality_queue_.pop_front();
  if (holdsMoveType(functionality))
    process_event(ReachedTarget());
  else if (std::holds_alternative<Load>(functionality))
    process_event(LoadedPayload());
  else if (std::holds_alternative<Unload>(functionality))
    process_event(UnloadedPayload());
  else if (std::holds_alternative<Charge>(functionality))
    process_event(ChargedBattery());
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

/// @brief
/// @return
std::vector<OrderStates> AmrPhysicalAsset::getCurrentAndTransitionedStates() {
  using s = OrderStates;
  std::vector<OrderStates> ret;
  switch (current_state()) {
    case s::kGoToPickUpLocation:
      ret = last_order_states_ == s::kFinished ? std::vector<s>{s::kStarted, s::kGoToPickUpLocation}
                                               : std::vector<s>{s::kGoToPickUpLocation};
      break;
    case s::kGoToDeliveryLocation:
      ret = {s::kLoaded, s::kGoToDeliveryLocation};
      break;
    case s::kLoad:
      ret = {s::kReachedPickUpLocation, s::kLoad};
      break;
    case s::kUnload:
      ret = {s::kReachedDeliveryLocation, s::kUnload};
      break;
    case s::kFinished:
      ret = last_order_states_ == s::kUnload ? std::vector<s>{s::kUnloaded, s::kFinished}
                                             : std::vector<s>{s::kFinished};
      break;
    default:
      ret = {current_state()};
  }
  last_order_states_ = current_state();
  return ret;
}

/// @brief send current OrderState to corresponding logical agent
void AmrPhysicalAsset::sendOrderUpdateNs3() {
  daisi::cpps::CppsTCPMessage message;
  for (OrderStates state : getCurrentAndTransitionedStates()) {
    AmrOrderUpdate task_update(state, getPosition());
    message.addMessage({amr::serialize(task_update), 0});
  }
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
  DAISI_CHECK(functionality_queue_.empty(), "Should not get new task before last task is finished");
  for (auto &functionality : order_info.getFunctionalities())
    functionality_queue_.push_back(functionality);
  process_event(ReceivedOrder());
  sendOrderUpdateNs3();
}

util::Position AmrPhysicalAsset::getPosition() const { return connector_.getPosition(); }

/////////////////////
// flmlite helpers //
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

template <typename T> void AmrPhysicalAsset::charge(const T &t) {
  amr_state_ = AmrState::kCharging;
  executeFrontFunctionality();
  if constexpr (std::is_same_v<ReceivedOrder, T>) {
    startVehicleStatusUpdates();
  }
}

template <typename T> void AmrPhysicalAsset::execute(const T &t) {
  amr_state_ = AmrState::kWorking;
  executeFrontFunctionality();
  if constexpr (std::is_same_v<ReceivedOrder, T>) {
    startVehicleStatusUpdates();
  }
}

template <typename T> void AmrPhysicalAsset::finish(const T &t) {
  if constexpr (std::is_same_v<ReceivedOrder, T>) {
    std::__throw_invalid_argument("empty task");
  }
  stopVehicleStatusUpdatesNs3();
  amr_state_ = AmrState::kIdle;
  sendVehicleStatusUpdateNs3(true);
}

////////////////////
// fsmlite guards //
////////////////////

template <typename T> bool AmrPhysicalAsset::isMoveToLoad(const T &t) const {
  return holdsMoveType(functionality_queue_.front()) &&
         std::holds_alternative<Load>(functionality_queue_.at(1));
}

template <typename T> bool AmrPhysicalAsset::isMoveToUnload(const T &t) const {
  return holdsMoveType(functionality_queue_.front()) &&
         std::holds_alternative<Unload>(functionality_queue_.at(1));
}

template <typename T> bool AmrPhysicalAsset::isMoveToCharge(const T &t) const {
  return holdsMoveType(functionality_queue_.front()) &&
         std::holds_alternative<Charge>(functionality_queue_.at(1));
}

template <typename T> bool AmrPhysicalAsset::isMove(const T &t) const {
  return holdsMoveType(functionality_queue_.front()) &&
         (functionality_queue_.size() == 1 || holdsMoveType(functionality_queue_.at(1)));
}

template <typename T> bool AmrPhysicalAsset::isLoad(const T &t) const {
  return std::holds_alternative<Load>(functionality_queue_.front());
}

template <typename T> bool AmrPhysicalAsset::isUnload(const T &t) const {
  return std::holds_alternative<Unload>(functionality_queue_.front());
}

// template <typename T> bool AmrPhysicalAsset::isCharge(const T &t) const {
//   return std::holds_alternative<Charge>(functionality_queue_.front());
// }

template <typename T> bool AmrPhysicalAsset::isFinish(const T &t) const {
  return functionality_queue_.empty();
}

}  // namespace daisi::cpps
