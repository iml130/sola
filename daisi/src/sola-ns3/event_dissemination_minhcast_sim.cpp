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

#include "SOLA/event_dissemination_minhcast.h"
#include "ns3/simulator.h"
#include "sola_check.h"

namespace sola {

// For debugging: Detecting invalid topic creations
static std::set<std::string> created_topics;

void EventDisseminationMinhcast::getResult(const std::string &topic,
                                           const std::function<void()> &on_result) {
  using namespace std::chrono_literals;
  auto res = result_.at(topic).wait_for(0ms);

  if (res == std::future_status::timeout) {
    ns3::Simulator::Schedule(ns3::MilliSeconds(10), &EventDisseminationMinhcast::getResult, this,
                             topic, on_result);
    return;
  }

  SOLA_CHECK(res == std::future_status::ready, "Result future is not ready yet");

  on_result();
}

void EventDisseminationMinhcast::checkTopicJoin(const std::string &topic, bool should_exist) {
  const bool topic_exist = created_topics.find(topic) != created_topics.end();
  if (should_exist) {
    SOLA_CHECK(topic_exist, "Tried joining topic, which does not exist already");
  } else {
    SOLA_CHECK(!topic_exist, "Tried to create topic tree but tree already exists")
    created_topics.insert(topic);
  }
}

}  // namespace sola
