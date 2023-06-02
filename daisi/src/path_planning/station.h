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

#ifndef DAISI_PATH_PLANNING_STATION_H_
#define DAISI_PATH_PLANNING_STATION_H_

#include "agv_info.h"
#include "destination.h"
#include "ns3/vector.h"
#include "route.h"

namespace daisi::path_planning {

//! Map to allow finding a route just with a route identifier
using StationRoutes = std::unordered_map<RouteIdentifier, Route, DestinationHash>;

struct IntersectionTimeInfo {
  PPVector intersection;
  double time_at_intersection;

  // Explicit serialization function needed to serialize PPVector
  template <class Archive> void serialize(Archive &ar) {
    ar(intersection.first, intersection.second, time_at_intersection);
  }
};

//! Map of all delivery stations (id) to their center points
using DeliveryStationPoints = std::unordered_map<uint32_t, ns3::Vector2D>;

//! Information for a pickup station about other pickup stations
struct PickupStationHandoverInfo {
  uint32_t id;
  ns3::Vector3D pos;
  std::string ip;
};

struct DeliveryStationInfo {
  uint32_t station_id = 0;
  ns3::Vector2D center_pos;
};

struct PickupStationInfo {
  uint32_t station_id;
  ns3::Vector2D center_pos;
  ns3::Vector2D out_pickup;
  ns3::Vector2D in_pickup;
  StationRoutes routes;
  DeliveryStationPoints delivery_station;
  std::vector<PickupStationHandoverInfo> pickup_station_registry;
};

}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_STATION_H_
