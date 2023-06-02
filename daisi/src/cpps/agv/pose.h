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

#ifndef DAISI_CPPS_AGV_POSE_NS3_H_
#define DAISI_CPPS_AGV_POSE_NS3_H_

#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/vector.h"

namespace daisi::cpps {

class Pose {
public:
  ns3::Vector getPosition() const;
  void setPosition(const ns3::Vector &position);

  double getTheta() const;
  void setTheta(double value);

private:
  ns3::Vector base_position_ = ns3::Vector(0, 0, 0);
  double theta_ = 0;
  // x-y-theta
};

}  // namespace daisi::cpps
#endif
