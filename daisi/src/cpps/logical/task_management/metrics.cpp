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

#include "metrics.h"

namespace daisi::cpps::logical {

Metrics::Metrics(daisi::util::Duration empty_travel_time, daisi::util::Duration loaded_travel_time,
                 daisi::util::Duration action_time, daisi::util::Distance empty_travel_distance,
                 daisi::util::Distance loaded_travel_distance)
    : empty_travel_time(empty_travel_time),
      loaded_travel_time(loaded_travel_time),
      action_time(action_time),
      empty_travel_distance(empty_travel_distance),
      loaded_travel_distance(loaded_travel_distance) {}

void Metrics::setMakespan(const daisi::util::Duration &makespan) {
  if (!isStartTimeSet() && !isExecutionStartTimeSet()) {
    makespan_ = makespan;
  } else {
    throw std::invalid_argument("makespan cannot be set if start_time_ is set");
  }
}

void Metrics::setStartTime(const daisi::util::Duration &start_time) {
  if (!isExecutionStartTimeSet()) {
    start_time_ = start_time;
    start_time_set_ = true;
  } else {
    throw std::invalid_argument("Execution start time is already set");
  }
}

bool Metrics::isStartTimeSet() const { return start_time_set_; }

void Metrics::setExecutionStartTime(const daisi::util::Duration &execution_start_time) {
  if (!isStartTimeSet()) {
    execution_start_time_ = execution_start_time;
    execution_start_time_set_ = true;
  } else {
    throw std::invalid_argument("Start time is already set");
  }
}

bool Metrics::isExecutionStartTimeSet() const { return execution_start_time_set_; }

daisi::util::Duration Metrics::getMakespan() const {
  if (makespan_ > 0) {
    return makespan_;
  }

  if (isExecutionStartTimeSet()) {
    return execution_start_time_ + getExecutionTime();
  }

  if (isStartTimeSet()) {
    return start_time_ + getTime();
  }

  throw std::runtime_error("No makespan values are available.");
}

daisi::util::Duration Metrics::getTime() const {
  return empty_travel_time + loaded_travel_time + action_time;
}

daisi::util::Duration Metrics::getExecutionTime() const {
  return empty_travel_time + loaded_travel_time + action_time - start_up_time;
}

daisi::util::Distance Metrics::getDistance() const {
  return empty_travel_distance + loaded_travel_distance;
}

Metrics Metrics::operator-(const Metrics &other) const {
  Metrics diff{empty_travel_time - other.empty_travel_time,
               loaded_travel_time - other.loaded_travel_time, action_time - other.action_time,
               empty_travel_distance - other.empty_travel_distance,
               loaded_travel_distance - other.loaded_travel_distance};

  diff.setMakespan(abs(getMakespan() - other.getMakespan()));
  return diff;
}

Metrics Metrics::operator+(const Metrics &other) const {
  Metrics sum{empty_travel_time + other.empty_travel_time,
              loaded_travel_time + other.loaded_travel_time, action_time + other.action_time,
              empty_travel_distance + other.empty_travel_distance,
              loaded_travel_distance + other.loaded_travel_distance};

  sum.setMakespan(std::max(getMakespan(), other.getMakespan()));
  return sum;
}

bool operator<(const Metrics &lhs, const Metrics &rhs) {
  return Metrics::utility_function_(lhs) < Metrics::utility_function_(rhs);
}
bool operator<=(const Metrics &lhs, const Metrics &rhs) {
  return Metrics::utility_function_(lhs) <= Metrics::utility_function_(rhs);
}
bool operator>(const Metrics &lhs, const Metrics &rhs) {
  return Metrics::utility_function_(lhs) > Metrics::utility_function_(rhs);
}
bool operator>=(const Metrics &lhs, const Metrics &rhs) {
  return Metrics::utility_function_(lhs) >= Metrics::utility_function_(rhs);
}
bool operator==(const Metrics &lhs, const Metrics &rhs) {
  return Metrics::utility_function_(lhs) == Metrics::utility_function_(rhs);
}
bool operator!=(const Metrics &lhs, const Metrics &rhs) {
  return Metrics::utility_function_(lhs) != Metrics::utility_function_(rhs);
}

// TODO: add option to somehow define this function by runtime
std::function<double(const Metrics &)> Metrics::utility_function_ = [](const Metrics &m) {
  return -m.empty_travel_time;
};

}  // namespace daisi::cpps::logical
