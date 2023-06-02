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

#ifndef DAISI_PATH_PLANNING_DESTINATION_H_
#define DAISI_PATH_PLANNING_DESTINATION_H_

#include <cstdint>
#include <functional>
#include <tuple>

#include "connection_points.h"

namespace daisi::path_planning {

//! Identifier of a route from a station and point type to another station with given point type
struct RouteIdentifier {
  uint32_t from_station_id = 0;
  MainConnPoints out_point;  // at ourself
  uint32_t to_station_id = 0;
  MainConnPoints in_point;  // at destination

  bool operator==(const RouteIdentifier &other) const {
    return std::tie(from_station_id, to_station_id, out_point, in_point) ==
           std::tie(other.from_station_id, to_station_id, other.out_point, other.in_point);
  }
};

struct DestinationHash {
  //! Hash implementation for \c RouteIdentifier
  std::size_t operator()(const RouteIdentifier &dest) const {
    return std::hash<uint32_t>()(dest.from_station_id) ^ std::hash<uint32_t>()(dest.to_station_id) ^
           std::hash<MainConnPoints>()(dest.out_point) ^ std::hash<MainConnPoints>()(dest.in_point);
  }
};

}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_DESTINATION_H_
