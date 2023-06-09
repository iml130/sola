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

#ifndef DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_METRICS_H_
#define DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_METRICS_H_

#include <algorithm>
#include <functional>

#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::order_management {

class Metrics {
public:
  Metrics()
      : empty_travel_time(0.0),
        loaded_travel_time(0.0),
        action_time(0.0),
        empty_travel_distance(0.0),
        loaded_travel_distance(0.0),
        makespan_(0.0),
        start_time_(0.0) {}

  Metrics(daisi::util::Duration empty_travel_time, daisi::util::Duration loaded_travel_time,
          daisi::util::Duration action_time, daisi::util::Distance empty_travel_distance,
          daisi::util::Distance loaded_travel_distance)
      : empty_travel_time(empty_travel_time),
        loaded_travel_time(loaded_travel_time),
        action_time(action_time),
        empty_travel_distance(empty_travel_distance),
        loaded_travel_distance(loaded_travel_distance),
        makespan_(0.0),
        start_time_(0.0) {}

  static void setMetricUtilityMapper(std::function<double(const Metrics &)> mapper) {
    utility_function_ = mapper;
  }

  void setMakespan(const daisi::util::Duration &makespan) {
    if (!isStartTimeSet()) {
      makespan_ = makespan;
    } else {
      throw std::invalid_argument("makespan cannot be set if start_time_ is set");
    }
  }

  void setStartTime(const daisi::util::Duration &start_time) { start_time_ = start_time; }

  bool isStartTimeSet() const { return start_time_ != 0; }

  daisi::util::Duration getMakespan() const {
    if (makespan_ > 0) {
      return makespan_;
    }

    return start_time_ + getTime();
  }

  daisi::util::Duration getTime() const {
    return empty_travel_time + loaded_travel_time + action_time;
  }

  daisi::util::Distance getDistance() const {
    return empty_travel_distance + loaded_travel_distance;
  }

  Metrics operator-(const Metrics &other) const {
    Metrics diff{empty_travel_time - other.empty_travel_time,
                 loaded_travel_time - other.loaded_travel_time, action_time - other.action_time,
                 empty_travel_distance - other.empty_travel_distance,
                 loaded_travel_distance - other.loaded_travel_distance};

    diff.setMakespan(std::min(getMakespan(), other.getMakespan()));
    return diff;
  }

  Metrics operator+(const Metrics &other) const {
    Metrics sum{empty_travel_time + other.empty_travel_time,
                loaded_travel_time + other.loaded_travel_time, action_time + other.action_time,
                empty_travel_distance + other.empty_travel_distance,
                loaded_travel_distance + other.loaded_travel_distance};

    sum.setMakespan(std::max(getMakespan(), other.getMakespan()));
    return sum;
  }

  inline friend bool operator<(const Metrics &lhs, const Metrics &rhs);

  inline friend bool operator==(const Metrics &lhs, const Metrics &rhs);

  static std::function<double(const Metrics &)> utility_function_;

  daisi::util::Duration empty_travel_time;
  daisi::util::Duration loaded_travel_time;
  daisi::util::Duration action_time;
  daisi::util::Distance empty_travel_distance;
  daisi::util::Distance loaded_travel_distance;

  SERIALIZE(empty_travel_time, loaded_travel_time, action_time, empty_travel_distance,
            loaded_travel_distance, makespan_, start_time_);

private:
  daisi::util::Duration makespan_ = 0.0;
  daisi::util::Duration start_time_ = 0.0;
};

bool operator<(const Metrics &lhs, const Metrics &rhs) {
  return Metrics::utility_function_(lhs) < Metrics::utility_function_(rhs);
}

bool operator==(const Metrics &lhs, const Metrics &rhs) {
  return Metrics::utility_function_(lhs) == Metrics::utility_function_(rhs);
}

}  // namespace daisi::cpps::order_management

#endif
