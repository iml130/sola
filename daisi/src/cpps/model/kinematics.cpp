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

#include "cpps/model/kinematics.h"

#include <cassert>

namespace daisi::cpps {

Kinematics::Kinematics(double max_velo, double min_velo, double max_acc, double min_acc,
                       double load_time, double unload_time) {
  max_acceleration_ = max_acc;
  min_acceleration_ = min_acc;
  max_velocity_ = max_velo;
  min_velocity_ = min_velo;
  load_time_ = load_time;
  unload_time_ = unload_time;
}

Kinematics::Kinematics(double max_velo, double min_velo, double max_acc, double min_acc) {
  max_acceleration_ = max_acc;
  min_acceleration_ = min_acc;
  max_velocity_ = max_velo;
  min_velocity_ = min_velo;
}

void Kinematics::set(const Kinematics &kinematics) {
  max_acceleration_ = kinematics.max_acceleration_;
  min_acceleration_ = kinematics.min_acceleration_;
  max_velocity_ = kinematics.max_velocity_;
  min_velocity_ = kinematics.min_velocity_;
  load_time_ = kinematics.load_time_;
  unload_time_ = kinematics.unload_time_;
}

double Kinematics::getMaxVelocity() const { return max_velocity_; }
double Kinematics::getMaxAcceleration() const { return max_acceleration_; }
double Kinematics::getMinVelocity() const { return min_velocity_; }
double Kinematics::getMinAcceleration() const { return min_acceleration_; }

double Kinematics::getLoadTime() const { return load_time_; }
double Kinematics::getUnloadTime() const { return unload_time_; }

std::pair<double, double> Kinematics::getStartStopTimeAndDistance(const ns3::Vector &start,
                                                                  const ns3::Vector &stop,
                                                                  const bool execution) const {
  assert(max_velocity_ > 0);
  assert(max_acceleration_ > 0);

  double distance_acc;
  double distance_brake;
  double time = 0;
  double distance = CalculateDistance(start, stop);

  distance_acc = (getMaxVelocity() * getMaxVelocity()) / (2 * (getMaxAcceleration()));
  distance_brake = (getMaxVelocity() * getMaxVelocity()) / (2 * std::abs(getMinAcceleration()));
  double distance_threshold = (distance_brake + distance_acc);

  // ACCELERATION:
  if (distance <= distance_threshold) {
    time += sqrt(
        (2 * distance / getMaxAcceleration()) *
        (std::abs(getMinAcceleration()) / (getMaxAcceleration() + std::abs(getMinAcceleration()))));
  } else {
    time += getMaxVelocity() / getMaxAcceleration();
  }

  // DECELERATION:
  if (distance <= distance_threshold) {
    time += sqrt(
        (2 * distance / getMaxAcceleration()) *
        (std::abs(getMaxAcceleration()) / (getMaxAcceleration() + std::abs(getMinAcceleration()))));
  } else {
    time += getMaxVelocity() / std::abs(getMinAcceleration());
  }

  // Driving/constant
  if (distance > distance_threshold) {
    distance_acc = (getMaxVelocity() * getMaxVelocity()) / (2 * (getMaxAcceleration()));
    distance_brake = (getMaxVelocity() * getMaxVelocity()) / (2 * std::abs(getMinAcceleration()));
    double d_const = distance - distance_acc - distance_brake;
    time += d_const / getMaxVelocity();
  }

  time *= 1000;  // in ms
  if (execution) {
    time += getLoadTime();
    time += getUnloadTime();
  }

  return {time, distance};
}

}  // namespace daisi::cpps
