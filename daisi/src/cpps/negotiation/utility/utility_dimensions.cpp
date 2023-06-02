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

#include "cpps/negotiation/utility/utility_dimensions.h"

#include <limits>

namespace daisi::cpps {

UtilityDimensions::UtilityDimensions()
    : start_time(0.0),
      spawn_time(0.0),
      earliest_start_time(0.0),
      latest_start_time(0.0),
      execution_duration(0.0),
      execution_distance(0.0),
      travel_to_pickup_duration(0.0),
      travel_to_pickup_distance(0.0),
      makespan(0.0),
      delta_execution_duration(0.0),
      delta_execution_distance(0.0),
      delta_travel_to_pickup_duration(0.0),
      delta_travel_to_pickup_distance(0.0),
      delta_makespan(0.0),
      quality_(0.0),
      quality_set_(true),
      costs_(std::numeric_limits<double>::max()),
      costs_set_(true),
      queue_size_(0),
      queue_size_set_(true){};

UtilityDimensions::UtilityDimensions(const Task &order)
    : start_time(0.0),
      spawn_time(order.time_window.getSpawnTime()),
      earliest_start_time(order.time_window.getEarliestStart()),
      latest_start_time(order.time_window.getLatestStart()),
      execution_duration(0.0),
      execution_distance(0.0),
      travel_to_pickup_duration(0.0),
      travel_to_pickup_distance(0.0),
      makespan(0.0),
      delta_execution_duration(0.0),
      delta_execution_distance(0.0),
      delta_travel_to_pickup_duration(0.0),
      delta_travel_to_pickup_distance(0.0),
      delta_makespan(0.0),
      quality_(0.0),
      quality_set_(false),
      costs_(0.0),
      costs_set_(false),
      queue_size_(0),
      queue_size_set_(false){};

UtilityDimensions UtilityDimensions::createInvalid() {
  UtilityDimensions dims;
  return dims;
}

}  // namespace daisi::cpps
