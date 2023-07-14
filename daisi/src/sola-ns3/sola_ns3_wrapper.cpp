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

#include "sola_ns3_wrapper.h"

#include "utils/sola_utils.h"

namespace daisi::sola_ns3 {

sola::ManagementOverlayMinhton::Config addLogger(sola::ManagementOverlayMinhton::Config config,
                                                 uint32_t device_id) {
  config.setLogger(std::vector<sola::ManagementOverlayMinhton::Logger>{
      daisi::global_logger_manager->createMinhtonLogger(device_id, "MO")});
  return config;
}

sola::EventDisseminationMinhcast::Config addLogger(sola::EventDisseminationMinhcast::Config config,
                                                   uint32_t device_id) {
  config.logger = std::vector<sola::EventDisseminationMinhcast::Logger>{
      daisi::global_logger_manager->createNatterLogger(device_id)};
  return config;
}

SOLAWrapperNs3::SOLAWrapperNs3(const sola::ManagementOverlayMinhton::Config &config_mo,
                               const sola::EventDisseminationMinhcast::Config &config_ed,
                               sola::MessageReceiveFct receive_fct,
                               sola::TopicMessageReceiveFct topic_recv,
                               std::shared_ptr<daisi::cpps::CppsLoggerNs3> logger,
                               std::string node_name, uint32_t device_id)
    : SOLA(addLogger(config_mo, device_id), addLogger(config_ed, device_id), receive_fct,
           topic_recv),
      device_id_(device_id),
      logger_(std::move(logger)),
      node_name_(std::move(node_name)) {}

void SOLAWrapperNs3::subscribeTopic(const std::string &topic) {
  if (!isSubscribed(topic)) {
    std::string ip = getIP();

    SolaNetworkUtils::get().createSockets(ip);
    logger_->logTopicEvent(topic, node_name_, true);
    const std::string postfix = "ED:" + topic;
    std::vector<sola::ManagementOverlayMinhton::Logger> logger_list{
        daisi::global_logger_manager->createMinhtonLogger(device_id_, postfix)};
    SOLA::subscribeTopic(topic, logger_list);

    subscribed_topics_.push_back(topic);
  }
}

void SOLAWrapperNs3::unsubscribeTopic(const std::string &topic) {
  if (isSubscribed(topic)) {
    logger_->logTopicEvent(topic, node_name_, false);
    SOLA::unsubscribeTopic(topic);

    subscribed_topics_.erase(
        std::remove(subscribed_topics_.begin(), subscribed_topics_.end(), topic),
        subscribed_topics_.end());
  }
}

void SOLAWrapperNs3::publishMessage(const std::string &topic, const std::string &serialized_message,
                                    const std::string &logging_content) {
  logger_->logTopicMessage(topic, "", node_name_, logging_content, false);
  SOLA::publishMessage(topic, serialized_message);
}

bool SOLAWrapperNs3::isSubscribed(const std::string &topic) {
  return std::find(subscribed_topics_.begin(), subscribed_topics_.end(), topic) !=
         subscribed_topics_.end();
}
}  // namespace daisi::sola_ns3
