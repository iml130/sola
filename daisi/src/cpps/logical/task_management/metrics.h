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

#ifndef DAISI_CPPS_LOGICAL_TASK_MANAGEMENT_METRICS_H_
#define DAISI_CPPS_LOGICAL_TASK_MANAGEMENT_METRICS_H_

#include <algorithm>
#include <functional>

#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

class Metrics {
public:
  Metrics() = default;

  Metrics(daisi::util::Duration empty_travel_time, daisi::util::Duration loaded_travel_time,
          daisi::util::Duration action_time, daisi::util::Distance empty_travel_distance,
          daisi::util::Distance loaded_travel_distance);

  void setMakespan(const daisi::util::Duration &makespan);

  bool isStartTimeSet() const;
  void setStartTime(const daisi::util::Duration &start_time);

  bool isExecutionStartTimeSet() const;
  void setExecutionStartTime(const daisi::util::Duration &start_time);

  daisi::util::Duration getMakespan() const;

  daisi::util::Duration getTime() const;

  daisi::util::Duration getExecutionTime() const;

  daisi::util::Distance getDistance() const;

  Metrics operator-(const Metrics &other) const;

  Metrics operator+(const Metrics &other) const;

  friend bool operator<(const Metrics &lhs, const Metrics &rhs);
  friend bool operator<=(const Metrics &lhs, const Metrics &rhs);
  friend bool operator>(const Metrics &lhs, const Metrics &rhs);
  friend bool operator>=(const Metrics &lhs, const Metrics &rhs);
  friend bool operator==(const Metrics &lhs, const Metrics &rhs);
  friend bool operator!=(const Metrics &lhs, const Metrics &rhs);

  static std::function<double(const Metrics &)> utility_function_;

  daisi::util::Duration empty_travel_time = 0.0;
  daisi::util::Duration loaded_travel_time = 0.0;
  daisi::util::Duration action_time = 0.0;
  daisi::util::Distance empty_travel_distance = 0.0;
  daisi::util::Distance loaded_travel_distance = 0.0;

  util::Duration start_up_time = 0.0;

  SERIALIZE(empty_travel_time, loaded_travel_time, action_time, empty_travel_distance,
            loaded_travel_distance, makespan_, start_time_, execution_start_time_, start_time_set_,
            execution_start_time_set_, start_up_time);

private:
  daisi::util::Duration makespan_ = 0.0;

  daisi::util::Duration start_time_ = 0.0;
  bool start_time_set_ = false;

  daisi::util::Duration execution_start_time_ = 0.0;
  bool execution_start_time_set_ = false;
};

}  // namespace daisi::cpps::logical

#endif
