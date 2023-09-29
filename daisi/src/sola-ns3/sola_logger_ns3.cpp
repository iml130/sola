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

#include "sola_logger_ns3.h"

#include <stdexcept>

#include "ns3/simulator.h"
#include "solanet/uuid.h"

namespace daisi::sola_ns3 {

#define TableDefinition static const DatabaseTable
#define ViewDefinition static const std::unordered_map<std::string, std::string>

SolaLoggerNs3::SolaLoggerNs3(LogDeviceApp log_device_application, LogFunction log)
    : sola::LoggerInterface("NOT-INITIALIZED"),
      log_device_application_(std::move(log_device_application)),
      log_(std::move(log)) {}

SolaLoggerNs3::~SolaLoggerNs3() {
  auto current_time = ns3::Simulator::Now().GetMilliSeconds();
  log_(toSQL("UPDATE DeviceApplication SET StopTime_ms=%lu WHERE ApplicationUuid='%s';",
             current_time, uuid_.c_str()));
}

// * TopicEvent
TableDefinition kTopicEvent("SolaTopicEvent", {DatabaseColumnInfo{"Id"},
                                               {"Timestamp_ms", "%lu", true},
                                               {"Topic", "%s", true},
                                               {"SolaApplicationId", "%s", true},
                                               {"Subscribe", "%u", true}});
static const std::string kCreateTopicEvent = getCreateTableStatement(kTopicEvent);
static bool topic_event_exists_ = false;

void SolaLoggerNs3::logSubscribeTopic(const std::string &topic) const {
  logTopicEvent(topic, kSubscribe);
};

void SolaLoggerNs3::logUnsubscribeTopic(const std::string &topic) const {
  logTopicEvent(topic, kUnsubscribe);
};

void SolaLoggerNs3::logTopicEvent(const std::string &topic, TopicEventType event_type) const {
  if (!topic_event_exists_) {
    log_(kCreateTopicEvent);
    topic_event_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Topic */ topic.c_str(),
      /* SolaApplicationId */ LoggerInterface::uuid_.c_str(),
      /* Subscribe */ event_type);
  log_(getInsertStatement(kTopicEvent, t));
}

// * SolaTopicMessage
TableDefinition kTopicMessage("SolaTopicMessage", {DatabaseColumnInfo{"Id"},
                                                   {"Timestamp_ms", "%lu", true},
                                                   {"Topic", "%s", true},
                                                   {"MessageUuid", "%s", true},
                                                   {"SolaApplicationId", "%s", true},
                                                   {"Receive", "%u", true}});
static const std::string kCreateTopicMessage = getCreateTableStatement(kTopicMessage);
static bool topic_message_exists_ = false;

void SolaLoggerNs3::logPublishTopicMessage(const sola::TopicMessage &msg) const {
  logTopicMessage(msg, kPublish);
};

void SolaLoggerNs3::logReceiveTopicMessage(const sola::TopicMessage &msg) const {
  logTopicMessage(msg, kReceive);
};

void SolaLoggerNs3::logTopicMessage(const sola::TopicMessage &msg, TopicPublishType type) const {
  if (!topic_message_exists_) {
    log_(kCreateTopicMessage);
    topic_message_exists_ = true;
  }

  // Store msg_uuid separately due to lifetime/invalidation of .c_str() in tuple
  std::string msg_uuid = solanet::uuidToString(msg.uuid);

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Topic */ msg.topic.c_str(),
      /* MessageUuid */ msg_uuid.c_str(),
      /* SolaApplicationId */ uuid_.c_str(),
      /* Receive */ type);
  log_(getInsertStatement(kTopicMessage, t));
}

TableDefinition kMessageIdMapping("SolaMessageIdMapping",
                                  {
                                      DatabaseColumnInfo{"Id"},
                                      {"SolaMessageUuid", "%s", true},
                                      {"EventDisseminationMessageUuid", "%s", true},
                                  });
static const std::string kCreateMessageIdMapping = getCreateTableStatement(kMessageIdMapping);
static bool message_id_mapping_exists = false;

void SolaLoggerNs3::logMessageIDMapping(const solanet::UUID &sola_msg_uuid,
                                        const solanet::UUID &ed_msg_uuid) const {
  if (!message_id_mapping_exists) {
    log_(kCreateMessageIdMapping);
    message_id_mapping_exists = true;
  }

  // Store msg_uuid separately due to lifetime/invalidation of .c_str() in tuple
  std::string sola_msg_uuid_str = solanet::uuidToString(sola_msg_uuid);
  std::string ed_msg_uuid_str = solanet::uuidToString(ed_msg_uuid);

  auto t = std::make_tuple(
      /* SolaMessageUuid */ sola_msg_uuid_str.c_str(),
      /* EventDisseminationMessageUuid */ ed_msg_uuid_str.c_str());
  log_(getInsertStatement(kMessageIdMapping, t));
}

void SolaLoggerNs3::setApplicationUUID(const std::string &app_uuid) {
  LoggerInterface::uuid_ = app_uuid;
  log_device_application_(uuid_);
};

}  // namespace daisi::sola_ns3
