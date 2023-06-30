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

#include "cpps/amr/amr_topology.h"

namespace daisi::cpps {

Topology::Topology(const util::Dimensions &size) : size_(size) {
  if (size.x <= 0 || size.y <= 0) throw std::invalid_argument("Topology dimensions need to > 0");
}

util::Dimensions Topology::getSize() const { return size_; }

bool Topology::isValid() const { return size_.x > 0 && size_.y > 0; }

double Topology::getWidth() const { return size_.x; }

double Topology::getHeight() const { return size_.y; }

bool Topology::isPositionInTopology(const util::Position &vector) const {
  if (vector.x < 0 || vector.y < 0) return false;
  if (vector.x > size_.x || vector.y > size_.y) return false;
  return true;
}

bool Topology::isPointInTopology(const util::Dimensions &vector) const {
  if (vector.x < 0 || vector.y < 0 || vector.z < 0)
    throw std::invalid_argument("Vector is negativ, should not be here the case ;)");

  if (vector.x > size_.x || vector.y > size_.y || vector.z > size_.z)
    throw std::invalid_argument("Vector is out of bounds");

  return true;
}
}  // namespace daisi::cpps
