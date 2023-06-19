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

#include "cpps/negotiation/task_management/basic_list_task_management.h"

#include <cassert>

namespace daisi::cpps {

BasicListTaskManagement::BasicListTaskManagement(
    const std::shared_ptr<Kinematics> &kinematics, const std::shared_ptr<amr::Ability> &ability,
    const std::shared_ptr<ns3::Vector> &last_position,
    const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
    const std::shared_ptr<UtilityEvaluator> &utility_evaluator)
    : TaskManagement(kinematics, ability, last_position, logger, utility_evaluator){};

int BasicListTaskManagement::getNumberOfOrdersInQueue() const { return queue_.size(); }

void BasicListTaskManagement::pickNextOrderToExecute() {
  std::tie(current_order_, current_utility_dimensions_) = queue_.front();
  queue_.erase(queue_.begin());
}

bool BasicListTaskManagement::hasOrdersInQueue() const { return !queue_.empty(); }

bool BasicListTaskManagement::canExecuteNextOrder() const { return hasOrdersInQueue(); }

bool BasicListTaskManagement::addOrder(Task order, const std::shared_ptr<InsertInfo> &info) {
  auto list_info = std::static_pointer_cast<ListInsertInfo>(info);

  order.setOrderState(OrderStates::kQueued);
  logger_->logTransportOrderUpdate(order, agv_uuid_);

  UtilityDimensions new_udims(order);
  std::list<std::pair<Task, UtilityDimensions>>::iterator it_new_element;
  if (list_info->list_position >= 0) {
    auto it = queue_.begin();
    std::advance(it, list_info->list_position);
    it_new_element = queue_.insert(it, {order, new_udims});
  } else {
    // -1 -> at beginning
    queue_.push_front({order, new_udims});
    it_new_element = queue_.begin();
  }

  updateQueue(it_new_element);
  return true;
}

void BasicListTaskManagement::updateQueue(
    std::list<std::pair<Task, UtilityDimensions>>::iterator &it) {
  bool iterator_at_first_position =
      std::get<0>(*it).getUUID() == std::get<0>(queue_.front()).getUUID();

  // it is the new inserted Order

  UtilityDimensions &new_udims = std::get<1>(*it);

  std::tie(new_udims.execution_duration, new_udims.execution_distance) =
      kinematics_->getStartStopTimeAndDistance(std::get<0>(*it).getPickupLocation(),
                                               std::get<0>(*it).getDeliveryLocation(), true);

  if (iterator_at_first_position) {
    auto const &[end_position, end_time] = getEndPositionAndTime();

    std::tie(new_udims.travel_to_pickup_duration, new_udims.travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(end_position,
                                                 std::get<0>(*it).getPickupLocation());

    new_udims.start_time = end_time + new_udims.travel_to_pickup_duration;

  } else {
    auto it_prev = std::prev(it);

    std::tie(new_udims.travel_to_pickup_duration, new_udims.travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(std::get<0>(*it_prev).getDeliveryLocation(),
                                                 std::get<0>(*it).getPickupLocation());
    new_udims.start_time = new_udims.travel_to_pickup_duration + std::get<1>(*it_prev).start_time +
                           std::get<1>(*it_prev).execution_duration;
  }

  new_udims.setQueueSize(queue_.size() - 1);
  utility_evaluator_->evaluate(new_udims);

  ns3::Vector new_order_delivery_location = std::get<0>(*it).getDeliveryLocation();
  std::advance(it, 1);

  if (it != queue_.end()) {
    // it is the order after the inserted order
    std::tie(std::get<1>(*it).travel_to_pickup_duration,
             std::get<1>(*it).travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(new_order_delivery_location,
                                                 std::get<0>(*it).getPickupLocation());
  }
}

UtilityDimensions BasicListTaskManagement::getUtilityDimensions(const Task &order) {
  return std::get<0>(BasicListTaskManagement::getUtilityDimensionsAndInsertInfo(order));
}

std::pair<UtilityDimensions, std::shared_ptr<InsertInfo>>
BasicListTaskManagement::getUtilityDimensionsAndInsertInfo(const Task &order) {
  UtilityDimensions dims(order);

  // Calculate costs
  std::tie(dims.execution_duration, dims.execution_distance) =
      kinematics_->getStartStopTimeAndDistance(order.getPickupLocation(),
                                               order.getDeliveryLocation(), true);

  if (queue_.empty()) {
    std::tie(dims.travel_to_pickup_duration, dims.travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(*last_position_, order.getPickupLocation());

  } else {
    std::tie(dims.travel_to_pickup_duration, dims.travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(std::get<0>(queue_.back()).getDeliveryLocation(),
                                                 order.getPickupLocation());
  }

  if (queue_.empty()) {
    double sim_time = (double)ns3::Simulator::Now().GetMilliSeconds();
    dims.start_time = sim_time + dims.travel_to_pickup_duration;

  } else {
    auto previous_dims = std::get<1>(queue_.back());
    dims.start_time = previous_dims.start_time + previous_dims.execution_duration;
  }

  auto info = std::make_shared<ListInsertInfo>();
  info->list_position = getNumberOfOrdersInQueue();

  dims.setQueueSize(getNumberOfOrdersInQueue());
  utility_evaluator_->evaluate(dims);

  return {dims, info};
}

}  // namespace daisi::cpps
