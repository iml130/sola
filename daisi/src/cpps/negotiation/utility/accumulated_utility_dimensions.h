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

#ifndef DAISI_NEGOTIATION_UTILITY_ACCUMULATED_UTILITY_DIMENSION_NS3_H_
#define DAISI_NEGOTIATION_UTILITY_ACCUMULATED_UTILITY_DIMENSION_NS3_H_

#include "cpps/negotiation/utility/utility_dimensions.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {

class AccumulatedUtilityDimensions {
public:
  AccumulatedUtilityDimensions()
      : execution_duration_(0),
        execution_distance_(0),
        travel_to_pickup_duration_(0),
        travel_to_pickup_distance_(0),
        makespan_(0),
        included_orders_(0) {}

  AccumulatedUtilityDimensions(const double &execution_duration, const double &execution_distance,
                               const double &travel_to_pickup_duration,
                               const double &travel_to_pickup_distance, const double &makespan,
                               const int &included_orders)
      : execution_duration_(execution_duration),
        execution_distance_(execution_distance),
        travel_to_pickup_duration_(travel_to_pickup_duration),
        travel_to_pickup_distance_(travel_to_pickup_distance),
        makespan_(makespan),
        included_orders_(included_orders) {}

  AccumulatedUtilityDimensions(const AccumulatedUtilityDimensions &) = default;

  void addToHistory(const UtilityDimensions &udim) {
    execution_duration_ += udim.execution_duration;
    execution_distance_ += udim.execution_distance;
    travel_to_pickup_duration_ += udim.travel_to_pickup_duration;
    travel_to_pickup_distance_ += udim.travel_to_pickup_distance;
    makespan_ = std::max(makespan_, udim.makespan);

    included_orders_++;
  }

  double getAccumulatedExecutionDuration() const { return execution_duration_; }
  double getAccumulatedExecutionDistance() const { return execution_distance_; }
  double getAccumulatedTravelToPickupDuration() const { return travel_to_pickup_duration_; }
  double getAccumulatedTravelToPickupDistance() const { return travel_to_pickup_distance_; }
  double getMakespan() const { return makespan_; }

  double getNumberOfExecutedOrders() const { return included_orders_; }

  SERIALIZE(execution_duration_, execution_distance_, travel_to_pickup_duration_,
            travel_to_pickup_distance_, makespan_, included_orders_);

  static AccumulatedUtilityDimensions getDelta(const AccumulatedUtilityDimensions &before,
                                               const AccumulatedUtilityDimensions &after) {
    AccumulatedUtilityDimensions delta;

    delta.execution_duration_ = after.execution_duration_ - before.execution_duration_;
    delta.execution_distance_ = after.execution_distance_ - before.execution_distance_;
    delta.travel_to_pickup_duration_ =
        after.travel_to_pickup_duration_ - before.travel_to_pickup_duration_;
    delta.travel_to_pickup_distance_ =
        after.travel_to_pickup_distance_ - before.travel_to_pickup_distance_;
    delta.makespan_ = after.makespan_ - before.makespan_;

    return delta;
  }

private:
  double execution_duration_;
  double execution_distance_;
  double travel_to_pickup_duration_;
  double travel_to_pickup_distance_;
  double makespan_;
  int included_orders_;
};

}  // namespace daisi::cpps
#endif
