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

#include "time_window.h"

namespace daisi::material_flow {

TimeWindow::TimeWindow(const util::Duration &earliest_start, const util::Duration &latest_finish)
    : earliest_start_(earliest_start), latest_finish_(latest_finish) {}

TimeWindow::TimeWindow(const util::Duration &earliest_start, const util::Duration &latest_finish,
                       const util::Duration &spawn_time)
    : earliest_start_(earliest_start), latest_finish_(latest_finish), spawn_time_(spawn_time) {}

const util::Duration &TimeWindow::getRelativeEarliestStart() const { return earliest_start_; }

const util::Duration &TimeWindow::getRelativeLatestFinish() const { return latest_finish_; }

util::Duration TimeWindow::getAbsoluteEarliestStart() const {
  if (hasSpawnTime()) {
    return earliest_start_ + spawn_time_.value();
  }
  throw std::runtime_error("Spawn time not set");
}

util::Duration TimeWindow::getAbsoluteLatestFinish() const {
  if (hasSpawnTime()) {
    return spawn_time_.value() + latest_finish_;
  }
  throw std::runtime_error("Spawn time not set");
}

bool TimeWindow::hasSpawnTime() const { return spawn_time_.has_value(); }

void TimeWindow::setSpawnTime(const util::Duration &spawn_time) { spawn_time_ = spawn_time; }

}  // namespace daisi::material_flow
