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

#ifndef DAISI_SOLA_NS3_SOLA_LOGGER_NS3_H_
#define DAISI_SOLA_NS3_SOLA_LOGGER_NS3_H_

#include <ctime>
#include <unordered_map>

#include "SOLA/logger_interface.h"
#include "logging/definitions.h"
#include "logging/sqlite/sqlite_helper.h"

namespace daisi::sola_ns3 {

class SolaLoggerNs3 final : public sola::LoggerInterface {
public:
  SolaLoggerNs3(LogDeviceApp log_device_application, LogFunction log);

  ~SolaLoggerNs3() override;

  void logSubscribeTopic(const std::string &topic) const override;

  void logUnsubscribeTopic(const std::string &topic) const override;

  void logPublishTopicMessage(const sola::TopicMessage &msg) const override;

  void logReceiveTopicMessage(const sola::TopicMessage &msg) const override;

  void setApplicationUUID(const std::string &app_uuid) override;

  void logMessageIDMapping(const solanet::UUID &sola_msg_uuid,
                           const solanet::UUID &ed_msg_uuid) const override;

private:
  enum TopicEventType {
    kUnsubscribe,
    kSubscribe,
  };

  enum TopicPublishType {
    kPublish,
    kReceive,
  };

  void logTopicEvent(const std::string &topic, TopicEventType event_type) const;

  void logTopicMessage(const sola::TopicMessage &ms, TopicPublishType type) const;

  LogDeviceApp log_device_application_;
  LogFunction log_;
};

}  // namespace daisi::sola_ns3

#endif
