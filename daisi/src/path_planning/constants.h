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

#ifndef DAISI_PATH_PLANNING_CONSTANTS_H_
#define DAISI_PATH_PLANNING_CONSTANTS_H_

#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "ns3/vector.h"
#include "path_planning/consensus/central/central_settings.h"
#include "path_planning/consensus/paxos/paxos_settings.h"

namespace daisi::path_planning {

//! List of points with the time in seconds when they are reached
using PointTimePairs = std::vector<std::pair<ns3::Vector2D, double>>;

//! 2D vector
using PPVector = std::pair<float, float>;

using Timestamp = double;
using Intersection = PPVector;
using ID = uint32_t;

//! Map of intersections and all timestamps with their corresponding driving instances when this
//! intersection is occupied
using IntersectionOccupancy = std::unordered_map<Intersection, std::map<Timestamp, ID>>;
}  // namespace daisi::path_planning

namespace std {
template <> struct hash<daisi::path_planning::PPVector> {
  //! Hash function for PPVector
  size_t operator()(daisi::path_planning::PPVector const &p) const {
    return std::hash<double>()(p.first) ^ std::hash<double>()(p.second);
  }
};
}  // namespace std

#endif  // DAISI_PATH_PLANNING_CONSTANTS_H_
