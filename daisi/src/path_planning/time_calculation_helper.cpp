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

#include "time_calculation_helper.h"

#include <math.h>

#include <functional>

#include "utils/daisi_check.h"

namespace daisi::path_planning {

using DistanceToTimeFct = std::function<double(double dist)>;
enum MobilityStates { kNotMoving, kAcceleration, kDriving, kDeceleration, kTotalTimeEstimation };

struct CalcVars {
  float time_acc = 0;
  float time_const = 0;
  float time_decel = 0;
  float dist_acc = 0;
  float dist_const = 0;
  float dist_decel = 0;
  float max_acc = 0;
  float min_acc = 0;
  float real_max_velo = 0;
};

// Ref #61
static constexpr double kEpsilon = 0.01;

static void verifyCalculation(ns3::Vector2D start, ns3::Vector2D stop, double max_velo,
                              CalcVars calc_vars) {
  {
    DAISI_CHECK(
        calc_vars.real_max_velo != max_velo ? calc_vars.time_const == 0 : calc_vars.time_const != 0,
        "only when driving some time with constant speed, reach max velo");
    DAISI_CHECK(calc_vars.real_max_velo <= max_velo, "Real velocity cannot exceed max velocity");

    // Sometimes very small double precision error (use type with fixed decimal part in future?)
    // Only checking for delta
    double dist_x = ns3::CalculateDistance(start, stop);
    double sum = calc_vars.dist_acc + calc_vars.dist_const + calc_vars.dist_decel;
    DAISI_CHECK(std::abs(dist_x - sum) < kEpsilon,
                "Actual distance and calculated distance differ.");

    if (calc_vars.max_acc == calc_vars.min_acc) {
      DAISI_CHECK(calc_vars.time_acc == calc_vars.time_decel,
                  "Acceleration and deceleration times must be equal");
      DAISI_CHECK(calc_vars.dist_acc == calc_vars.dist_decel,
                  "Acceleration and deceleration distances must be equal");
    }
  }
}

static void verifyFunction(ns3::Vector2D start, ns3::Vector2D stop, CalcVars calc_vars,
                           const DistanceToTimeFct &fct) {
  double dist = ns3::CalculateDistance(start, stop);
  double calctime = fct(dist);
  double total_time = calc_vars.time_acc + calc_vars.time_const + calc_vars.time_decel;
  DAISI_CHECK(std::abs(calctime - total_time) < kEpsilon,
              "Calculated and expected total time differs");
}

static double estimatesCostsInTime(const ns3::Vector &start, const ns3::Vector &stop,
                                   MobilityStates state, const cpps::AmrKinematics &kinematics) {
  double max_velocity = kinematics.getMaxVelocity();
  double max_acceleration = kinematics.getMaxAcceleration();
  double min_acceleration = kinematics.getMaxDeceleration();

  double distance_acc;
  double distance_brake;
  double d_const;

  // calculate distance threshold
  distance_acc = (max_velocity * max_velocity) / (2 * (max_acceleration));
  distance_brake = (max_velocity * max_velocity) / (2 * std::abs(min_acceleration));
  double distance_threshold = (distance_brake + distance_acc);
  double temp_time = 0;
  double distance = ns3::CalculateDistance(start, stop);

  switch (state) {
    case kAcceleration:

      if (distance <= distance_threshold) {
        temp_time =
            sqrt((2 * distance / max_acceleration) *
                 (std::abs(min_acceleration) / (max_acceleration + std::abs(min_acceleration))));
      } else {
        temp_time = max_velocity / max_acceleration;
      }
      break;
    case kDeceleration:
      if (distance <= distance_threshold) {
        temp_time =
            sqrt((2 * distance / max_acceleration) *
                 (std::abs(max_acceleration) / (max_acceleration + std::abs(min_acceleration))));
      } else {
        temp_time = max_velocity / std::abs(min_acceleration);
      }
      break;
    case kDriving:
      if (distance <= distance_threshold) return 0;
      distance_acc = (max_velocity * max_velocity) / (2 * (max_acceleration));
      distance_brake = (max_velocity * max_velocity) / (2 * std::abs(min_acceleration));
      d_const = distance - distance_acc - distance_brake;
      temp_time = d_const / max_velocity;
      break;
    default:
      break;
  }
  return temp_time;
}

static CalcVars calculateVariables(const cpps::AmrKinematics &kinematics, ns3::Vector2D start,
                                   ns3::Vector2D stop) {
  using namespace ns3;

  double time_acc = estimatesCostsInTime(Vector3D(start.x, start.y, 0), Vector3D(stop.x, stop.y, 0),
                                         kAcceleration, kinematics);
  double time_const = estimatesCostsInTime(Vector3D(start.x, start.y, 0),
                                           Vector3D(stop.x, stop.y, 0), kDriving, kinematics);
  double time_decel = estimatesCostsInTime(Vector3D(start.x, start.y, 0),
                                           Vector3D(stop.x, stop.y, 0), kDeceleration, kinematics);

  double min_acc = std::abs(kinematics.getMaxDeceleration());
  double max_acc = kinematics.getMaxAcceleration();
  double max_velo = kinematics.getMaxVelocity();

  double dist_acc = (max_acc * time_acc * time_acc) / 2.0;  // 1/2 * a * t^2 + 0 (initially)
  double real_max_velo = max_acc * time_acc;                // a * t + 0 (initially)

  double dist_const = real_max_velo * time_const;
  double dist_decel = (min_acc * time_decel * time_decel) / -2.0 +
                      real_max_velo * time_decel;  // - 1/2 * a * t^2 + v_0 * t + x_0 (decelerating
                                                   // from real_max_velo and not adding any x_0
                                                   // offset (ony using total distance while decel)

  CalcVars calc_vars;
  calc_vars.time_acc = static_cast<float>(time_acc);
  calc_vars.time_decel = static_cast<float>(time_decel);
  calc_vars.time_const = static_cast<float>(time_const);
  calc_vars.min_acc = static_cast<float>(min_acc);
  calc_vars.max_acc = static_cast<float>(max_acc);
  calc_vars.dist_acc = static_cast<float>(dist_acc);
  calc_vars.dist_const = static_cast<float>(dist_const);
  calc_vars.dist_decel = static_cast<float>(dist_decel);
  calc_vars.real_max_velo = static_cast<float>(real_max_velo);

  verifyCalculation(start, stop, max_velo, calc_vars);

  return calc_vars;
}

/**
 * Calculate function which takes an intermediate distance from start to stop and returns the time
 * when this distance is reached
 * @param kinematics
 * @param start
 * @param stop
 * @return
 */
static DistanceToTimeFct calculateDistanceToTimeFct(const cpps::AmrKinematics &kinematics,
                                                    ns3::Vector2D start, ns3::Vector2D stop) {
  CalcVars calc_vars = calculateVariables(kinematics, start, stop);

  auto function_dist_to_time = [calc_vars =
                                    std::as_const(calc_vars)](const float distance) -> float {
    double time;
    if (distance <= calc_vars.dist_acc) {
      // Distance in acceleration phase
      // x = 1/2 * a * t^2
      // ==> t = sqrt(2x/a)
      time = sqrt(2 * distance / calc_vars.max_acc);
    } else if (distance <= calc_vars.dist_acc + calc_vars.dist_const) {
      // Distance in acceleration phase
      time = calc_vars.time_acc;
      const double const_speed_dist = distance - calc_vars.dist_acc;
      DAISI_CHECK(const_speed_dist > 0, "Should drive constant but with no distance");
      // x = v * t
      // ==> x / v = t
      time += const_speed_dist / calc_vars.real_max_velo;
    } else if (distance <=
               calc_vars.dist_acc + calc_vars.dist_const + calc_vars.dist_decel + kEpsilon) {
      // Distance in deceleration phase
      time = calc_vars.time_acc + calc_vars.time_const;
      double decel_speed_dist = distance - (calc_vars.dist_acc + calc_vars.dist_const);
      DAISI_CHECK(decel_speed_dist > 0, "Should decelerate but with no distance");
      if (decel_speed_dist > calc_vars.dist_decel &&
          decel_speed_dist <= calc_vars.dist_decel + kEpsilon) {
        decel_speed_dist = calc_vars.dist_decel;
      }
      DAISI_CHECK(decel_speed_dist <= calc_vars.dist_decel, "");

      // x = - 1 / 2 * a * t^2 + v_0 * t + x_0 // v_0 is real_max_velo, x_0 not relevant as already
      // subtracted above 0 = - 1 / 2 * a * t^2 + v_0 * t - x
      // => t = v_0 / a - sqrt((v_0^2 - 2 * x * a) / a^2)
      double rad = (pow(calc_vars.real_max_velo, 2) - 2 * decel_speed_dist * calc_vars.min_acc) /
                   pow(calc_vars.min_acc, 2);
      if (rad < 0) {
        // Capture possible precision error
        DAISI_CHECK(rad > -kEpsilon, "");
        rad = 0;
      }

      time += (calc_vars.real_max_velo / calc_vars.min_acc) - sqrt(rad);
    } else {
      throw std::runtime_error("distance greater than total route distance");
    }
    return time;
  };

  verifyFunction(start, stop, calc_vars, function_dist_to_time);

  return function_dist_to_time;
}

PointTimePairs calculateTimeTillPoints(const cpps::AmrKinematics &kinematics,
                                       const cpps::AmrLoadHandlingUnit &load_handling,
                                       ns3::Vector2D start, ns3::Vector2D stop,
                                       const std::vector<ns3::Vector2D> &points) {
  DistanceToTimeFct fct = calculateDistanceToTimeFct(kinematics, start, stop);

  PointTimePairs pairs;
  pairs.reserve(points.size());
  for (auto point : points) {
    double distance = ns3::CalculateDistance(start, point);
    double time = fct(distance);
    pairs.push_back({point, time});
  }

  return pairs;
}
}  // namespace daisi::path_planning
