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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_ROUTE_CALCULATION_HELPER_H_
#define DAISI_PATH_PLANNING_CONSENSUS_ROUTE_CALCULATION_HELPER_H_

#include "path_planning/constants.h"

namespace daisi::path_planning::consensus {
struct RouteCalculationHelper {
  /**
   * Calculate a possible conflict free global start time with the global knowledge of all
   * intersection occupancies
   * @param points requested route with relative timestamps
   * @param initial_start_time earliest possible start time
   * @param max_preplanning_time_s Maximum time in which the start time might be in the future
   * @param time_between_intersections_s time between occupancies of the same intersection
   * @param intersection_occupancy all global intersections occupancies
   * @return possible global start time in seconds or quiet_NaN() if no start time could be found
   */
  [[nodiscard]] static double calculatePossibleStartTime(
      const PointTimePairs &points, double initial_start_time, double max_preplanning_time_s,
      double time_between_intersections_s, const IntersectionOccupancy &intersection_occupancy);

private:
  /**
   * Check if the route from \p points and the given \p start_time is conflict free to all other
   * intersection occupancies
   * @param points requested route with relative timestamps
   * @param start_time global start time in seconds
   * @param time_between_intersections_s time between occupancies of the same intersection
   * @param intersection_occupancy all global intersections occupancies
   * @return true if the route occupancy is conflict free
   */
  [[nodiscard]] static bool possibleRoute(const PointTimePairs &points, double start_time_s,
                                          double time_between_intersections_s,
                                          const IntersectionOccupancy &intersection_occupancy);
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_ROUTE_CALCULATION_HELPER_H_
