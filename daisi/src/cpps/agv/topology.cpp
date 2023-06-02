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

#include "cpps/agv/topology.h"

#include "ns3/log.h"

namespace daisi::cpps {

TopologyNs3::TopologyNs3(const ns3::Vector &size) : size_(size) {
  if (size.x <= 0 || size.y <= 0) throw std::invalid_argument("Topology dimensions need to > 0");
}

ns3::Vector TopologyNs3::getSize() const { return size_; }

bool TopologyNs3::isValid() const { return size_.x > 0 && size_.y > 0; }

double TopologyNs3::getWidth() const { return size_.x; }

double TopologyNs3::getHeight() const { return size_.y; }

bool TopologyNs3::isPointInToplogy(const ns3::Vector &vector) const {
  if (vector.x < 0 || vector.y < 0 || vector.z < 0)
    throw std::invalid_argument("Vector is negativ, should not be here the case ;)");

  if (vector.x > size_.x || vector.y > size_.x || vector.z > size_.x)
    throw std::invalid_argument("Vector is out of bounds");
  //    return false;

  return true;
}
};  // namespace daisi::cpps
