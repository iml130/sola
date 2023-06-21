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

#include "stn_order_management.h"

#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <sstream>

using namespace daisi::material_flow;

namespace daisi::cpps::logical {

template <class> inline constexpr bool kAlwaysFalseV = false;

StnOrderManagement::StnOrderManagement(const AmrDescription &amr_description,
                                       const Topology &topology, const daisi::util::Pose &pose)
    : AuctionBasedOrderManagement(amr_description, topology, pose),
      current_task_end_location_(std::nullopt),
      current_task_expected_finish_time_(0),
      time_now_(0),
      latest_calculated_insertion_info_(std::nullopt) {}

void StnOrderManagement::setCurrentTime(const daisi::util::Duration &now) {
  if (now < time_now_) {
    throw std::invalid_argument("new time must be later than current time");
  }

  updateOriginConstraints(now - time_now_);
  time_now_ = now;
}

void StnOrderManagement::updateOriginConstraints(const daisi::util::Duration &time_difference) {
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
  for (size_t i = 0; i < adjacency_matrix_.size(); ++i) {
    auto &edge = adjacency_matrix_[i][0];
    if (edge.has_value()) {
      auto weight = edge->getWeight();
      assert(weight <= 0);
      edge->updateWeight(0, weight - time_difference);

      // TODO: update travel constraints with new position
    }
  }
}

bool StnOrderManagement::hasTasks() const { return current_task_.has_value(); }

Task StnOrderManagement::getCurrentTask() const {
  if (!hasTasks()) {
    throw std::logic_error("no tasks available");
  }
  return current_task_.value();
}

bool StnOrderManagement::setNextTask() {
  if (!current_ordering_.empty()) {
    auto current_insert_info = current_ordering_.front();
    current_task_ = current_insert_info.task;
    current_task_expected_finish_time_ =
        time_now_ + current_insert_info.metrics_composition.getCurrentMetrics().getTime();
    current_task_end_location_ = current_insert_info.end_locations.back();

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

bool StnOrderManagement::canAddTask(const Task &task) {
  // TODO first checking whether we have the ability to execute the task

  latest_calculated_insertion_info_ = std::nullopt;

  StnOrderManagement copy(*this);
  if (copy.addTask(task)) {
    latest_calculated_insertion_info_ = copy.getLatestCalculatedInsertionInfo();
    return true;
  }

  return false;
}

bool StnOrderManagement::addTask(
    const Task &task,
    std::shared_ptr<AuctionBasedOrderManagement::InsertionPoint> insertion_point) {
  latest_calculated_insertion_info_ = std::nullopt;

  const auto orders = task.getOrders();
  if (orders.empty()) {
    throw std::invalid_argument("Task must have at least one order");
  }

  StnOrderManagement::TaskInsertInfo info{task, {}, {}};

  for (auto orders_it = orders.begin(); orders_it != orders.end(); orders_it++) {
    StnOrderManagementVertex start_curr{*orders_it, true};
    StnOrderManagementVertex finish_curr{*orders_it, false};

    addVertex(start_curr);
    addVertex(finish_curr);

    // TODO time windows

    // precedence constraint to previous order in task
    // always sequential -> cannot change
    if (orders_it != orders.begin()) {
      auto previous_finish = getVertexOfOrder(*std::prev(orders_it), false);
      addBinaryConstraint(previous_finish, start_curr, 0, std::nullopt);
    }

    addDurationConstraints(start_curr, finish_curr, *orders_it, info);

    auto end_location_of_order = getEndLocationOfOrder(*orders_it);
    if (end_location_of_order.has_value()) {
      info.end_locations.push_back(end_location_of_order.value());
    } else {
      info.end_locations.push_back(info.end_locations.back());
    }
  }

  // TODO time windows

  for (const auto &prec_task : task.getPrecedingTasks()) {
    addPrecedenceConstraintBetweenTask(getVertexOfOrder(orders.front(), true), prec_task);
  }

  if (insertion_point) {
    std::shared_ptr<StnOrderManagement::StnInsertionPoint> stn_insertion_point =
        std::static_pointer_cast<StnOrderManagement::StnInsertionPoint>(insertion_point);

    addOrderingConstraintBetweenTasks(*stn_insertion_point, info);

    // double check if its still valid
    bool success = solve();
    if (success) {
      MetricsComposition metrics = newest_task_insert_info_->metrics_composition;
      latest_calculated_insertion_info_ = std::make_pair(metrics, insertion_point);
      return true;
    }

  } else {
    auto result = addBestOrdering(info);
    if (result.has_value()) {
      latest_calculated_insertion_info_ = result.value();
      return true;
    }
  }

  return false;
}

void StnOrderManagement::addPrecedenceConstraintBetweenTask(
    const StnOrderManagementVertex &start_vertex, const std::string &precedence_task_name) {
  auto task_info_it =
      std::find_if(current_ordering_.begin(), current_ordering_.end(),
                   [&](const auto &info) { return info.task.getName() == precedence_task_name; });

  if (task_info_it != current_ordering_.end()) {
    auto last_finish_vertex = getVertexOfOrder(task_info_it->task.getOrders().back(), false);
    addBinaryConstraint(last_finish_vertex, start_vertex, 0, std::nullopt);
  }
}

void StnOrderManagement::addDurationConstraints(
    const StnOrderManagementVertex &start_vertex, const StnOrderManagementVertex &finish_vertex,
    const Order &order, const StnOrderManagement::TaskInsertInfo &task_insert_info) {
  addBinaryConstraint(start_vertex, finish_vertex,
                      calcOrderDurationForInsert(order, task_insert_info), std::nullopt);
}

daisi::util::Position StnOrderManagement::getLastPositionBefore(const int task_index) {
  if (task_index == 0) {
    if (hasTasks()) {
      return current_task_end_location_->getPosition();
    }
    return current_pose_.position;
  }

  return current_ordering_[task_index - 1].end_locations.back().getPosition();
}

std::vector<StnOrderManagement::StnInsertionPoint> StnOrderManagement::calcInsertionPoints() {
  std::vector<StnOrderManagement::StnInsertionPoint> insertion_points;

  if (current_ordering_.empty()) {
    insertion_points.push_back(
        StnOrderManagement::StnInsertionPoint{{}, *vertices_.begin(), std::nullopt, 0});
  } else {
    auto next_start = getVertexOfOrder(current_ordering_.front().task.getOrders().front(), true);

    insertion_points.push_back(
        StnOrderManagement::StnInsertionPoint{{}, *vertices_.begin(), next_start, 0});
  }

  int i = 1;
  for (auto it = current_ordering_.begin(); it != current_ordering_.end(); it++) {
    auto next = std::next(it, 1);

    auto previous_finish = getVertexOfOrder(it->task.getOrders().back(), false);

    if (next != current_ordering_.end()) {
      auto next_start = getVertexOfOrder(next->task.getOrders().front(), true);

      insertion_points.push_back(
          StnOrderManagement::StnInsertionPoint{{}, previous_finish, next_start, i});
    } else {
      insertion_points.push_back(
          StnOrderManagement::StnInsertionPoint{{}, previous_finish, std::nullopt, i});
    }

    i++;
  }

  return insertion_points;
}

bool StnOrderManagement::solve() {
  bool success = SimpleTemporalNetwork::solve();
  if (success) {
    updateCurrentOrdering();
  }
  return success;
}

void StnOrderManagement::updateCurrentOrdering() {
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

      const auto finish_index = getVertexIndexOfOrder(order, false);
      const double finish_time = -d_graph_[finish_index][0];

      order_start_times.push_back(start_time);
      insertOrderPropertiesIntoMetrics(order, new_current_metric, task_info, i, start_time);
    }

    auto task_start_time = *std::min_element(order_start_times.begin(), order_start_times.end());
    auto offset = std::max(current_task_expected_finish_time_, time_now_);

    start_time_mapping[task_info.task] = task_start_time;
    new_current_metric.setStartTime(task_start_time + offset);

    task_info.metrics_composition.updateCurrentMetrics(new_current_metric);
  }

  // sorting ordering by start time
  std::sort(current_ordering_.begin(), current_ordering_.end(),
            [&start_time_mapping](const StnOrderManagement::TaskInsertInfo &i1,
                                  const StnOrderManagement::TaskInsertInfo &i2) {
              return start_time_mapping[i1.task] < start_time_mapping[i2.task];
            });

  // calc new total metrics
  auto previous_total_metrics = current_total_metrics_;
  current_total_metrics_ = Metrics{};
  for (const auto &info : current_ordering_) {
    current_total_metrics_ = current_total_metrics_ + info.metrics_composition.getCurrentMetrics();
  }

  // set metric difference in new task
  auto find_new_task_lambda = [&](const StnOrderManagement::TaskInsertInfo &info) {
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

std::optional<std::pair<MetricsComposition, std::shared_ptr<StnOrderManagement::StnInsertionPoint>>>
StnOrderManagement::addBestOrdering(StnOrderManagement::TaskInsertInfo &task_insert_info) {
  auto insertion_points = calcInsertionPoints();

  int best_index = -1;
  std::optional<MetricsComposition> best_metrics = std::nullopt;

  for (auto i = 0; i < insertion_points.size(); i++) {
    const auto &point = insertion_points[i];

    StnOrderManagement copy(*this);
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
        std::make_shared<StnOrderManagement::StnInsertionPoint>(insertion_points[best_index]));
  }

  return std::nullopt;
}

void StnOrderManagement::addOrderingConstraintBetweenTasks(
    StnOrderManagement::StnInsertionPoint insertion_point,
    StnOrderManagement::TaskInsertInfo &task_insert_info) {
  current_ordering_.insert(current_ordering_.begin() + insertion_point.new_index, task_insert_info);

  auto start_vertex = getVertexOfOrder(task_insert_info.task.getOrders().front(), true);
  addBinaryConstraint(insertion_point.previous_finish, start_vertex, 0, std::nullopt);

  updateDurationConstraints(insertion_point.new_index);

  if (insertion_point.next_start.has_value()) {
    addBinaryConstraint(getVertexOfOrder(task_insert_info.task.getOrders().back(), false),
                        insertion_point.next_start.value(), 0, std::nullopt);

    updateDurationConstraints(insertion_point.new_index + 1);
  }
}

std::optional<Location> StnOrderManagement::getEndLocationOfOrder(const Order &order) {
  return std::visit(
      [&](auto &&arg) -> std::optional<Location> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, MoveOrder>) {
          return std::get<MoveOrder>(order).getMoveOrderStep().getLocation();
        } else if constexpr (std::is_same_v<T, TransportOrder>) {
          return std::get<TransportOrder>(order).getDeliveryTransportOrderStep().getLocation();
        } else if constexpr (std::is_same_v<T, ActionOrder>) {
          return std::nullopt;
        } else {
          static_assert(kAlwaysFalseV<T>, "Order type not handled");
        }
      },
      order);
}

daisi::util::Duration StnOrderManagement::calcOrderDurationForInsert(
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
    // only supported if this is not the first TransportOrder in the task

    if (order_index >= 1) {
      auto previous_location = task_insert_info.end_locations[order_index - 1];
      auto funcs = materialFlowToFunctionalities({order}, previous_location.getPosition());
      return AmrMobilityHelper::estimateDuration(daisi::util::Pose(), funcs, amr_description_,
                                                 topology_, false);
    }
  }

  // we dont know the location of the previous task yet
  // will be determined in the updateDurationConstraints method properly
  return std::numeric_limits<double>::max();
}

void StnOrderManagement::insertOrderPropertiesIntoMetrics(
    const Order &order, Metrics &metrics,
    const StnOrderManagement::TaskInsertInfo &task_insert_info, const int &task_ordering_index,
    const daisi::util::Duration &start_time) {
  if (!metrics.isStartTimeSet()) {
    metrics.setStartTime(start_time);
  }

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

void StnOrderManagement::updateDurationConstraints(const int &task_index_to_update) {
  auto last_position = getLastPositionBefore(task_index_to_update);

  const auto &task_info_to_update = current_ordering_[task_index_to_update];
  const auto &first_order = task_info_to_update.task.getOrders().front();

  if (!std::holds_alternative<TransportOrder>(first_order)) {
    throw std::invalid_argument("only supports transport orders at the beginngin of a task");
  }

  auto funcs = materialFlowToFunctionalities({first_order}, last_position);
  auto duration = AmrMobilityHelper::estimateDuration(daisi::util::Pose(last_position), funcs,
                                                      amr_description_, topology_, false);

  updateLastBinaryConstraint(getVertexOfOrder(task_info_to_update.task.getOrders().front(), true),
                             getVertexOfOrder(task_info_to_update.task.getOrders().front(), false),
                             duration, std::nullopt);
}

StnOrderManagement::VertexIterator StnOrderManagement::getVertexIteratorOfOrder(const Order &order,
                                                                                const bool start) {
  auto it = std::find_if(vertices_.begin(), vertices_.end(),
                         [&start, &order](const StnOrderManagementVertex &v) {
                           return v.isStart() == start && order == v.getOrder();
                         });

  if (it == vertices_.end()) {
    throw std::runtime_error("Order not part of STN");
  }

  return it;
}

int StnOrderManagement::getVertexIndexOfOrder(const Order &order, const bool start) {
  return std::distance(vertices_.begin(), getVertexIteratorOfOrder(order, start));
}

const StnOrderManagementVertex &StnOrderManagement::getVertexOfOrder(const Order &order,
                                                                     const bool start) {
  return *getVertexIteratorOfOrder(order, start);
}

std::pair<MetricsComposition, std::shared_ptr<AuctionBasedOrderManagement::InsertionPoint>>
StnOrderManagement::getLatestCalculatedInsertionInfo() const {
  if (latest_calculated_insertion_info_.has_value()) {
    return latest_calculated_insertion_info_.value();
  }
  throw std::logic_error(
      "There are no intertion info if the canAddTask or addTask were not sucessful");
}

}  // namespace daisi::cpps::logical
