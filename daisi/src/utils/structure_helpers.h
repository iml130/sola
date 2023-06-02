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

#ifndef DAISI_UTILS_STRUCTURE_HELPERS_H_
#define DAISI_UTILS_STRUCTURE_HELPERS_H_

#include <ns3/vector.h>

#include "solanet/serializer/serialize.h"

template <class Archive> inline void serialize(Archive &archive, ns3::Vector2D &m) {
  archive(m.x, m.y);
}

template <class Archive> inline void serialize(Archive &archive, ns3::Vector3D &m) {
  archive(m.x, m.y);
}

inline ns3::Vector2D operator/(const ns3::Vector2D &vec, double d) {
  return {vec.x / d, vec.y / d};
}

inline ns3::Vector2D operator*(const ns3::Vector2D &vec, double d) {
  return {vec.x * d, vec.y * d};
}

inline ns3::Vector2D operator*(double d, const ns3::Vector2D &vec) { return vec * d; }

inline ns3::Vector2D operator-=(ns3::Vector2D &vec1, const ns3::Vector2D &vec2) {
  vec1.x = vec1.x - vec2.x;
  vec1.y = vec1.y - vec2.y;
  return vec1;
}

inline ns3::Vector2D operator+=(ns3::Vector2D &vec1, const ns3::Vector2D &vec2) {
  vec1.x = vec1.x + vec2.x;
  vec1.y = vec1.y + vec2.y;
  return vec1;
}

inline ns3::Vector2D operator-(const ns3::Vector2D &vec) { return {-vec.x, -vec.y}; }

namespace daisi::util {

using Position = ns3::Vector2D;
using Velocity = ns3::Vector2D;
using Acceleration = ns3::Vector2D;
using Dimensions = ns3::Vector3D;
using Duration = double;
using Distance = double;
struct Pose {
  Pose() = default;
  Pose(Position position_m, double orientation_rad)
      : position(position_m), orientation(orientation_rad) {}
  explicit Pose(Position position_m) : position(position_m), orientation(0) {}

  Position position;
  double orientation = 0.0;
  SERIALIZE(position, orientation);
};

}  // namespace daisi::util

#endif  // DAISI_UTILS_STRUCTURE_HELPERS_H_
