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

#include <queue>
#include <unordered_map>

#include "core/watchdog.h"
#include "ns3/event-id.h"
#include "ns3/simulator.h"

namespace minhton::core {
class WatchDog::Impl {
public:
  void addJob(std::function<void()> function, uint32_t miliseconds, TimeoutType &timeout_type) {
    auto new_event =
        ns3::Simulator::Schedule(ns3::MilliSeconds(miliseconds), &WatchDog::Impl::callCallback,
                                 this, function, timeout_type);
    bool has_type = events_.find(timeout_type) != events_.end();

    if (!has_type) {
      events_[timeout_type] = std::queue<ns3::EventId>();
    }
    events_[timeout_type].push(new_event);
  }

  void callCallback(const std::function<void()> &callback, const TimeoutType &timeout_type) {
    callback();
    events_[timeout_type].pop();
  }

  void cancelJob(const TimeoutType &timeout_type) {
    // canceling all events of the timeout_type
    // usually there is only one event per type
    while (!events_[timeout_type].empty()) {
      events_[timeout_type].front().Cancel();
      events_[timeout_type].pop();
    }
  }

private:
  std::unordered_map<TimeoutType, std::queue<ns3::EventId>> events_;
};

WatchDog::WatchDog() : pimpl_(std::make_unique<Impl>()) {}

WatchDog::~WatchDog() = default;

void WatchDog::addJob(std::function<void()> function, uint32_t milliseconds,
                      TimeoutType &timeout_type) {
  pimpl_->addJob(function, milliseconds, timeout_type);
}

void WatchDog::cancelJob(const TimeoutType &timeout_type) { pimpl_->cancelJob(timeout_type); }
}  // namespace minhton::core
