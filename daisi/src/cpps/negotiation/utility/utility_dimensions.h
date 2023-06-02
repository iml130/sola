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

#ifndef DAISI_NEGOTIATION_UTILITY_UTILITY_DIMENSION_NS3_H_
#define DAISI_NEGOTIATION_UTILITY_UTILITY_DIMENSION_NS3_H_

#include "cpps/message/serialize.h"
#include "cpps/model/task.h"

namespace daisi::cpps {

class UtilityDimensions {
public:
  UtilityDimensions();
  explicit UtilityDimensions(const Task &order);

  double start_time;
  double spawn_time;
  double earliest_start_time;
  double latest_start_time;

  double execution_duration;
  double execution_distance;
  double travel_to_pickup_duration;
  double travel_to_pickup_distance;
  double makespan;

  double delta_execution_duration;
  double delta_execution_distance;
  double delta_travel_to_pickup_duration;
  double delta_travel_to_pickup_distance;
  double delta_makespan;

  double getFinishTime() const { return start_time + execution_duration; }

  double getDelay() const { return start_time - earliest_start_time; }
  double getBuffer() const { return latest_start_time - earliest_start_time; }

  static UtilityDimensions createInvalid();

  double getQuality() const {
    if (quality_set_) {
      return quality_;
    }
    throw std::logic_error("Quality not set");
  }

  double getCosts() const {
    if (costs_set_) {
      return costs_;
    }
    throw std::logic_error("Costs not set");
  }

  int getQueueSize() const {
    if (queue_size_set_) {
      return queue_size_;
    }
    throw std::logic_error("Queue Size not set");
  }

  void setQuality(const double &quality) {
    quality_set_ = true;
    quality_ = quality;
  }

  void setCosts(const double &costs) {
    costs_set_ = true;
    costs_ = costs;
  }

  void setQueueSize(const int &size) {
    if (queue_size_set_) {
      throw std::logic_error("Queue size already set. May not be set again.");
    }
    queue_size_set_ = true;
    queue_size_ = size;
  }

  double getUtility() const { return getQuality() - getCosts(); };

  bool isQualitySet() const { return quality_set_; }
  bool isCostsSet() const { return costs_set_; }
  bool isQueueSizeSet() const { return queue_size_set_; }

  SERIALIZE(start_time, spawn_time, earliest_start_time, latest_start_time, execution_duration,
            execution_distance, travel_to_pickup_duration, travel_to_pickup_distance, makespan,
            delta_execution_duration, delta_execution_distance, delta_travel_to_pickup_duration,
            delta_travel_to_pickup_distance, delta_makespan, quality_, quality_set_, costs_,
            costs_set_, queue_size_, queue_size_set_);

private:
  double quality_;
  bool quality_set_;

  double costs_;
  bool costs_set_;

  int queue_size_;
  bool queue_size_set_;
};

}  // namespace daisi::cpps
#endif
