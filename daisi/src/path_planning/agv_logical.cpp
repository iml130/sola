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

#include "agv_logical.h"

#include <unordered_map>
#include <utility>

#include "cpps/common/agv_description.h"
#include "cpps/common/uuid_generator.h"
#include "cpps/packet.h"
#include "minhton/utils/config_reader.h"
#include "path_planning/consensus/constants.h"
#include "path_planning/consensus/paxos/constants.h"
#include "path_planning/message/serializer.h"
#include "sola-ns3/sola_ns3_wrapper.h"
#include "utils/daisi_check.h"
#include "utils/sola_utils.h"

namespace daisi::path_planning {

AGVLogical::AGVLogical(cpps::TopologyNs3 topology, consensus::ConsensusSettings consensus_settings,
                       bool first_node, const ns3::Ptr<ns3::Socket> &socket, uint32_t device_id)
    : device_id_(device_id),
      socket_(socket),
      first_node_(first_node),
      uuid_(UUIDGenerator::get()()),
      topology_(std::move(topology)),
      consensus_settings_(std::move(consensus_settings)) {
  logger_ = daisi::global_logger_manager->createPathPlanningLogger(device_id_, "AGVLogical");
  logger_->setApplicationUUID(uuid_);
  init();
}

bool AGVLogical::connectionRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  // Accept all connections
  return true;
}

void AGVLogical::processMessageDescription(const std::string &payload) {
  cpps::AgvDataModel data;

  std::istringstream stream(payload);
  stream >> data;
  kinematics_ = data.agv_properties.kinematic;

  logAGV();
  std::cout << "AGV logical registered" << std::endl;
}

void AGVLogical::processMessageUpdate(const message::PositionUpdate &msg) {
  last_x_ = msg.x;
  last_y_ = msg.y;

  daisi::cpps::AGVPositionLoggingInfo info;
  info.uuid = uuid_;
  info.x = msg.x;
  info.y = msg.y;
  info.z = msg.z;
  info.state = 0;

  logger_->logPositionUpdate(info);
}

void AGVLogical::readFromSocket(ns3::Ptr<ns3::Socket> socket) {
  auto packet = socket->Recv();
  cpps::CppsTCPMessage header;
  packet->RemoveHeader(header);

  for (auto &msg : header.getMessages()) {
    uint8_t type = msg.type;
    switch (type) {
      case 0:
        processMessageDescription(msg.payload);
        break;
      case 2:
        processMessageField(message::deserialize<message::FieldMessage>(msg.payload));
        break;
      default:
        throw std::runtime_error("invalid packet type for logical agv");
    }
  }
}

void AGVLogical::newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr) {
  socket_agv_ = socket;
  socket_agv_->SetRecvCallback(MakeCallback(&AGVLogical::readFromSocket, this));
}

void AGVLogical::logAGV() const {
  daisi::cpps::AGVLoggingInfo info;
  // info.friendly_name = data_model_.agv_properties.friendly_name;
  // info.manufacturer = data_model_.agv_properties.manufacturer;
  // info.model_name = data_model_.agv_properties.model_name;
  // info.model_number = data_model_.agv_properties.model_number;
  // info.serial_number = data_model_.agv_device_descr.serial_number;
  info.ip_logical_core = sola_->getIP();
  info.port_logical_core = sola_->getPort();
  // info.ip_logical_asset_ = agv_logical_asset_ip_;
  info.port_logical_asset = 0;
  // info.ip_physical_ = agv_physical_ip_;
  info.port_physical = 0;

  info.load_time = 0;
  info.unload_time = 0;
  // info.load_carrier_type_ = std::get<1>(data_model_.agv_properties.ability).getTypeAsString();
  info.max_weight = 0;

  info.max_velocity = 0;
  info.min_velocity = 0;
  info.max_acceleration = 0;
  info.min_acceleration = 0;

  logger_->logAGV(info);
}

void AGVLogical::init() {
  // SOLA --------------

  const std::string config_file =
      first_node_ ? "configurations/root.yml" : "configurations/join.yml";

  sola::ManagementOverlayMinhton::Config config_mo = minhton::config::readConfig(config_file);

  if (!first_node_) {
    sola_ns3::SOLAWrapperNs3::setJoinIp(config_mo);
  }

  // Nothing to configure (yet)
  sola::EventDisseminationMinhcast::Config config_ed;

  sola_ = std::make_shared<sola_ns3::SOLAWrapperNs3>(
      config_mo, config_ed, [this](const sola::Message &m) { processMessage(m); },
      [&](const sola::TopicMessage &m) { processTopicMessage(m); }, logger_, uuid_, device_id_);

  // ns3 ----------------------------
  socket_->Listen();
  socket_->SetAcceptCallback(ns3::MakeCallback(&AGVLogical::connectionRequest, this),
                             ns3::MakeCallback(&AGVLogical::newConnectionCreated, this));

  ns3::Address addr;
  socket_->GetSockName(addr);
}

std::string AGVLogical::getConnectionString() const { return sola_->getConectionString(); }

void AGVLogical::processTopicMessage(const sola::TopicMessage &msg) {
  if (std::holds_alternative<consensus::PaxosSettings>(consensus_settings_)) {
    assert(msg.topic == consensus::kReplicationTopic);
    auto replication_msg = message::deserialize<consensus::ReplicationMessage>(msg.content);
    logger_->logRecvPathPlanningTopicTraffic(
        msg.topic, uuid_, replication_msg.instance_id,
        static_cast<uint32_t>(consensus::ConsensusMsgTypes::kReplication));
    DAISI_CHECK(replication_manager_.has_value(), "Replication manager not initialized");
    replication_manager_->processReplicationMessage(replication_msg);
  } else {
    throw std::runtime_error("unhandled message");
  }
}

void AGVLogical::registerByAuthority(const std::string &ip) {
  message::NewAuthorityAGV new_msg{sola_->getConectionString(),
                                   uuid_,
                                   kinematics_.getMinAcceleration(),
                                   kinematics_.getMaxAcceleration(),
                                   kinematics_.getMinVelocity(),
                                   kinematics_.getMaxVelocity(),
                                   kinematics_.getLoadTime(),
                                   kinematics_.getUnloadTime(),
                                   last_x_,
                                   last_y_};
  current_authority_ip_ = ip.substr(0, ip.find(':'));
  current_authority_port_ = std::stoi(ip.substr(ip.find(':') + 1, ip.length()));
  logger_->logSendPathPlanningTraffic(uuid_, std::to_string(current_authority_station_id_), 2);
  sola_->sendData(message::serialize<message::MiscMessage>(new_msg),
                  sola::Endpoint(ip));  // Introduce ourselves by new owner

  postInit();
}

void AGVLogical::initReplication() {
  replication_manager_.emplace(uuid_, logger_,
                               std::get<consensus::PaxosSettings>(consensus_settings_));
  sola_->subscribeTopic(consensus::kReplicationTopic);
}

void AGVLogical::postInit() {
  if (const auto paxos = std::get_if<consensus::PaxosSettings>(&consensus_settings_)) {
    DAISI_CHECK(!paxos->amr_active_participate, "AMR cannot actively participate in paxos");
    if (paxos->replication) initReplication();
  }
}

void AGVLogical::processMessage(const sola::Message &msg) {
  if (msg.sender != current_authority_ip_)
    throw std::runtime_error("No authority to send messages");
  auto message = message::deserialize<message::StationAGVMessage>(msg.content);
  if (auto drive_msg = std::get_if<message::DriveMessage>(&message)) {
    processDriveMessage(*drive_msg);
  } else if (auto handover_msg = std::get_if<message::HandoverMessage>(&message)) {
    processHandoverMessage(*handover_msg);
  } else {
    throw std::runtime_error("invalid message type");
  }
}

void AGVLogical::processDriveMessage(const message::DriveMessage &msg) {
  logger_->logRecvPathPlanningTraffic(std::to_string(current_authority_station_id_), uuid_, 1);

  ns3::Vector2D goal = msg.getGoal();

  message::DriveMessageField field_msg{goal.x, goal.y};

  cpps::CppsTCPMessage message;
  message.addMessage(
      {message::serialize<message::FieldMessage>(field_msg), 2});  // 2 -> Field Message

  ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>();
  packet->AddHeader(message);

  socket_agv_->Send(packet);
}

void AGVLogical::processReachedGoal() {
  // Notify station
  message::ReachedGoal goal{uuid_, last_x_, last_y_};
  logger_->logSendPathPlanningTraffic(uuid_, std::to_string(current_authority_station_id_), 3);
  sola_->sendData(message::serialize<message::MiscMessage>(goal),
                  {current_authority_ip_, current_authority_port_});
}

void AGVLogical::processMessageField(const message::FieldMessage &msg) {
  if (auto pos_update = std::get_if<message::PositionUpdate>(&msg)) {
    processMessageUpdate(*pos_update);
  } else if (auto reached_goal = std::get_if<message::ReachedGoalField>(&msg)) {
    processReachedGoal();
  } else {
    throw std::runtime_error("unhandled message");
  }
}

void AGVLogical::processHandoverMessage(const message::HandoverMessage &msg) {
  current_authority_station_id_ = msg.getNextStation();
  current_authority_ip_ = msg.getNextStationIP();
  // Not updating current_authority_port_ because currently all stations using the same port

  // Introduce ourselves by new owner
  message::NewAuthorityAGV new_msg{sola_->getConectionString(),
                                   uuid_,
                                   kinematics_.getMinAcceleration(),
                                   kinematics_.getMaxAcceleration(),
                                   kinematics_.getMinVelocity(),
                                   kinematics_.getMaxVelocity(),
                                   kinematics_.getLoadTime(),
                                   kinematics_.getUnloadTime(),
                                   last_x_,
                                   last_y_};
  new_msg.initial = false;
  new_msg.current_del_dest_x = msg.getDriveMessage().getGoal().x;
  new_msg.current_del_dest_y = msg.getDriveMessage().getGoal().y;
  logger_->logSendPathPlanningTraffic(uuid_, std::to_string(current_authority_station_id_), 2);
  sola_->sendData(message::serialize<message::MiscMessage>(new_msg),
                  {current_authority_ip_,
                   current_authority_port_});  // TODO Must receive first before reachedGoal()

  processDriveMessage(msg.getDriveMessage());
}

}  // namespace daisi::path_planning
