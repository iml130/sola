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

#include "agv_physical_basic.h"

#include <cassert>

#include "cpps/packet.h"
#include "path_planning/message/serializer.h"
#include "path_planning/message/types_all.h"

namespace daisi::cpps {

AGVPhysicalBasic::AGVPhysicalBasic(const AmrDescription &description, const Topology &topology,
                                   ns3::Ptr<cpps::AmrMobilityModelNs3> mobility,
                                   int id_for_friendly, ns3::Ptr<ns3::Socket> socket)
    : amr_description_(description), socket_(socket) {
  // Topology
  ns3::Vector size = topology.getSize();
  topology_ = Topology({size.x, size.y, size.z});

  mobility_ = mobility;

  socket_->SetRecvCallback(MakeCallback(&AGVPhysicalBasic::readFromSocket, this));
}

void AGVPhysicalBasic::connect(ns3::InetSocketAddress endpoint) {
  if (socket_->Connect(endpoint) != 0) throw std::runtime_error("failed");

  daisi::cpps::CppsTCPMessage message;

  std::stringstream stream;

  // stream << agv_data_model_;

  message.addMessage({stream.str(), 0});
  message.addMessage({generatePosUpdate(), 2});

  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_->Send(packet);
}

void AGVPhysicalBasic::mobilityCallback() {
  using namespace daisi::path_planning::message;
  assert(state_ == State::kMoving);
  state_ = State::kIdle;
  assert(mobility_->GetVelocity() == ns3::Vector3D(0, 0, 0));
  ns3::Simulator::Cancel(next_update_event_);

  // Notify logical
  sendFieldMessage(generatePosUpdate());
  sendFieldMessage(serialize<FieldMessage>(ReachedGoalField{}));
}

void AGVPhysicalBasic::sendFieldMessage(const std::string &content) {
  daisi::cpps::CppsTCPMessage message;
  message.addMessage({content, 2});
  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_->Send(packet);
}

void AGVPhysicalBasic::readFromSocket(ns3::Ptr<ns3::Socket> socket) {
  using namespace daisi::path_planning::message;
  auto packet = socket->Recv();
  daisi::cpps::CppsTCPMessage header;
  packet->RemoveHeader(header);

  for (auto &msg : header.getMessages()) {
    switch (msg.type) {
      case 2: {
        if (state_ != State::kIdle) throw std::runtime_error("cannot move while not idle");
        auto drive_message = std::get<DriveMessageField>(deserialize<FieldMessage>(msg.payload));
        goal_ = ns3::Vector(drive_message.x, drive_message.y, 0.0);
        mobility_->execute(MoveTo({goal_.x, goal_.y}), amr_description_, topology_,
                           [this](const FunctionalityVariant &f) { this->mobilityCallback(); });
        state_ = State::kMoving;
        assert(ns3::Simulator::IsExpired(next_update_event_));
        next_update_event_ = ns3::Simulator::Schedule(ns3::Seconds(1.0 / kUpdateFrequencyHz),
                                                      &AGVPhysicalBasic::sendPosUpdate, this);
        break;
      }
      default:
        throw std::runtime_error("unhandled message");
    }
  }
}

void AGVPhysicalBasic::sendPosUpdate() {
  daisi::cpps::CppsTCPMessage message;
  message.addMessage({generatePosUpdate(), 2});

  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);
  socket_->Send(packet);
  next_update_event_ = ns3::Simulator::Schedule(ns3::Seconds(1.0 / kUpdateFrequencyHz),
                                                &AGVPhysicalBasic::sendPosUpdate, this);
}

std::string AGVPhysicalBasic::generatePosUpdate() {
  using namespace daisi::path_planning::message;
  ns3::Vector pos = mobility_->GetPosition();
  PositionUpdate update{pos.x, pos.y, pos.z};
  return serialize<FieldMessage>(update);
}

}  // namespace daisi::cpps
