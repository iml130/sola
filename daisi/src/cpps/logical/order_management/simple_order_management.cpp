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

#include "simple_order_management.h"

using namespace daisi::material_flow;
namespace daisi::cpps::logical {

SimpleOrderManagement::SimpleOrderManagement(const AmrDescription &amr_description,
                                             const Topology &topology,
                                             const daisi::util::Pose &pose)
    : OrderManagement(amr_description, topology, pose),
      active_task_(),
      queue_(),
      final_metrics_(),
      expected_end_position_(pose.position),
      time_now_(0) {}

Metrics SimpleOrderManagement::getFinalMetrics() const { return final_metrics_; }

void SimpleOrderManagement::setCurrentTime(const daisi::util::Duration &now) {
  if (now < time_now_) {
    throw std::invalid_argument("new time must be later than current time");
  }
  time_now_ = now;
}

bool SimpleOrderManagement::hasTasks() const { return active_task_.has_value(); }

Task SimpleOrderManagement::getCurrentTask() const {
  if (!hasTasks()) {
    throw std::logic_error("no tasks available");
  }
  return active_task_.value();
}

bool SimpleOrderManagement::setNextTask() {
  if (!queue_.empty()) {
    active_task_ = queue_.front();
    queue_.erase(queue_.begin());
    return true;
  }
  active_task_ = std::nullopt;
  return false;
}

bool SimpleOrderManagement::canAddTask(const Task &task) {
  SimpleOrderManagement copy(*this);
  bool result = copy.addTask(task);

  return result;
}

bool SimpleOrderManagement::addTask(const Task &task) {
  // simply add all orders in the given order
  const auto orders = task.getOrders();
  if (orders.empty()) {
    throw std::invalid_argument("Task must have at least one order");
  }

  queue_.push_back(task);

  // update current_metrics to match the final order of the new task
  updateFinalMetrics();

  // find final order with an endpoint to update expected_end_position_
  std::optional<Order> final_order = std::nullopt;
  for (auto rev_orders_it = orders.rbegin(); rev_orders_it != orders.rend(); rev_orders_it++) {
    if (std::holds_alternative<TransportOrder>(*rev_orders_it) ||
        std::holds_alternative<MoveOrder>(*rev_orders_it)) {
      final_order = *rev_orders_it;
      break;
    }
  }
  if (!final_order.has_value()) {
    throw std::logic_error("Task must contain at least one TransportOrder or MoveOrder");
  }
  auto end_location = OrderManagementHelper::getEndLocationOfOrder(final_order.value());
  expected_end_position_ = end_location->getPosition();

  return true;
}

void SimpleOrderManagement::updateFinalMetrics() {
  // calculate the start time and metrics for the new task
  auto start_time = std::max(final_metrics_.getMakespan(), time_now_);

  Metrics new_current_metrics;
  Task new_task = queue_.back();
  auto orders = new_task.getOrders();

  // iterate through orders to find the correct start time for the final order
  for (auto it = orders.begin(); it != orders.end(); it++) {
    new_current_metrics = Metrics();
    auto order = *it;
    insertOrderPropertiesIntoMetrics(order, new_current_metrics, new_task, start_time);
    start_time = new_current_metrics.getMakespan();
  }

  // update current metrics
  final_metrics_ = new_current_metrics;
}

void SimpleOrderManagement::insertOrderPropertiesIntoMetrics(
    const Order &order, Metrics &metrics, const Task &task,
    const daisi::util::Duration &start_time) {
  if (!metrics.isStartTimeSet()) {
    metrics.setStartTime(start_time);
  }
  const auto orders = task.getOrders();
  auto order_it = std::find(orders.rbegin(), orders.rend(), order);
  const int order_index = orders.rend() - order_it - 1;

  // determine end location of previous order
  std::optional<Location> previous_location;
  order_it++;
  for (; order_it != orders.rend(); order_it++) {
    previous_location = OrderManagementHelper::getEndLocationOfOrder(*order_it);
    if (previous_location.has_value()) {
      break;
    }
  }
  daisi::util::Position previous_position;
  if (!previous_location.has_value()) {
    if (!expected_end_position_.has_value()) {
      throw std::logic_error("there must exist a location for the amr to start from");
    }
    previous_position = expected_end_position_.value();
  } else {
    previous_position = previous_location.value().getPosition();
  }
  auto funcs = materialFlowToFunctionalities({order}, previous_position);

  if (std::holds_alternative<MoveOrder>(order)) {
    if (order_index == 0) {
      throw std::invalid_argument("move order should not be first");
    }
    metrics.empty_travel_time += AmrMobilityHelper::estimateDuration(
        daisi::util::Pose{}, funcs, amr_description_, topology_, false);

    metrics.empty_travel_distance = AmrMobilityHelper::calculateDistance(previous_position, funcs);
  } else if (std::holds_alternative<TransportOrder>(order)) {
    auto res = AmrMobilityHelper::calculateMetricsByDomain(previous_position, funcs,
                                                           amr_description_, topology_);
    metrics.empty_travel_time += std::get<0>(res);
    metrics.loaded_travel_time += std::get<1>(res);
    metrics.action_time += std::get<2>(res);
    metrics.empty_travel_distance += std::get<3>(res);
    metrics.loaded_travel_distance += std::get<4>(res);
  } else if (std::holds_alternative<ActionOrder>(order)) {
    metrics.action_time += AmrMobilityHelper::estimateDuration(daisi::util::Pose{}, funcs,
                                                               amr_description_, topology_, false);
  } else {
    throw std::invalid_argument("Order type not supported");
  }
}

}  // namespace daisi::cpps::logical