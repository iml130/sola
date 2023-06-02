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

#ifndef DAISI_MODEL_TIME_WINDOW_NS3_H_
#define DAISI_MODEL_TIME_WINDOW_NS3_H_

#include <cassert>
#include <optional>

#include "cpps/message/serialize.h"

namespace daisi::cpps {

struct TimeWindow {
public:
  TimeWindow()
      : earliest_start_(0), latest_finish_(0), duration_(0), duration_set_(false), spawn_time_(0) {}

  TimeWindow(const double &earliest_start, const double &latest_finish, const double &duration)
      : earliest_start_(earliest_start),
        latest_finish_(latest_finish),
        duration_(duration),
        duration_set_(true),
        spawn_time_(0) {}

  TimeWindow(const double &earliest_start, const double &latest_finish)
      : earliest_start_(earliest_start),
        latest_finish_(latest_finish),
        duration_(0),
        duration_set_(false),
        spawn_time_(0) {}

  TimeWindow(const double &earliest_start, const double &latest_finish, const double &duration,
             const double &spawn_time)
      : earliest_start_(earliest_start),
        latest_finish_(latest_finish),
        duration_(duration),
        duration_set_(true),
        spawn_time_(spawn_time) {}

  double getEarliestStart() const { return spawn_time_ + earliest_start_; }
  double getEarliestFinish() const { return spawn_time_ + earliest_start_ + getDuration(); }
  double getLatestStart() const { return spawn_time_ + latest_finish_ - getDuration(); }
  double getLatestFinish() const { return spawn_time_ + latest_finish_; }
  double getSpawnTime() const { return spawn_time_; }

  void constraintEarliestStart(const double &earliest_start) {
    earliest_start_ = std::max(earliest_start_, earliest_start - spawn_time_);
  }
  void constraintLatestFinish(const double &latest_finish) {
    latest_finish_ = std::min(latest_finish_, latest_finish - spawn_time_);
  }

  double getDuration() const {
    assert(duration_set_);
    return duration_;
  }
  void setDuration(const double &duration) {
    duration_ = duration;
    duration_set_ = true;
  }
  void setSpawnTime(const double &spawn_time) { spawn_time_ = spawn_time; }

  bool isValid() const { return getLatestFinish() - getEarliestStart() >= getDuration(); }
  double getBuffer() const { return getLatestFinish() - getEarliestStart() - getDuration(); }

  SERIALIZE(earliest_start_, latest_finish_, duration_, spawn_time_);

private:
  double earliest_start_;
  double latest_finish_;
  double duration_;
  bool duration_set_;
  double spawn_time_;
};

}  // namespace daisi::cpps

#endif
