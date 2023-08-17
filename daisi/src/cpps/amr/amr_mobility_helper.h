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

#ifndef DAISI_CPPS_AMR_AMR_MOBILITY_HELPER_H_
#define DAISI_CPPS_AMR_AMR_MOBILITY_HELPER_H_

#include <vector>

#include "cpps/amr/amr_description.h"
#include "cpps/amr/amr_topology.h"
#include "cpps/amr/physical/amr_mobility_status.h"
#include "cpps/amr/physical/functionality.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {
/// @brief
class AmrMobilityHelper {
public:
  /// @brief Estimates how much time (seconds) it takes to complete a list of \c
  /// FunctionalityVariant .
  /// @throws \c std::invalid_argument
  /// * if \c start_pose or any other Position is outside of \c Topology and check_positioning is
  /// true, or
  /// * if a requested \c Functionality is missing in description
  /// @result \c Duration (double, seconds): Total Time to complete all tasks.
  static util::Duration estimateDuration(const util::Pose &start_pose,
                                         const std::vector<FunctionalityVariant> &functionalities,
                                         const AmrDescription &description,
                                         const Topology &topology, bool check_positioning = true);

  /// @brief Estimates how much time (seconds) it takes to complete a list of \c
  /// FunctionalityVariant .
  /// @throws \c std::invalid_argument
  /// * if \c start_pose or any other Position is outside of \c Topology and check_positioning is
  /// true, or
  /// * if a requested \c FunctionalityVariant is missing in description
  /// @result \c Duration (double, seconds): Total Time to complete all tasks.
  static util::Duration estimateDuration(const util::Pose &start_pose,
                                         const FunctionalityVariant &functionality,
                                         const AmrDescription &description,
                                         const Topology &topology, bool check_positioning = true);

  /// @brief Calculates acceleration, constant velocity and deceleration phases for simulation. The
  /// deceleration vector is pointing in the opposite direction of the velocity vector.
  /// @throws \c std::invalid_argument
  /// * if \c start_pose or any other Position is outside of \c Topology or
  /// * if a requested \c FunctionalityVariant is missing in description
  /// @return List of statuses. Lists a status for every change in state (phase) or direction. The
  /// last status is always kIdle. Its timestamp contains the FunctionalityVariant's completion
  /// time.
  static std::vector<daisi::cpps::AmrMobilityStatus> calculatePhases(
      const util::Duration &start_timestamp, const util::Pose &start_pose,
      const FunctionalityVariant &functionality, const AmrDescription &description,
      const Topology &topology);

  /// @brief Calculates the current status of the AMR for simulation.
  /// @throws \c std::invalid_argument
  /// * if \c current_timestamp is smaller than \c current_phase.timestamp
  /// @details Calculates the current status of the AMR given an initial status.
  static daisi::cpps::AmrMobilityStatus calculateMobilityStatus(
      const daisi::cpps::AmrMobilityStatus &current_phase, const util::Duration &current_timestamp);

  /// @brief Calculates distance it takes to execute reach functionality starting from
  /// start_position
  /// @return Estimated distance in meters
  static daisi::util::Distance calculateDistance(
      const daisi::util::Position &start_position,
      const std::vector<FunctionalityVariant> &functionalities);

  /// @brief Calculates durations and distances different parts of the execution. The domains are
  /// empty travel (when the AMR carries no load), loaded, and action (when loading/unloading).
  /// @return A tuple of durations and distances in the following order: Empty time, loaded time,
  /// action time, empty distance, and loaded distance
  static std::tuple<daisi::util::Duration, daisi::util::Duration, daisi::util::Duration,
                    daisi::util::Distance, daisi::util::Distance>
  calculateMetricsByDomain(const daisi::util::Position &start_position,
                           const std::vector<FunctionalityVariant> &functionalities,
                           const AmrDescription &description, const Topology &topology);

private:
  /// @brief calculates durations (seconds) for acceleration, max_speed and deceleration phases.
  /// @return std::vector of length 3
  static std::vector<util::Duration> calculatePhaseDurations(const std::vector<double> &distances_m,
                                                             const AmrDescription &description);

  /// @brief calculates distances (meters) for acceleration, max_speed and deceleration phases.
  /// @return std::vector of length 3
  static std::vector<double> calculatePhaseDistances(const util::Position &start_position,
                                                     const FunctionalityVariant &functionality,
                                                     const AmrDescription &description);

  static std::vector<AmrMobilityStatus> calculatePhases(const util::Duration &start_timestamp,
                                                        const util::Position &start_position,
                                                        const util::Position &destination,
                                                        const std::vector<double> &distances,
                                                        const util::Duration &stationary_time,
                                                        const AmrDescription &description);

  static std::vector<AmrMobilityStatus> calculatePhases(const util::Duration &start_timestamp,
                                                        const util::Position &start_position,
                                                        const Navigate &navigate,
                                                        const AmrDescription &description);

  /// @{
  /// Calculate distance (meters)
  static daisi::util::Distance calculateDistance(const util::Position &start_position,
                                                 const FunctionalityVariant &functionality);

  static daisi::util::Distance calculateDistance(const util::Position &start_position,
                                                 const Navigate &functionality);
  /// @}

  static void arePositionsInTopology(const daisi::cpps::FunctionalityVariant &functionality,
                                     const Topology &topology);

  static void arePositionsInTopology(const std::vector<FunctionalityVariant> &functionalities,
                                     const Topology &topology);

  static bool isFunctionalityInDescription(const FunctionalityVariant &functionality,
                                           const AmrDescription &description);

  static void sanityCheck(const util::Pose &start_pose, const FunctionalityVariant &functionality,
                          const AmrDescription &description, const Topology &topology);
};

}  // namespace daisi::cpps
#endif
