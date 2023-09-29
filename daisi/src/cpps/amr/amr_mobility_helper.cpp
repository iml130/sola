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

#include "cpps/amr/amr_mobility_helper.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <numeric>
#include <variant>

namespace daisi::cpps {

// helper variable for visitor pattern
template <class> inline constexpr bool kAlwaysFalseV = false;

util::Duration AmrMobilityHelper::estimateDuration(
    const util::Pose &start_pose, const std::vector<FunctionalityVariant> &functionalities,
    const AmrDescription &description, const Topology &topology, const bool check_positioning) {
  // estimate
  util::Position start = start_pose.position;
  util::Duration total_duration = 0;
  for (auto f : functionalities) {
    total_duration +=
        estimateDuration(util::Pose(start, 0), f, description, topology, check_positioning);
    if (auto move_to = std::get_if<MoveTo>(&f))
      start = move_to->destination;
    else if (auto navigate = std::get_if<Navigate>(&f))
      start = navigate->waypoints.back();
  }
  return total_duration;
}

util::Duration AmrMobilityHelper::estimateDuration(const util::Pose &start_pose,
                                                   const FunctionalityVariant &functionality,
                                                   const AmrDescription &description,
                                                   const Topology &topology,
                                                   const bool check_positioning) {
  // Sanity check
  sanityCheck(start_pose, functionality, description, topology);
  // estimation
  if (auto move_to = std::get_if<MoveTo>(&functionality)) {
    std::vector<double> distances =
        calculatePhaseDistances(start_pose.position, *move_to, description);
    std::vector<util::Duration> durations = calculatePhaseDurations(distances, description);
    util::Duration total_duration = std::accumulate(durations.begin(), durations.end(), 0.0);
    return total_duration;
  }
  if (auto navigate = std::get_if<Navigate>(&functionality)) {
    std::vector<double> distances =
        calculatePhaseDistances(start_pose.position, *navigate, description);
    std::vector<util::Duration> durations = calculatePhaseDurations(distances, description);
    util::Duration total_duration = std::accumulate(durations.begin(), durations.end(), 0.0);
    return total_duration;
  }
  if (auto load = std::get_if<Load>(&functionality)) {
    if (check_positioning && calculateDistance(start_pose.position, *load) != 0)
      throw std::invalid_argument("start_pose is invalid for destination of Load");
    return description.getLoadHandling().getLoadTime();
  }
  if (auto unload = std::get_if<Unload>(&functionality)) {
    if (check_positioning && calculateDistance(start_pose.position, *unload) != 0)
      throw std::invalid_argument("start_pose is invalid for destination of Unload");
    return description.getLoadHandling().getUnloadTime();
  }
  return -1;
}

std::vector<daisi::cpps::AmrMobilityStatus> AmrMobilityHelper::calculatePhases(
    const util::Duration &start_timestamp, const util::Pose &start_pose,
    const FunctionalityVariant &functionality, const AmrDescription &description,
    const Topology &topology) {
  // Sanity check
  sanityCheck(start_pose, functionality, description, topology);
  // estimation
  if (auto load = std::get_if<Load>(&functionality)) {
    return calculatePhases(start_timestamp, start_pose.position, load->destination, {0, 0, 0},
                           description.getLoadHandling().getLoadTime(), description);
  }
  if (auto move_to = std::get_if<MoveTo>(&functionality)) {
    auto distances = calculatePhaseDistances(start_pose.position, functionality, description);
    return calculatePhases(start_timestamp, start_pose.position, move_to->destination, distances, 0,
                           description);
  }
  if (auto unload = std::get_if<Unload>(&functionality)) {
    return calculatePhases(start_timestamp, start_pose.position, unload->destination, {0, 0, 0},
                           description.getLoadHandling().getUnloadTime(), description);
  }
  if (auto navigate = std::get_if<Navigate>(&functionality)) {
    return calculatePhases(start_timestamp, start_pose.position, *navigate, description);
  }
  return {};
}

AmrMobilityStatus AmrMobilityHelper::calculateMobilityStatus(
    const AmrMobilityStatus &current_phase, const util::Duration &current_timestamp) {
  AmrMobilityStatus status;
  util::Duration delta_t = current_timestamp - current_phase.timestamp;
  if (delta_t < -std::numeric_limits<float>::epsilon())
    throw std::invalid_argument("current_timestamp is before phase timestamp");
  switch (current_phase.state) {
    case AmrMobilityState::kAccelerating:
      status = current_phase;
      status.position +=
          status.velocity * delta_t + 1 / 2.0 * status.acceleration * delta_t * delta_t;
      status.velocity += status.acceleration * delta_t;
      break;
    case AmrMobilityState::kConstSpeedTraveling:
      status = current_phase;
      status.position += status.velocity * delta_t;
      break;
    case AmrMobilityState::kDecelerating:
      status = current_phase;
      // status.acceleration is pointing in the opposite direction of status.velocity during
      // deceleration
      status.position +=
          status.velocity * delta_t + 1 / 2.0 * status.acceleration * delta_t * delta_t;
      status.velocity += status.acceleration * delta_t;
      break;
    default:
      status = current_phase;
  }
  status.timestamp = current_timestamp;
  return status;
}

void AmrMobilityHelper::arePositionsInTopology(const FunctionalityVariant &functionality,
                                               const Topology &topology) {
  if (auto load = std::get_if<Load>(&functionality)) {
    util::Position p = load->destination;
    if (!topology.isPositionInTopology(p))
      throw std::invalid_argument("destination of Deliver is invalid");
    return;
  }
  if (auto move_to = std::get_if<MoveTo>(&functionality)) {
    util::Position p = move_to->destination;
    if (!topology.isPositionInTopology(p))
      throw std::invalid_argument("destination of MoveTo is invalid");
    return;
  }
  if (auto unload = std::get_if<Unload>(&functionality)) {
    util::Position p = unload->destination;
    if (!topology.isPositionInTopology(p))
      throw std::invalid_argument("destination of PickUp is invalid");
    return;
  }
  if (auto navigate = std::get_if<Navigate>(&functionality)) {
    std::vector<util::Position> wpts = navigate->waypoints;
    if (std::any_of(wpts.begin(), wpts.end(), [topology](const util::Position &p) {
          return !topology.isPositionInTopology(p);
        }))
      throw std::invalid_argument("waypoint of Navigate is invalid");
    return;
  }
}

void AmrMobilityHelper::arePositionsInTopology(
    const std::vector<FunctionalityVariant> &functionalities, const Topology &topology) {
  for (auto f : functionalities)
    arePositionsInTopology(f, topology);
}

daisi::util::Distance AmrMobilityHelper::calculateDistance(
    const daisi::util::Position &start_position, const FunctionalityVariant &functionality) {
  if (auto load = std::get_if<Load>(&functionality))
    return (start_position - load->destination).GetLength();
  if (auto move_to = std::get_if<MoveTo>(&functionality))
    return (start_position - move_to->destination).GetLength();
  if (auto unload = std::get_if<Unload>(&functionality))
    return (start_position - unload->destination).GetLength();
  if (auto navigate = std::get_if<Navigate>(&functionality))
    return calculateDistance(start_position, *navigate);
  return -1;
}

daisi::util::Distance AmrMobilityHelper::calculateDistance(const util::Position &start_position,
                                                           const Navigate &functionality) {
  daisi::util::Distance d = 0;
  util::Position p_last = start_position;
  for (const util::Position &p : functionality.waypoints) {
    d += (p - p_last).GetLength();
    p_last = p;
  }
  return d;
}

std::vector<util::Duration> AmrMobilityHelper::calculatePhaseDurations(
    const std::vector<double> &distances_m, const AmrDescription &description) {
  util::Duration duration_acc = 0;
  util::Duration duration_const = 0;
  util::Duration duration_dec = 0;
  assert(distances_m.size() >= 3);
  double distance_acc = distances_m[0];
  double distance_const = distances_m[1];
  double distance_dec = distances_m[2];
  double max_vel = description.getKinematics().getMaxVelocity();
  double max_dec = std::abs(description.getKinematics().getMaxDeceleration());
  double max_acc = description.getKinematics().getMaxAcceleration();

  if (distance_const == 0) {
    duration_acc = sqrt(2 * distance_acc / max_acc);
    duration_const = 0;
    duration_dec = sqrt(2 * distance_dec / max_dec);
  } else {
    duration_acc = max_vel / max_acc;
    duration_const = distance_const / max_vel;
    duration_dec = max_vel / max_dec;
  }
  return {duration_acc, duration_const, duration_dec};
}

std::vector<double> AmrMobilityHelper::calculatePhaseDistances(
    const util::Position &start_position, const FunctionalityVariant &functionality,
    const AmrDescription &description) {
  double distance_m = calculateDistance(start_position, functionality);
  double max_vel = description.getKinematics().getMaxVelocity();
  double max_dec = std::abs(description.getKinematics().getMaxDeceleration());
  double max_acc = description.getKinematics().getMaxAcceleration();

  double distance_acc = (max_vel * max_vel) / (2 * max_acc);
  double distance_const = 0.0;
  double distance_dec = (max_vel * max_vel) / (2 * max_dec);
  double distance_threshold = (distance_dec + distance_acc);
  if (distance_m < distance_threshold) {
    distance_acc = distance_m * max_dec / (max_acc + max_dec);
    distance_const = 0;
    distance_dec = distance_m * max_acc / (max_acc + max_dec);
  } else {
    distance_const = distance_m - distance_acc - distance_dec;
  }
  return {distance_acc, distance_const, distance_dec};
}

std::vector<AmrMobilityStatus> AmrMobilityHelper::calculatePhases(
    const util::Duration &start_timestamp, const util::Position &start_position,
    const util::Position &destination, const std::vector<double> &distances,
    const util::Duration &stationary_time, const AmrDescription &description) {
  const std::vector<util::Duration> durations = calculatePhaseDurations(distances, description);
  const util::Position translation = destination - start_position;
  util::Position next_position = start_position;
  util::Duration next_timestamp = start_timestamp;
  double next_velocity_scalar = 0;
  std::vector<AmrMobilityStatus> ret;
  // acceleration
  if (durations[0] > 0) {
    assert(translation.GetLength() != 0);
    AmrMobilityStatus acceleration;
    acceleration.state = AmrMobilityState::kAccelerating;
    acceleration.velocity = {0, 0};
    acceleration.acceleration =
        translation / translation.GetLength() * description.getKinematics().getMaxAcceleration();
    acceleration.position = next_position;
    acceleration.timestamp = next_timestamp;
    ret.push_back(acceleration);
    next_timestamp += durations[0];
    next_position += translation / translation.GetLength() * distances[0];
    next_velocity_scalar = durations[0] * description.getKinematics().getMaxAcceleration();
  }
  // constant speed
  if (durations[1] > 0) {
    assert(translation.GetLength() != 0);
    AmrMobilityStatus const_velocity;
    const_velocity.state = AmrMobilityState::kConstSpeedTraveling;
    const_velocity.velocity =
        translation / translation.GetLength() * description.getKinematics().getMaxVelocity();
    const_velocity.acceleration = {0, 0};
    const_velocity.position = next_position;
    const_velocity.timestamp = next_timestamp;
    ret.push_back(const_velocity);
    next_timestamp += durations[1];
    next_position += translation / translation.GetLength() * distances[1];
    next_velocity_scalar = description.getKinematics().getMaxVelocity();
  }
  // deceleration
  if (durations[2] > 0) {
    assert(translation.GetLength() != 0);
    AmrMobilityStatus deceleration;
    deceleration.state = AmrMobilityState::kDecelerating;
    deceleration.velocity = translation / translation.GetLength() * next_velocity_scalar;
    deceleration.acceleration =
        -translation / translation.GetLength() * description.getKinematics().getMaxDeceleration();
    deceleration.position = next_position;
    deceleration.timestamp = next_timestamp;
    ret.push_back(deceleration);
    next_position += translation / translation.GetLength() * distances[2];
    next_timestamp += durations[2];
  }
  // stationary
  if (stationary_time > 0) {
    AmrMobilityStatus stationary;
    stationary.state = AmrMobilityState::kStationary;
    stationary.velocity = {0, 0};
    stationary.acceleration = {0, 0};
    stationary.position = destination;
    stationary.timestamp = next_timestamp;
    ret.push_back(stationary);
    next_timestamp += stationary_time;
  }
  // idle
  AmrMobilityStatus idle;
  idle.state = AmrMobilityState::kIdle;
  idle.velocity = {0, 0};
  idle.acceleration = {0, 0};
  idle.position = destination;
  idle.timestamp = next_timestamp;
  ret.push_back(idle);
  return ret;
}

std::vector<AmrMobilityStatus> AmrMobilityHelper::calculatePhases(
    const util::Duration &start_timestamp, const util::Position &start_position,
    const Navigate &navigate, const AmrDescription &description) {
  const std::vector<double> distances =
      calculatePhaseDistances(start_position, navigate, description);
  const std::vector<util::Duration> durations = calculatePhaseDurations(distances, description);
  util::Position translation = navigate.waypoints[0] - start_position;
  double max_acc = description.getKinematics().getMaxAcceleration();
  double max_dec = description.getKinematics().getMaxDeceleration();
  double max_vel = description.getKinematics().getMaxVelocity();
  std::vector<AmrMobilityStatus> ret;
  double cumulative_distance = 0;
  util::Duration cumulative_time = 0;
  util::Position wp = start_position;
  util::Position last_wp;
  util::Velocity last_vel = {0, 0};
  std::vector<AmrMobilityStatus>::size_type i = 0;
  for (; i != navigate.waypoints.size(); i++) {
    last_wp = wp;
    wp = navigate.waypoints[i];
    translation = wp - last_wp;
    cumulative_distance += translation.GetLength();
    if (translation.GetLength() == 0) continue;
    // acceleration
    if (cumulative_distance < distances[0]) {
      AmrMobilityStatus status;
      status.state = AmrMobilityState::kAccelerating;
      status.timestamp = start_timestamp + cumulative_time;
      status.position = last_wp;
      util::Acceleration direction = translation / translation.GetLength();
      status.acceleration = direction * max_acc;
      util::Duration time = -last_vel.GetLength() / max_acc +
                            sqrt(last_vel.GetLength() * last_vel.GetLength() / max_acc / max_acc +
                                 2 * translation.GetLength() / max_acc);
      status.velocity = direction * (last_vel.GetLength() + max_acc * time);
      ret.push_back(status);
      last_vel = status.velocity;
      cumulative_time += time;
    } else
      break;
  }
  // acceleration to max speed
  if (translation.GetLength() > 0 && durations[0] > 0) {
    AmrMobilityStatus acceleration;
    acceleration.state = AmrMobilityState::kAccelerating;
    acceleration.velocity = last_vel.GetLength() * translation / translation.GetLength();
    acceleration.acceleration = translation / translation.GetLength() * max_acc;
    acceleration.position = last_wp;
    acceleration.timestamp = start_timestamp + cumulative_time;
    ret.push_back(acceleration);
    wp = last_wp + translation / translation.GetLength() *
                       (distances[0] + translation.GetLength() - cumulative_distance);
    cumulative_time = durations[0];
    cumulative_distance = distances[0];
    last_vel = translation / translation.GetLength() * (max_acc * durations[0]);
  }
  // constant velocity
  for (; i != navigate.waypoints.size(); i++) {
    last_wp = wp;
    wp = navigate.waypoints[i];
    translation = wp - last_wp;
    cumulative_distance += translation.GetLength();
    if (translation.GetLength() == 0) continue;
    if (cumulative_distance < distances[0] + distances[1]) {
      AmrMobilityStatus status;
      status.state = AmrMobilityState::kConstSpeedTraveling;
      status.timestamp = start_timestamp + cumulative_time;
      status.position = last_wp;
      util::Acceleration direction = translation / translation.GetLength();
      status.acceleration = {0, 0};
      util::Duration time = translation.GetLength() / max_vel;
      status.velocity = direction * max_vel;
      ret.push_back(status);
      cumulative_time += time;
    } else
      break;
  }
  // constant velocity to deceleration
  if (translation.GetLength() > 0 && durations[1] > 0) {
    AmrMobilityStatus const_velocity;
    const_velocity.state = AmrMobilityState::kConstSpeedTraveling;
    const_velocity.velocity = translation / translation.GetLength() * max_vel;
    const_velocity.acceleration = {0, 0};
    const_velocity.position = last_wp;
    const_velocity.timestamp = start_timestamp + cumulative_time;
    ret.push_back(const_velocity);
    wp =
        last_wp + translation / translation.GetLength() *
                      (distances[0] + distances[1] + translation.GetLength() - cumulative_distance);
    cumulative_time = durations[0] + durations[1];
    cumulative_distance = distances[0] + distances[1];
    last_vel = const_velocity.velocity;
  } else {
    wp -= translation;
    cumulative_distance -= translation.GetLength();
  }
  // deceleration
  for (; i != navigate.waypoints.size(); i++) {
    last_wp = wp;
    wp = navigate.waypoints[i];
    translation = wp - last_wp;
    cumulative_distance += translation.GetLength();
    if (translation.GetLength() == 0) continue;
    if (cumulative_distance > distances[0] + distances[1]) {
      AmrMobilityStatus status;
      status.state = AmrMobilityState::kDecelerating;
      status.timestamp = start_timestamp + cumulative_time;
      status.position = last_wp;
      util::Acceleration direction = translation / translation.GetLength();
      status.acceleration = -direction * max_dec;
      util::Duration time = last_vel.GetLength() / max_dec -
                            sqrt(last_vel.GetLength() * last_vel.GetLength() / max_dec / max_dec -
                                 2 * translation.GetLength() / max_dec);
      status.velocity = direction * last_vel.GetLength();
      last_vel = direction * (last_vel.GetLength() - max_dec * time);
      ret.push_back(status);
      cumulative_time += time;
    }
  }
  // idle
  AmrMobilityStatus idle;
  idle.state = AmrMobilityState::kIdle;
  idle.velocity = {0, 0};
  idle.acceleration = {0, 0};
  idle.position = wp;
  idle.timestamp = start_timestamp + durations[0] + durations[1] + durations[2];

  ret.push_back(idle);
  return ret;
}

bool AmrMobilityHelper::isFunctionalityInDescription(const FunctionalityVariant &functionality,
                                                     const AmrDescription &description) {
  if (std::holds_alternative<Load>(functionality))
    return description.getProperties().getFunctionalities().count(FunctionalityType::kLoad) > 0;
  if (std::holds_alternative<MoveTo>(functionality))
    return description.getProperties().getFunctionalities().count(FunctionalityType::kMoveTo) > 0;
  if (std::holds_alternative<Unload>(functionality))
    return description.getProperties().getFunctionalities().count(FunctionalityType::kUnload) > 0;
  if (std::holds_alternative<Navigate>(functionality))
    return description.getProperties().getFunctionalities().count(FunctionalityType::kNavigate) > 0;
  return false;
}

void AmrMobilityHelper::sanityCheck(const util::Pose &start_pose,
                                    const FunctionalityVariant &functionality,
                                    const AmrDescription &description, const Topology &topology) {
  util::Position start_position(start_pose.position);
  if (!isFunctionalityInDescription(functionality, description))
    throw std::invalid_argument("functionality is invalid");
  if (!topology.isPositionInTopology(start_position))
    throw std::invalid_argument("start_pose is invalid");
  arePositionsInTopology(functionality, topology);
}

daisi::util::Distance AmrMobilityHelper::calculateDistance(
    const daisi::util::Position &start_position,
    const std::vector<FunctionalityVariant> &functionalities) {
  daisi::util::Distance distance = 0;
  daisi::util::Position last_pos = start_position;

  for (const auto &functionality : functionalities) {
    distance += calculateDistance(last_pos, functionality);

    // calc end position
    if (auto move_to_pval = std::get_if<MoveTo>(&functionality)) {
      last_pos = move_to_pval->destination;
    } else if (auto navigate_pval = std::get_if<Navigate>(&functionality)) {
      last_pos = navigate_pval->waypoints.back();
    }
  }

  return distance;
}

std::tuple<daisi::util::Duration, daisi::util::Duration, daisi::util::Duration,
           daisi::util::Distance, daisi::util::Distance>
AmrMobilityHelper::calculateMetricsByDomain(
    const daisi::util::Position &start_position,
    const std::vector<FunctionalityVariant> &functionalities, const AmrDescription &description,
    const Topology &topology) {
  daisi::util::Duration empty_time = 0;
  daisi::util::Duration loaded_time = 0;
  daisi::util::Duration action_time = 0;

  daisi::util::Distance empty_distance = 0;
  daisi::util::Distance loaded_distance = 0;

  bool loaded = false;
  daisi::util::Position last_pos = start_position;

  for (auto const &functionality : functionalities) {
    std::visit(
        [&](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;

          if constexpr (std::is_same_v<T, MoveTo>) {
            daisi::util::Distance dist = calculateDistance(last_pos, functionality);
            daisi::util::Duration time =
                estimateDuration(util::Pose(last_pos), functionality, description, topology, false);

            if (loaded) {
              loaded_distance += dist;
              loaded_time += time;
            } else {
              empty_distance += dist;
              empty_time += time;
            }

            last_pos = std::get<MoveTo>(functionality).destination;

          } else if constexpr (std::is_same_v<T, Load>) {
            loaded = true;
            action_time +=
                estimateDuration(util::Pose(last_pos), functionality, description, topology, false);

          } else if constexpr (std::is_same_v<T, Unload>) {
            loaded = false;
            action_time +=
                estimateDuration(util::Pose(last_pos), functionality, description, topology, false);

          } else if constexpr (std::is_same_v<T, Navigate>) {
            throw std::invalid_argument("Functionality Navigate not implemented yet");
          } else if constexpr (std::is_same_v<T, std::monostate>) {
          } else {
            static_assert(kAlwaysFalseV<T>, "Functionality not handled");
          }
        },
        functionality);
  }

  return {empty_time, loaded_time, action_time, empty_distance, loaded_distance};
}

}  // namespace daisi::cpps
