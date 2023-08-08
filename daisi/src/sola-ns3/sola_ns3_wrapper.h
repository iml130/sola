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

#ifndef DAISI_SOLA_NS3_SOLA_NS3_WRAPPER_H_
#define DAISI_SOLA_NS3_SOLA_NS3_WRAPPER_H_

#include <cstring>

#include "SOLA/event_dissemination_minhcast.h"
#include "SOLA/management_overlay_minhton.h"
#include "SOLA/sola.h"
#include "logging/logger_manager.h"

namespace daisi::sola_ns3 {

class SOLAWrapperNs3
    : public sola::SOLA<sola::ManagementOverlayMinhton, sola::EventDisseminationMinhcast> {
public:
  SOLAWrapperNs3(const sola::ManagementOverlayMinhton::Config &config_mo,
                 const sola::EventDisseminationMinhcast::Config &config_ed,
                 sola::MessageReceiveFct receive_fct, sola::TopicMessageReceiveFct topic_recv,
                 std::string node_name_);

  void subscribeTopic(const std::string &topic);
  void unsubscribeTopic(const std::string &topic);
  solanet::UUID publishMessage(const std::string &topic, const std::string &serialized_message);

  bool isSubscribed(const std::string &topic);


private:
  std::string node_name_;

  std::vector<std::string> subscribed_topics_;
};
}  // namespace daisi::sola_ns3

#endif
