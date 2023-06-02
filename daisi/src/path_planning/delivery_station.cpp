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

#include "delivery_station.h"

#include <utility>

#include "cpps/common/uuid_generator.h"
#include "minhton/utils/config_reader.h"
#include "path_planning/consensus/paxos/constants.h"
#include "path_planning/message/serializer.h"
#include "utils/daisi_check.h"
#include "utils/sola_utils.h"

namespace daisi::path_planning {

DeliveryStation::DeliveryStation(DeliveryStationInfo info,
                                 consensus::ConsensusSettings consensus_settings,
                                 std::shared_ptr<PathPlanningLoggerNs3> logger, uint32_t device_id)
    : device_id_(device_id),
      info_(info),
      consensus_settings_(std::move(consensus_settings)),
      logger_(std::move(logger)) {
  logger_->setApplicationUUID(UUIDGenerator::get()());
  logger_->logStation(getFullName(), "delivery", info_.center_pos);
  init();
}

void DeliveryStation::init() {
  const std::string config_file = "configurations/join.yml";

  sola::ManagementOverlayMinhton::Config config_mo = minhton::config::readConfig(config_file);

  sola_ns3::SOLAWrapperNs3::setJoinIp(config_mo);

  // Nothing to configure (yet)
  sola::EventDisseminationMinhcast::Config config_ed;

  sola_ = std::make_shared<sola_ns3::SOLAWrapperNs3>(
      config_mo, config_ed,
      [&](const sola::Message &m) { throw std::runtime_error("Should not receive msg"); },
      [&](const sola::TopicMessage &m) { processTopicMessage(m); }, logger_, getFullName(),
      device_id_);
}

void DeliveryStation::initReplication() {
  replication_manager_.emplace(std::to_string(info_.station_id), logger_,
                               std::get<consensus::PaxosSettings>(consensus_settings_));
  sola_->subscribeTopic(consensus::kReplicationTopic);
}

void DeliveryStation::postInit() {
  if (const auto paxos = std::get_if<consensus::PaxosSettings>(&consensus_settings_)) {
    DAISI_CHECK(!paxos->delivery_active_participate,
                "Delivery station cannot actively participate in paxos");
    if (paxos->replication) initReplication();
  }
}

std::string DeliveryStation::getFullName() const {
  return "/station " + std::to_string(info_.station_id);
}

void DeliveryStation::processTopicMessage(const sola::TopicMessage &message) {
  if (std::holds_alternative<consensus::PaxosSettings>(consensus_settings_)) {
    assert(message.topic == consensus::kReplicationTopic);
    auto replication_msg = message::deserialize<consensus::ReplicationMessage>(message.content);
    DAISI_CHECK(replication_manager_.has_value(), "Replication manager not initialized");
    replication_manager_->processReplicationMessage(replication_msg);
    logger_->logRecvPathPlanningTopicTraffic(
        consensus::kReplicationTopic, getFullName(), replication_msg.instance_id,
        static_cast<uint32_t>(consensus::ConsensusMsgTypes::kReplication));
  } else {
    throw std::runtime_error("unhandled message");
  }
}

}  // namespace daisi::path_planning
