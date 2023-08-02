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

SOLAWrapperNs3::SOLAWrapperNs3(const sola::ManagementOverlayMinhton::Config &config_mo,
                               const sola::EventDisseminationMinhcast::Config &config_ed,
                               sola::MessageReceiveFct receive_fct,
                               sola::TopicMessageReceiveFct topic_recv, std::string node_name)
    : SOLA(config_mo, config_ed, receive_fct, topic_recv,
           daisi::global_logger_manager->createSolaLogger()),
      node_name_(std::move(node_name)) {}

void SOLAWrapperNs3::subscribeTopic(const std::string &topic) {
  if (!isSubscribed(topic)) {
    SOLA::subscribeTopic(topic);

    subscribed_topics_.push_back(topic);
  }
}

void SOLAWrapperNs3::unsubscribeTopic(const std::string &topic) {
  if (isSubscribed(topic)) {
    SOLA::unsubscribeTopic(topic);

    subscribed_topics_.erase(
        std::remove(subscribed_topics_.begin(), subscribed_topics_.end(), topic),
        subscribed_topics_.end());
  }
}

void SOLAWrapperNs3::publishMessage(const std::string &topic, const std::string &serialized_message,
                                    const std::string &logging_content) {
  SOLA::publishMessage(topic, serialized_message);
}

bool SOLAWrapperNs3::isSubscribed(const std::string &topic) {
  return std::find(subscribed_topics_.begin(), subscribed_topics_.end(), topic) !=
         subscribed_topics_.end();
}
}  // namespace daisi::sola_ns3
