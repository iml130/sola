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

#include "route_calculation_helper.h"

#include <cmath>

#include "utils/daisi_check.h"

namespace daisi::path_planning::consensus {
double RouteCalculationHelper::calculatePossibleStartTime(
    const PointTimePairs &points, double initial_start_time, double max_preplanning_time_s,
    double time_between_intersections_s, const IntersectionOccupancy &intersection_occupancy) {
  // Step size to increase possible start times
  const double delta_s = 0.001;
  auto start_time = [delta_s, initial_start_time](int i) {
    return initial_start_time + delta_s * i;
  };
  bool possible = false;
  const int retries = static_cast<int>(max_preplanning_time_s) / delta_s;
  int i = 0;
  for (; i < retries && !possible; i++) {
    // Increment start time by \p delta_s until a conflict free occupancy is found or \p
    // max_preplanning_time_s is reached
    possible =
        possibleRoute(points, start_time(i), time_between_intersections_s, intersection_occupancy);
  }
  double actual_start_time =
      possible ? start_time(i - 1) : std::numeric_limits<double>::quiet_NaN();

  if (!std::isnan(actual_start_time)) {
    DAISI_CHECK(initial_start_time <= actual_start_time, "Cannot start before initial start time");
  }

  return actual_start_time;
}

bool RouteCalculationHelper::possibleRoute(const PointTimePairs &points, double start_time_s,
                                           double time_between_intersections_s,
                                           const IntersectionOccupancy &intersection_occupancy) {
  for (auto point : points) {
    if (intersection_occupancy.count({point.first.x, point.first.y}) == 0) {
      // Point not occupied at all and therefore free
      continue;
    }
    auto entry = intersection_occupancy.at({point.first.x, point.first.y});
    if (entry.empty()) {
      // Point is already known but is not occupied at all.
      continue;
    }

    double time_at_point = point.second + start_time_s;

    // Other occupancy of this intersection directly at this time
    if (entry.find(time_at_point) != entry.end()) return false;

    // Check if intersection is occupied in delta range
    // Get next greater timestamp when this intersection is occupied
    auto next_upper_it = entry.upper_bound(time_at_point);

    if (next_upper_it != entry.end() &&
        next_upper_it->first <= time_at_point + time_between_intersections_s) {
      // Other occupancy shortly after our wanted occupancy within delta range.
      return false;
    }
    if (next_upper_it != entry.begin()) {
      next_upper_it--;
      if (next_upper_it->first >= time_at_point - time_between_intersections_s) {
        // Other occupancy shortly before our wanted occupancy within delta range.
        return false;
      }
    }
  }
  return true;
}

}  // namespace daisi::path_planning::consensus
