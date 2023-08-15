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

#ifndef DAISI_CPPS_AMR_AMR_KINEMATICS_H_
#define DAISI_CPPS_AMR_AMR_KINEMATICS_H_

#include "solanet/serializer/serialize.h"

namespace daisi::cpps {
class AmrKinematics {
public:
  AmrKinematics() = default;
  AmrKinematics(double max_velocity_m_s, double min_velocity_m_s, double max_acceleration_m_s2,
                double max_deceleration_m_s2) {
    max_velocity_m_s_ = max_velocity_m_s;
    min_velocity_m_s_ = min_velocity_m_s;
    max_acceleration_m_s2_ = max_acceleration_m_s2;
    max_deceleration_m_s2_ = max_deceleration_m_s2;
  }

  /// @{
  /// Return velocity in m/s
  double getMaxVelocity() const { return max_velocity_m_s_; }
  double getMinVelocity() const { return min_velocity_m_s_; }
  /// @}

  /// @{
  /// Return acceleration in m/s^2
  double getMaxAcceleration() const { return max_acceleration_m_s2_; }
  double getMaxDeceleration() const { return max_deceleration_m_s2_; }
  /// @}

  SERIALIZE(max_acceleration_m_s2_, max_deceleration_m_s2_, max_velocity_m_s_, min_velocity_m_s_)

private:
  double max_velocity_m_s_ = 0.0;
  double min_velocity_m_s_ = 0.0;
  double max_acceleration_m_s2_ = 0.0;
  double max_deceleration_m_s2_ = 0.0;
};
}  // namespace daisi::cpps
#endif
