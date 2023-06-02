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

#ifndef DAISI_MODEL_KINEMATICS_NS3_H_
#define DAISI_MODEL_KINEMATICS_NS3_H_

#include "cpps/message/serialize.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/vector.h"

namespace daisi::cpps {

class Kinematics {
public:
  Kinematics() = default;
  Kinematics(double max_velo, double min_velo, double max_acc, double min_acc, double load_time,
             double unload_time);
  Kinematics(double max_velo, double min_velo, double max_acc, double min_acc);

  Kinematics(const Kinematics &) = default;
  ~Kinematics() = default;

  void set(const Kinematics &kinematics);

  double getMaxVelocity() const;
  double getMaxAcceleration() const;
  double getMinVelocity() const;
  double getMinAcceleration() const;

  double getLoadTime() const;
  double getUnloadTime() const;

  std::pair<double, double> getStartStopTimeAndDistance(const ns3::Vector &start,
                                                        const ns3::Vector &stop,
                                                        bool execution = false) const;

  SERIALIZE(max_velocity_, min_velocity_, max_acceleration_, min_acceleration_, load_time_,
            unload_time_);

private:
  // these values are randomly chosen
  double max_velocity_ = 10.0;      // m/s
  double min_velocity_ = 0.0;       // m/s
  double max_acceleration_ = 2.0;   // m/(s^2)
  double min_acceleration_ = -2.0;  // m/(s^2)

  double load_time_ = 0.0;
  double unload_time_ = 0.0;
};
}  // namespace daisi::cpps
#endif
