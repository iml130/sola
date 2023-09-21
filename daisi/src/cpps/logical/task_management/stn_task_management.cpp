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

#include "stn_task_management.h"

#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <sstream>

using namespace daisi::material_flow;

namespace daisi::cpps::logical {

template <class> inline constexpr bool kAlwaysFalseV = false;

StnTaskManagement::StnTaskManagement(const AmrDescription &amr_description,
                                     const Topology &topology, const daisi::util::Pose &pose)
    : AuctionBasedTaskManagement(amr_description, topology, pose),
      current_task_end_location_(std::nullopt),
      latest_calculated_insertion_info_(std::nullopt) {
  current_total_metrics_.setStartTime(0);
}

void StnTaskManagement::setCurrentTime(const daisi::util::Duration &now) {
  if (now < time_now_) {
    throw std::invalid_argument("new time must be later than current time");
  }

  updateOriginConstraints(now - time_now_);
  time_now_ = now;
}

void StnTaskManagement::updateOriginConstraints(const daisi::util::Duration &time_difference) {
  if (time_difference < 0) {
    return;
  }

  assert(vertices_[0].isOrigin());

  // outgoing edges are
  // [0] origin ---- ( + latest start/finish ) ----> vertex
  for (auto &edge : adjacency_matrix_[0]) {
    if (edge.has_value()) {
      assert(edge->getNumberOfWeights() == 1);
      auto weight = edge->getWeight();

      assert(weight >= 0);
      edge->updateWeight(0, weight + time_difference);
    }
  }

  // incoming edges are
  // [0] origin <---- ( - earliest start/finish ) ---- vertex
  // [1] origin <---- ( - travel constraint ) ---- vertex

  // update [0] with offset_difference
  // update [1] with travel time of new position
  for (size_t i = 0; i < adjacency_matrix_.size(); ++i) {  // NOLINT(modernize-loop-convert)
    auto &edge = adjacency_matrix_[i][0];
    if (edge.has_value()) {
      auto weight = edge->getWeight();
      assert(weight <= 0);
      edge->updateWeight(0, weight - time_difference);

      // TODO: update travel constraints with new position
    }
  }
}

bool StnTaskManagement::hasTasks() const { return current_task_.has_value(); }

Task StnTaskManagement::getCurrentTask() const {
  if (!hasTasks()) {
    throw std::logic_error("no tasks available");
  }
  return current_task_.value();
}

bool StnTaskManagement::setNextTask() {
  if (!current_ordering_.empty()) {
    auto current_insert_info = current_ordering_.front();
    current_task_ = current_insert_info.task;
    current_task_expected_finish_time_ =
        time_now_ + current_insert_info.metrics_composition.getCurrentMetrics().getTime();
    current_task_end_location_ = current_insert_info.end_locations.back();

    setCurrentTime(current_task_expected_finish_time_);

    // remove all vertices of the orders contained in the task
    for (const auto &order : current_task_->getOrders()) {
      auto order_start_vertex = getVertexOfOrder(order, true);
      removeVertex(order_start_vertex);

      auto order_finish_vertex = getVertexOfOrder(order, false);
      removeVertex(order_finish_vertex);
    }

    current_ordering_.erase(current_ordering_.begin());
    return true;
  }

  current_task_ = std::nullopt;
  return false;
}

bool StnTaskManagement::canAddTask(const Task &task,
                                   std::shared_ptr<InsertionPoint> insertion_point) {
  // TODO first checking whether we have the ability to execute the task

  latest_calculated_insertion_info_ = std::nullopt;

  StnTaskManagement copy(*this);
  copy.clearNotifyTaskAssignmentCallback();
  if (copy.addTask(task, insertion_point)) {
    latest_calculated_insertion_info_ = copy.getLatestCalculatedInsertionInfo();
    return true;
  }

  return false;
}

bool StnTaskManagement::addTask(
    const Task &task, std::shared_ptr<AuctionBasedTaskManagement::InsertionPoint> insertion_point) {
  latest_calculated_insertion_info_ = std::nullopt;

  auto orders = task.getOrders();
  if (orders.empty()) {
    throw std::invalid_argument("Task must have at least one order");
  }

  StnTaskManagement::TaskInsertInfo info{task, {}, {}};

  for (auto orders_it = orders.begin(); orders_it != orders.end(); orders_it++) {
    StnTaskManagementVertex start_curr{*orders_it, true};
    StnTaskManagementVertex finish_curr{*orders_it, false};

    addVertex(start_curr);
    addVertex(finish_curr);

    if (task.hasTimeWindow() && orders_it == orders.begin()) {
      // earliest start constraint for first order of task

      if (task.getTimeWindow().getAbsoluteEarliestStart() - time_now_ < 0) {
        return false;  // task must have started in the past to be still in the correct time window
      }

      addUnaryConstraint(start_curr, task.getTimeWindow().getAbsoluteEarliestStart() - time_now_,
                         std::nullopt);
    }

    if (task.hasTimeWindow() && orders_it == --orders.end()) {
      // latest finish constraint for last order of task

      addUnaryConstraint(finish_curr, std::nullopt,
                         task.getTimeWindow().getAbsoluteLatestFinish() - time_now_);
    }

    // precedence constraint to previous order in task
    // always sequential -> cannot change
    if (orders_it != orders.begin()) {
      auto previous_finish = getVertexOfOrder(*std::prev(orders_it), false);
      addBinaryConstraint(previous_finish, start_curr, 0, std::nullopt);
    }

    addDurationConstraints(start_curr, finish_curr, *orders_it, info);

    auto end_location_of_order = TaskManagementHelper::getEndLocationOfOrder(*orders_it);
    if (end_location_of_order.has_value()) {
      info.end_locations.push_back(end_location_of_order.value());
    } else {
      info.end_locations.push_back(info.end_locations.back());
    }
  }

  for (const auto &prec_task : task.getPrecedingTaskUuids()) {
    addPrecedenceConstraintBetweenTask(getVertexOfOrder(orders.front(), true), prec_task);
  }

  bool added = false;
  if (insertion_point) {
    std::shared_ptr<StnTaskManagement::StnInsertionPoint> stn_insertion_point =
        std::static_pointer_cast<StnTaskManagement::StnInsertionPoint>(insertion_point);

    addOrderingConstraintBetweenTasks(*stn_insertion_point, info);

    // double check if its still valid
    if (solve()) {
      MetricsComposition metrics = newest_task_insert_info_->metrics_composition;
      latest_calculated_insertion_info_ = std::make_pair(metrics, insertion_point);
      added = true;
    }

  } else {
    auto result = addBestOrdering(info);
    if (result.has_value()) {
      latest_calculated_insertion_info_ = result.value();
      added = true;
    }
  }

  if (added) {
    for (auto &callback : task_assignment_callbacks_) {
      callback();
    }
    return true;
  }

  return false;
}

void StnTaskManagement::addPrecedenceConstraintBetweenTask(
    const StnTaskManagementVertex &start_vertex, const std::string &precedence_task_name) {
  auto task_info_it =
      std::find_if(current_ordering_.begin(), current_ordering_.end(),
                   [&](const auto &info) { return info.task.getName() == precedence_task_name; });

  if (task_info_it != current_ordering_.end()) {
    auto last_finish_vertex = getVertexOfOrder(task_info_it->task.getOrders().back(), false);
    addBinaryConstraint(last_finish_vertex, start_vertex, 0, std::nullopt);
  }
}

void StnTaskManagement::addDurationConstraints(
    const StnTaskManagementVertex &start_vertex, const StnTaskManagementVertex &finish_vertex,
    const Order &order, const StnTaskManagement::TaskInsertInfo &task_insert_info) {
  addBinaryConstraint(start_vertex, finish_vertex,
                      calcOrderDurationForInsert(order, task_insert_info), std::nullopt);
}

daisi::util::Position StnTaskManagement::getLastPositionBefore(const int task_index) {
  if (task_index == 0) {
    if (hasTasks()) {
      return current_task_end_location_->getPosition();
    }
    return current_pose_.position;
  }

  return current_ordering_[task_index - 1].end_locations.back().getPosition();
}

std::vector<StnTaskManagement::StnInsertionPoint> StnTaskManagement::calcInsertionPoints() {
  std::vector<StnTaskManagement::StnInsertionPoint> insertion_points;

  if (current_ordering_.empty()) {
    insertion_points.push_back(
        StnTaskManagement::StnInsertionPoint{{}, *vertices_.begin(), std::nullopt, 0});
  } else {
    auto next_start = getVertexOfOrder(current_ordering_.front().task.getOrders().front(), true);

    insertion_points.push_back(
        StnTaskManagement::StnInsertionPoint{{}, *vertices_.begin(), next_start, 0});
  }

  int i = 1;
  for (auto it = current_ordering_.begin(); it != current_ordering_.end(); it++) {
    auto next = std::next(it, 1);

    auto previous_finish = getVertexOfOrder(it->task.getOrders().back(), false);

    if (next != current_ordering_.end()) {
      auto next_start = getVertexOfOrder(next->task.getOrders().front(), true);

      insertion_points.push_back(
          StnTaskManagement::StnInsertionPoint{{}, previous_finish, next_start, i});
    } else {
      insertion_points.push_back(
          StnTaskManagement::StnInsertionPoint{{}, previous_finish, std::nullopt, i});
    }

    i++;
  }

  return insertion_points;
}

bool StnTaskManagement::solve() {
  bool success = SimpleTemporalNetwork::solve();
  if (success) {
    updateCurrentOrdering();
  }
  return success;
}

void StnTaskManagement::updateCurrentOrdering() {
  std::unordered_map<Task, daisi::util::Duration> start_time_mapping;

  // updating current metrics
  for (size_t i = 0; i < current_ordering_.size(); i++) {
    auto &task_info = current_ordering_[i];

    Metrics new_current_metric;

    std::vector<daisi::util::Duration> order_start_times;

    for (size_t j = 0; j < task_info.task.getOrders().size(); j++) {
      auto order = task_info.task.getOrders()[j];

      const auto start_index = getVertexIndexOfOrder(order, true);
      const double start_time = -d_graph_[start_index][0];

      order_start_times.push_back(start_time + time_now_);
      insertOrderPropertiesIntoMetrics(order, new_current_metric, task_info, i);
    }

    auto task_start_time = *std::min_element(order_start_times.begin(), order_start_times.end());

    start_time_mapping[task_info.task] = task_start_time;
    new_current_metric.setExecutionStartTime(task_start_time);

    // time required to get to the start of the first order in the task
    new_current_metric.start_up_time = calcGetToStartDuration(i);

    task_info.metrics_composition.updateCurrentMetrics(new_current_metric);
  }

  // sorting ordering by start time
  std::sort(current_ordering_.begin(), current_ordering_.end(),
            [&start_time_mapping](const StnTaskManagement::TaskInsertInfo &i1,
                                  const StnTaskManagement::TaskInsertInfo &i2) {
              return start_time_mapping[i1.task] < start_time_mapping[i2.task];
            });

  // calc new total metrics
  auto previous_total_metrics = current_total_metrics_;
  for (const auto &info : current_ordering_) {
    current_total_metrics_ = previous_total_metrics + info.metrics_composition.getCurrentMetrics();
  }

  // set metric difference in new task
  auto find_new_task_lambda = [&](const StnTaskManagement::TaskInsertInfo &info) {
    return !info.metrics_composition.hasDiffInsertionMetrics();
  };
  if (std::count_if(current_ordering_.begin(), current_ordering_.end(), find_new_task_lambda) !=
      1) {
    throw std::logic_error("there should be exactly one entry without diff metrics");
  }

  auto new_task_it =
      std::find_if(current_ordering_.begin(), current_ordering_.end(), find_new_task_lambda);
  Metrics diff_metrics_for_new_task = current_total_metrics_ - previous_total_metrics;
  new_task_it->metrics_composition.setDiffInsertionMetrics(diff_metrics_for_new_task);
  new_task_it->metrics_composition.fixInsertionMetrics();
  newest_task_insert_info_ = new_task_it;
}

std::optional<std::pair<MetricsComposition, std::shared_ptr<StnTaskManagement::StnInsertionPoint>>>
StnTaskManagement::addBestOrdering(StnTaskManagement::TaskInsertInfo &task_insert_info) {
  auto insertion_points = calcInsertionPoints();

  int best_index = -1;
  std::optional<MetricsComposition> best_metrics = std::nullopt;

  for (auto i = 0; i < insertion_points.size(); i++) {
    const auto &point = insertion_points[i];

    StnTaskManagement copy(*this);
    copy.addOrderingConstraintBetweenTasks(point, task_insert_info);
    bool success = copy.solve();

    if (success) {
      MetricsComposition current_metrics = copy.newest_task_insert_info_->metrics_composition;

      if (!best_metrics.has_value() || best_metrics.value() < current_metrics) {
        best_metrics = current_metrics;
        best_index = i;
      }
    }
  }

  if (best_index >= 0) {
    addOrderingConstraintBetweenTasks(insertion_points[best_index], task_insert_info);
    bool success = solve();
    if (!success) {
      throw std::logic_error("failed to solve although it was solvable on the copy");
    }

    return std::make_pair(
        newest_task_insert_info_->metrics_composition,
        std::make_shared<StnTaskManagement::StnInsertionPoint>(insertion_points[best_index]));
  }

  return std::nullopt;
}

void StnTaskManagement::addOrderingConstraintBetweenTasks(
    StnTaskManagement::StnInsertionPoint insertion_point,
    StnTaskManagement::TaskInsertInfo &task_insert_info) {
  current_ordering_.insert(current_ordering_.begin() + insertion_point.new_index, task_insert_info);

  auto start_vertex = getVertexOfOrder(task_insert_info.task.getOrders().front(), true);
  addBinaryConstraint(insertion_point.previous_finish, start_vertex, 0, std::nullopt);

  updateGetToStartDurationConstraint(insertion_point.new_index);

  if (insertion_point.next_start.has_value()) {
    addBinaryConstraint(getVertexOfOrder(task_insert_info.task.getOrders().back(), false),
                        insertion_point.next_start.value(), 0, std::nullopt);

    updateGetToStartDurationConstraint(insertion_point.new_index + 1);
  }
}

daisi::util::Duration StnTaskManagement::calcOrderDurationForInsert(
    const Order &order, const TaskInsertInfo &task_insert_info) const {
  const auto orders = task_insert_info.task.getOrders();
  const auto it = std::find(orders.begin(), orders.end(), order);
  const int order_index = it - orders.begin();

  if (std::holds_alternative<ActionOrder>(order)) {
    auto previous_location = task_insert_info.end_locations[order_index - 1];
    auto funcs = materialFlowToFunctionalities({order}, previous_location.getPosition());
    return AmrMobilityHelper::estimateDuration(daisi::util::Pose(), funcs, amr_description_,
                                               topology_, false);
  }

  if (std::holds_alternative<MoveOrder>(order)) {
    assert(order_index != 0);

    auto previous_location = task_insert_info.end_locations[order_index - 1];
    auto funcs = materialFlowToFunctionalities({order}, previous_location.getPosition());

    return AmrMobilityHelper::estimateDuration(daisi::util::Pose(), funcs, amr_description_,
                                               topology_, false);
  }

  if (std::holds_alternative<TransportOrder>(order)) {
    if (order_index > 0) {
      auto previous_location = task_insert_info.end_locations[order_index - 1];
      auto funcs = materialFlowToFunctionalities({order}, previous_location.getPosition());
      return AmrMobilityHelper::estimateDuration(daisi::util::Pose(), funcs, amr_description_,
                                                 topology_, false);
    }

    // order_index == 0
    // previous position does not matter because we remove the
    // first MoveTo functionality from funcs anyways
    daisi::util::Position previous_position(-1, -1);
    auto funcs = materialFlowToFunctionalities({order}, previous_position);

    if (auto move_to = std::get_if<MoveTo>(&funcs.front())) {
      auto start_position = move_to->destination;
      funcs.erase(funcs.begin());

      return AmrMobilityHelper::estimateDuration(daisi::util::Pose(start_position), funcs,
                                                 amr_description_, topology_, false);
    }

    throw std::runtime_error("First functionality of TransportOrder must be MoveTo.");
  }

  throw std::runtime_error("Unknown Order Type.");
}

void StnTaskManagement::insertOrderPropertiesIntoMetrics(
    const Order &order, Metrics &metrics, const StnTaskManagement::TaskInsertInfo &task_insert_info,
    const int task_ordering_index) {
  const auto orders = task_insert_info.task.getOrders();
  const auto order_it = std::find(orders.begin(), orders.end(), order);
  const int order_index = order_it - orders.begin();

  if (std::holds_alternative<MoveOrder>(order)) {
    if (order_index == 0) {
      throw std::invalid_argument("move order should not be first");
    }

    auto previous_location = task_insert_info.end_locations[order_index - 1];

    auto funcs = materialFlowToFunctionalities({order}, previous_location.getPosition());
    metrics.empty_travel_time += AmrMobilityHelper::estimateDuration(
        daisi::util::Pose{}, funcs, amr_description_, topology_, false);

    metrics.empty_travel_distance =
        AmrMobilityHelper::calculateDistance(previous_location.getPosition(), funcs);

  } else if (std::holds_alternative<TransportOrder>(order)) {
    daisi::util::Position previous_position;
    if (order_index > 0) {
      previous_position = task_insert_info.end_locations[order_index - 1].getPosition();

    } else {
      // checking location of previous task
      previous_position = getLastPositionBefore(task_ordering_index);
    }

    auto funcs = materialFlowToFunctionalities({order});
    auto res = AmrMobilityHelper::calculateMetricsByDomain(previous_position, funcs,
                                                           amr_description_, topology_);
    metrics.empty_travel_time += std::get<0>(res);
    metrics.loaded_travel_time += std::get<1>(res);
    metrics.action_time += std::get<2>(res);
    metrics.empty_travel_distance += std::get<3>(res);
    metrics.loaded_travel_distance += std::get<4>(res);

  } else if (std::holds_alternative<ActionOrder>(order)) {
    auto previous_location = task_insert_info.end_locations[order_index - 1];

    auto funcs = materialFlowToFunctionalities({order}, previous_location.getPosition());
    metrics.action_time += AmrMobilityHelper::estimateDuration(daisi::util::Pose{}, funcs,
                                                               amr_description_, topology_, false);
  } else {
    throw std::invalid_argument("Order type not supported");
  }
}

void StnTaskManagement::updateGetToStartDurationConstraint(const int task_index_to_update) {
  const auto duration = calcGetToStartDuration(task_index_to_update);

  const auto &task_info_to_update = current_ordering_[task_index_to_update];
  const auto &this_task_first_order = task_info_to_update.task.getOrders().front();

  StnTaskManagementVertex previous_finish_vertex = getOrigin();
  if (task_index_to_update > 0) {
    const auto &previous_task_last_order =
        current_ordering_[task_index_to_update - 1].task.getOrders().back();
    previous_finish_vertex = getVertexOfOrder(previous_task_last_order, false);
  }
  StnTaskManagementVertex this_start_vertex = getVertexOfOrder(this_task_first_order, true);

  updateLastBinaryConstraint(previous_finish_vertex, this_start_vertex, duration, std::nullopt);
}

util::Duration StnTaskManagement::calcGetToStartDuration(const int task_index_to_update) {
  auto last_position = getLastPositionBefore(task_index_to_update);

  const auto &task_info_to_update = current_ordering_[task_index_to_update];
  const auto &this_task_first_order = task_info_to_update.task.getOrders().front();

  if (!std::holds_alternative<TransportOrder>(this_task_first_order)) {
    throw std::invalid_argument("Only supports transport orders at the beginngin of a task.");
  }

  auto funcs = materialFlowToFunctionalities({this_task_first_order}, last_position);
  auto duration = AmrMobilityHelper::estimateDuration(
      daisi::util::Pose(last_position), {funcs.front()}, amr_description_, topology_, false);

  return duration;
}

StnTaskManagement::VertexIterator StnTaskManagement::getVertexIteratorOfOrder(const Order &order,
                                                                              const bool start) {
  auto it = std::find_if(vertices_.begin(), vertices_.end(),
                         [&start, &order](const StnTaskManagementVertex &v) {
                           return v.isStart() == start && order == v.getOrder();
                         });

  if (it == vertices_.end()) {
    throw std::runtime_error("Order not part of STN");
  }

  return it;
}

int StnTaskManagement::getVertexIndexOfOrder(const Order &order, const bool start) {
  return std::distance(vertices_.begin(), getVertexIteratorOfOrder(order, start));
}

const StnTaskManagementVertex &StnTaskManagement::getVertexOfOrder(const Order &order,
                                                                   const bool start) {
  return *getVertexIteratorOfOrder(order, start);
}

std::pair<MetricsComposition, std::shared_ptr<AuctionBasedTaskManagement::InsertionPoint>>
StnTaskManagement::getLatestCalculatedInsertionInfo() const {
  if (latest_calculated_insertion_info_.has_value()) {
    return latest_calculated_insertion_info_.value();
  }
  throw std::logic_error(
      "There are no intertion info if the canAddTask or addTask were not sucessful");
}

}  // namespace daisi::cpps::logical
