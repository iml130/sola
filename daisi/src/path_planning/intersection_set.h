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

#ifndef DAISI_PATH_PLANNING_GEOMETRY_INTERSECTION_SET_H_
#define DAISI_PATH_PLANNING_GEOMETRY_INTERSECTION_SET_H_

#include <ns3/vector.h>

#include <set>

namespace daisi::path_planning {
//! For simulation only: Singleton set that contains all intersections in this scenario
inline std::set<ns3::Vector2D> &getAllIntersections() {
  static std::set<ns3::Vector2D> all_intersections;
  return all_intersections;
}

}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_GEOMETRY_INTERSECTION_SET_H_
