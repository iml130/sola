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

#include "cpps/negotiation/utils/simple_temporal_network.h"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <string>

namespace daisi::cpps {

SimpleTemporalNetwork::SimpleTemporalNetwork(
    const std::shared_ptr<UtilityEvaluator> &utility_evaluator)
    : utility_evaluator_(utility_evaluator) {
  initOrigin();
}

SimpleTemporalNetwork::SimpleTemporalNetwork() { initOrigin(); }

void SimpleTemporalNetwork::initOrigin() {
  origin_.start = false;
  origin_.task_id = origin_id_;
  origin_.position = ns3::Vector(0, 0, 0);

  addVertex(origin_);
}

std::pair<UtilityDimensions, std::shared_ptr<STNInsertInfo>> SimpleTemporalNetwork::insertOrder(
    Task order) {
  STN_Vertex start_i;
  start_i.start = true;
  start_i.task_id = order.getUUID();
  start_i.position = order.getPickupLocation();
  addVertex(start_i);

  STN_Vertex finish_i;
  finish_i.start = false;
  finish_i.task_id = order.getUUID();
  finish_i.position = order.getDeliveryLocation();
  addVertex(finish_i);

  UtilityDimensions new_order_utility_dims(order);
  std::tie(new_order_utility_dims.execution_duration, new_order_utility_dims.execution_distance) =
      kinematics_->getStartStopTimeAndDistance(start_i.position, finish_i.position, true);
  new_order_utility_dims.setQueueSize(getNumberOfIncludedOrders());
  included_orders_.push_back({order, new_order_utility_dims});

  insertTimeWindowConstraints(start_i, finish_i, order);

  insertPrecedenceConstraints(start_i, order);

  return insertTravelConstraints(start_i, finish_i, order);
}

void SimpleTemporalNetwork::insertOrderWithTimeWindows(const Task &order) {
  STN_Vertex start_i;
  start_i.start = true;
  start_i.task_id = order.getUUID();
  start_i.position = order.getPickupLocation();
  addVertex(start_i);

  STN_Vertex finish_i;
  finish_i.start = false;
  finish_i.task_id = order.getUUID();
  finish_i.position = order.getDeliveryLocation();
  addVertex(finish_i);

  insertTimeWindowConstraints(start_i, finish_i, order);
}

void SimpleTemporalNetwork::insertPrecedenceConstraints(const STN_Vertex &start, Task order) {
  for (auto const &precedence_uuid : order.precedence_constraints.getConstraintUUIDs()) {
    // looking if the order with precedence_uuid is inserted in this STN
    std::string vertex_id = precedence_uuid;
    auto prec_finish_it = std::find_if(
        vertices_.begin(), vertices_.end(),
        [&vertex_id](const STN_Vertex &v) { return !v.start && v.task_id == vertex_id; });

    if (prec_finish_it != vertices_.end()) {
      addEdge(start, *prec_finish_it, STN_Edge{0});
    }
  }
}

std::pair<UtilityDimensions, std::shared_ptr<STNInsertInfo>>
SimpleTemporalNetwork::insertTravelConstraints(const STN_Vertex &start, const STN_Vertex &finish,
                                               Task &order) {
  // [finish_before, start_after]
  std::vector<std::pair<STN_Vertex, STN_Vertex>> insertion_points;

  if (!current_ordering_.empty()) {
    // very first
    std::string first_start_after_id = std::get<0>(current_ordering_[0]);
    auto first_start_after_it = std::find_if(vertices_.begin(), vertices_.end(),
                                             [&first_start_after_id](const STN_Vertex &v) {
                                               return v.start && v.task_id == first_start_after_id;
                                             });
    assert(first_start_after_it != vertices_.end());

    insertion_points.push_back({origin_, *first_start_after_it});

    // very last
    std::string last_finish_before_id = std::get<0>(current_ordering_.back());
    auto finish_before_it = std::find_if(vertices_.begin(), vertices_.end(),
                                         [&last_finish_before_id](const STN_Vertex &v) {
                                           return !v.start && v.task_id == last_finish_before_id;
                                         });
    assert(finish_before_it != vertices_.end());

    insertion_points.push_back({*finish_before_it, STN_Vertex{}});

  } else {
    insertion_points.push_back({origin_, STN_Vertex{}});
  }

  for (size_t i = 1; i < current_ordering_.size(); i++) {
    std::string finish_before_id = std::get<0>(current_ordering_[i - 1]);
    std::string start_after_id = std::get<0>(current_ordering_[i]);

    auto finish_before_it =
        std::find_if(vertices_.begin(), vertices_.end(), [&finish_before_id](const STN_Vertex &v) {
          return !v.start && v.task_id == finish_before_id;
        });
    auto start_after_it = std::find_if(
        vertices_.begin(), vertices_.end(),
        [&start_after_id](const STN_Vertex &v) { return v.start && v.task_id == start_after_id; });

    assert(finish_before_it != vertices_.end());
    assert(start_after_it != vertices_.end());

    insertion_points.push_back({*finish_before_it, *start_after_it});
  }

  // checking each insertion point
  bool valid_found = false;
  UtilityDimensions best_udims = UtilityDimensions::createInvalid();
  auto best_insert_info = std::make_shared<STNInsertInfo>();

  for (auto const &[finish_before, start_after] : insertion_points) {
    insertTravelConstraintsBetween(finish_before, start_after, start, finish, order);

    generateDGraph();
    if (isConsistent()) {
      UtilityDimensions dims = calcUtilityDimensions(order);

      if (dims.getUtility() > best_udims.getUtility()) {
        valid_found = true;
        best_udims = dims;
        best_insert_info->finish_before = finish_before;
        best_insert_info->start_after = start_after;
        best_insert_info->required_finish_time = dims.getFinishTime();
        best_insert_info->ordering_and_start_times = calcOrderingAndStartTimes();
      }
    }

    removeTravelConstraintsBetween(finish_before, start_after, start, finish);
  }

  // insert best and return

  if (valid_found) {
    insertTravelConstraintsBetween(best_insert_info->finish_before, best_insert_info->start_after,
                                   start, finish, order);
    updateUtilityDimensions(best_insert_info);
    generateDGraph();
  }

  return {best_udims, best_insert_info};
}

bool SimpleTemporalNetwork::insertTravelConstraintsBetween(const STN_Vertex &finish_before,
                                                           const STN_Vertex &start_after,
                                                           const STN_Vertex &start,
                                                           const STN_Vertex &finish, Task &order) {
  // before_finish belongs to t1
  // after_start belongs to t2
  // order creates new t3

  bool finish_exists =
      std::find(vertices_.begin(), vertices_.end(), finish_before) != vertices_.end();

  if (!(finish_exists || (included_orders_.size() == 1))) {
    return false;
  }

  // if(!finish_exists) {
  //   if(included_orders_.size() != 1) {
  //     return false;
  //   }
  // }

  // travel constraint between F(t1) and S(t3)
  // F(t1) -> S(t3) : [TT(t1-finish, t3-start), inf)
  // edge S(t3) -> F(t1): weight ( - TT(t1-finish, t3-start) )
  if (finish_exists) {
    double travel_time_previous_to_start = std::get<0>(
        kinematics_->getStartStopTimeAndDistance(finish_before.position, start.position, false));
    addEdge(start, finish_before, STN_Edge{-travel_time_previous_to_start});
  }

  // travel constraint between F(t3) and S(t2)
  // F(t3) -> S(t2) : [TT(t3-finish, t2-start), inf)
  // edge S(t2) -> F(t3): weight ( - TT(t3-finish, t2-start) )
  if (!start_after.task_id.empty()) {
    double travel_time_finish_to_next = std::get<0>(
        kinematics_->getStartStopTimeAndDistance(finish.position, start_after.position, false));

    bool start_exists =
        std::find(vertices_.begin(), vertices_.end(), start_after) != vertices_.end();
    bool end_exists = std::find(vertices_.begin(), vertices_.end(), finish) != vertices_.end();
    if (!start_exists || !end_exists) {
      return false;
    }

    addEdge(start_after, finish, STN_Edge{-travel_time_finish_to_next});
  }

  return true;
}

void SimpleTemporalNetwork::removeTravelConstraintsBetween(const STN_Vertex &finish_before,
                                                           const STN_Vertex &start_after,
                                                           const STN_Vertex &start,
                                                           const STN_Vertex &finish) {
  auto &before_edges = adjacency_lists_[start][finish_before];
  before_edges.pop_back();

  if (!start_after.task_id.empty()) {
    auto &after_edges = adjacency_lists_[start_after][finish];
    after_edges.pop_back();
  }
}

void SimpleTemporalNetwork::insertTimeWindowConstraints(
    const STN_Vertex &start, const STN_Vertex &finish, Task order,
    std::optional<double> required_finish_time) {
  double execution_duration;
  if (order.hasSpecificAbilityRequirement()) {
    execution_duration = order.getDuration(true);
  } else {
    execution_duration = order.getFastestDuration(true);
  }

  if (order.time_window.getDuration() < 0.01) {
    order.time_window.setDuration(execution_duration);
  } else {
    assert(order.time_window.getDuration() + 2 >= execution_duration);
  }
  double duration = order.time_window.getDuration();

  if (required_finish_time) {
    order.time_window.constraintLatestFinish(*required_finish_time);
  }
  order.time_window.constraintEarliestStart(
      order.precedence_constraints.getEarliestValidStartTime());

  // [earliest start, latest start] - X0
  // origin -> start : + ( latest start - X0 )
  // start -> origin : - ( earliest start - X0 )

  double latest_start_constraint = order.time_window.getLatestStart() - origin_.value;
  double earliest_start_constraint = order.time_window.getEarliestStart() - origin_.value;

  addEdge(origin_, start, STN_Edge{latest_start_constraint});
  addEdge(start, origin_, STN_Edge{-earliest_start_constraint});

  // [earliest finish, latest finish] - X0
  // origin -> finish : + (latest finish - X0 )
  // finish -> origin : - (earliest start - X0)

  double latest_finish_constraint = order.time_window.getLatestFinish() - origin_.value;
  double earliest_finish_constraint = order.time_window.getEarliestFinish() - origin_.value;

  addEdge(origin_, finish, STN_Edge{latest_finish_constraint});
  addEdge(finish, origin_, STN_Edge{-earliest_finish_constraint});

  // [duration, inf)
  // finish -> start : - duration
  addEdge(finish, start, STN_Edge{-duration});
}

bool SimpleTemporalNetwork::insertOrderByInsertInfo(
    Task order, const std::shared_ptr<STNInsertInfo> &insert_info) {
  STN_Vertex start_i;
  start_i.start = true;
  start_i.task_id = order.getUUID();
  start_i.position = order.getPickupLocation();
  addVertex(start_i);

  STN_Vertex finish_i;
  finish_i.start = false;
  finish_i.task_id = order.getUUID();
  finish_i.position = order.getDeliveryLocation();
  addVertex(finish_i);

  UtilityDimensions new_order_utility_dims(order);
  std::tie(new_order_utility_dims.execution_duration, new_order_utility_dims.execution_distance) =
      kinematics_->getStartStopTimeAndDistance(start_i.position, finish_i.position, true);
  new_order_utility_dims.setQueueSize(getNumberOfIncludedOrders());
  included_orders_.push_back({order, new_order_utility_dims});

  insertTimeWindowConstraints(start_i, finish_i, order, insert_info->required_finish_time);

  insertPrecedenceConstraints(start_i, order);

  bool success = insertTravelConstraintsBetween(insert_info->finish_before,
                                                insert_info->start_after, start_i, finish_i, order);

  if (success) {
    generateDGraph();
    if (isConsistent()) {
      updateUtilityDimensions(insert_info);
      return true;
    }
  }

  return false;
}

std::vector<STN_Vertex> SimpleTemporalNetwork::getStartVertices() const {
  std::vector<STN_Vertex> start_vertices;
  std::copy_if(vertices_.begin(), vertices_.end(), std::back_inserter(start_vertices),
               [](const STN_Vertex &v) { return v.start; });
  return start_vertices;
}

std::vector<STN_Vertex> SimpleTemporalNetwork::getFinishVertices() const {
  std::vector<STN_Vertex> finish_vertices;
  std::copy_if(vertices_.begin(), vertices_.end(), std::back_inserter(finish_vertices),
               [&](const STN_Vertex &v) { return !v.start && v.task_id != origin_id_; });
  return finish_vertices;
}

void SimpleTemporalNetwork::generateDGraph() { d_graph_ = this->floydWarshall(); }

bool SimpleTemporalNetwork::isConsistent() {
  assert(vertices_.size() == d_graph_.size());

  for (size_t i = 0; i < vertices_.size(); i++) {
    if (!isZero(d_graph_[i][i])) {
      return false;
    }
  }

  return true;
}

std::vector<std::pair<std::string, double>> SimpleTemporalNetwork::calcOrderingAndStartTimes() {
  std::vector<std::pair<std::string, double>> ordering_and_start_times;
  for (size_t i = 0; i < vertices_.size(); i++) {
    auto vertex = vertices_[i];
    if (!vertex.start) {
      continue;
    }

    double start_time = -d_graph_[i][0] + origin_.value;
    ordering_and_start_times.push_back({vertex.task_id, start_time});
  }
  std::sort(ordering_and_start_times.begin(), ordering_and_start_times.end(),
            [](const auto &t1, const auto &t2) { return std::get<1>(t1) < std::get<1>(t2); });

  return ordering_and_start_times;
}

UtilityDimensions SimpleTemporalNetwork::calcUtilityDimensions(const Task &order) {
  std::vector<std::pair<std::string, double>> ordering_and_start_times =
      calcOrderingAndStartTimes();

  UtilityDimensions new_order_utility_dims(order);
  auto previous_finish_it =
      std::find_if(vertices_.begin(), vertices_.end(),
                   [&](const STN_Vertex &v) { return v.task_id == origin_id_; });

  for (auto const &[task_id, start_time] : ordering_and_start_times) {
    std::string vertex_id = task_id;
    auto it = std::find_if(
        included_orders_.begin(), included_orders_.end(),
        [vertex_id](const auto &pair) { return std::get<0>(pair).getUUID() == vertex_id; });
    UtilityDimensions dims = std::get<1>(*it);

    auto start_it = std::find_if(
        vertices_.begin(), vertices_.end(),
        [&vertex_id](const STN_Vertex &v) { return v.start && v.task_id == vertex_id; });
    auto finish_it = std::find_if(
        vertices_.begin(), vertices_.end(),
        [&vertex_id](const STN_Vertex &v) { return !v.start && v.task_id == vertex_id; });

    std::tie(dims.travel_to_pickup_duration, dims.travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(previous_finish_it->position, start_it->position,
                                                 false);
    std::tie(dims.execution_duration, dims.execution_distance) =
        kinematics_->getStartStopTimeAndDistance(start_it->position, finish_it->position, true);
    dims.start_time = start_time;

    if (task_id == order.getUUID()) {
      dims.makespan = getMakespan();
      utility_evaluator_->evaluate(dims);
      new_order_utility_dims = dims;
    } else {
      utility_evaluator_->reevaluate(dims);
    }

    previous_finish_it = finish_it;
  }

  return new_order_utility_dims;
}

std::pair<Task, UtilityDimensions> SimpleTemporalNetwork::removeOrder(
    const std::string &order_uuid) {
  auto order_it =
      std::find_if(included_orders_.begin(), included_orders_.end(),
                   [&](const auto &pair) { return std::get<0>(pair).getUUID() == order_uuid; });

  auto start_it = std::find_if(
      vertices_.begin(), vertices_.end(),
      [order_uuid](const STN_Vertex &v) { return v.start && v.task_id == order_uuid; });
  removeVertex(*start_it);  // remove start

  auto finish_it = std::find_if(
      vertices_.begin(), vertices_.end(),
      [order_uuid](const STN_Vertex &v) { return !v.start && v.task_id == order_uuid; });
  removeVertex(*finish_it);  // remove end

  utility_evaluator_->evaluate(std::get<1>(*order_it));
  std::pair<Task, UtilityDimensions> removed_task_info = *order_it;
  included_orders_.erase(order_it);

  removeOrderUUIDFromOrdering(order_uuid);

  return removed_task_info;
}

void SimpleTemporalNetwork::setOriginInformation(const ns3::Vector &position, const double &time) {
  double offset_difference = time - origin_.value;
  origin_.value = time;
  origin_.position = position;

  assert(vertices_[0].task_id == origin_id_);
  vertices_[0] = origin_;

  updateOriginEdges(offset_difference);
}

void SimpleTemporalNetwork::updateOriginEdges(const double &offset_difference) {
  for (const auto &vertex : vertices_) {
    if (vertex.task_id == origin_id_) {
      continue;
    }

    std::vector<STN_Edge> &incoming_edges = adjacency_lists_[vertex][origin_];
    assert(!incoming_edges.empty());

    // incoming edges are
    // [0] origin <---- ( - earliest start/finish ) ---- vertex
    // [1] origin <---- ( - travel constraint ) ---- vertex

    // update [0] with offset_difference
    // update [1] with travel time of new position

    incoming_edges[0].weight += offset_difference;
  }

  for (auto &[vertex_id, outgoing_edges] : adjacency_lists_[origin_]) {
    assert(outgoing_edges.size() == 1);

    // outgoing edges are
    // [0] origin ---- ( + latest start/finish ) ----> vertex
    // update [0] with offset_difference

    outgoing_edges[0].weight -= offset_difference;
  }
}

void SimpleTemporalNetwork::updateUtilityDimensions(
    const std::shared_ptr<STNInsertInfo> &insert_info) {
  auto previous_finish_it =
      std::find_if(vertices_.begin(), vertices_.end(),
                   [&](const STN_Vertex &v) { return v.task_id == "origin"; });

  current_ordering_ = insert_info->ordering_and_start_times;
  for (auto const &[task_id, start_time] : insert_info->ordering_and_start_times) {
    std::string current_task_id = task_id;

    auto included_order_it = std::find_if(included_orders_.begin(), included_orders_.end(),
                                          [current_task_id](const auto &pair) {
                                            return std::get<0>(pair).getUUID() == current_task_id;
                                          });

    if (included_order_it == included_orders_.end()) {
      if (std::get<0>(insert_info->ordering_and_start_times[0]) == current_task_id) {
        current_ordering_.erase(current_ordering_.begin());
        continue;
      }
      throw std::runtime_error("Insert Info invalidly outdated");
    }

    auto start_it = std::find_if(
        vertices_.begin(), vertices_.end(),
        [current_task_id](const STN_Vertex &v) { return v.start && v.task_id == current_task_id; });
    auto finish_it =
        std::find_if(vertices_.begin(), vertices_.end(), [current_task_id](const STN_Vertex &v) {
          return !v.start && v.task_id == current_task_id;
        });

    std::tie(std::get<1>(*included_order_it).travel_to_pickup_duration,
             std::get<1>(*included_order_it).travel_to_pickup_distance) =
        kinematics_->getStartStopTimeAndDistance(previous_finish_it->position, start_it->position,
                                                 false);
    std::tie(std::get<1>(*included_order_it).execution_duration,
             std::get<1>(*included_order_it).execution_distance) =
        kinematics_->getStartStopTimeAndDistance(start_it->position, finish_it->position, true);
    std::get<1>(*included_order_it).start_time = start_time;
    std::get<1>(*included_order_it).makespan = getMakespan();

    utility_evaluator_->reevaluate(std::get<1>(*included_order_it));

    previous_finish_it = finish_it;
  }

  std::get<1>(included_orders_.back()).makespan = getMakespan();
}

std::string SimpleTemporalNetwork::getNextOrderUUID() const {
  return std::get<0>(current_ordering_.front());
}

void SimpleTemporalNetwork::removeOrderUUIDFromOrdering(const std::string &uuid) {
  current_ordering_.erase(
      std::remove_if(current_ordering_.begin(), current_ordering_.end(),
                     [uuid](const auto &tupl) { return std::get<0>(tupl) == uuid; }),
      current_ordering_.end());
}

Task SimpleTemporalNetwork::getNextOrder() const {
  std::string uuid = std::get<0>(current_ordering_.front());

  auto order_it =
      std::find_if(included_orders_.begin(), included_orders_.end(),
                   [uuid](const auto &pair) { return std::get<0>(pair).getUUID() == uuid; });

  return std::get<0>(*order_it);
}

int SimpleTemporalNetwork::getNumberOfIncludedOrders() const { return included_orders_.size(); }

std::vector<std::tuple<std::string, double>>
SimpleTemporalNetwork::generateEarliestStartSolution() {
  std::vector<std::tuple<std::string, double>> solution;
  for (size_t i = 0; i < vertices_.size(); i++) {
    auto vertex = vertices_[i];
    if (!vertex.start) {
      continue;
    }

    double start_time = -d_graph_[i][0];
    solution.push_back({vertex.task_id, start_time});
  }

  return solution;
}
std::vector<std::tuple<std::string, double>>
SimpleTemporalNetwork::generateEarliestFinishSolution() {
  std::vector<std::tuple<std::string, double>> solution;
  for (size_t i = 0; i < vertices_.size(); i++) {
    auto vertex = vertices_[i];
    if (vertex.start) {
      continue;
    }

    double start_time = -d_graph_[i][0];
    solution.push_back({vertex.task_id, start_time});
  }

  return solution;
}

std::vector<std::tuple<std::string, double>> SimpleTemporalNetwork::generateLatestFinishSolution() {
  std::vector<std::tuple<std::string, double>> solution;
  for (size_t i = 0; i < vertices_.size(); i++) {
    auto vertex = vertices_[i];
    if (vertex.start || vertex.task_id == origin_id_) {
      continue;
    }

    double start_time = d_graph_[0][i];
    solution.push_back({vertex.task_id, start_time});
  }

  return solution;
}

void SimpleTemporalNetwork::insertPrecedenceConstraintsOnPath(const std::vector<Task> &path) {
  for (size_t i = 1; i < path.size(); i++) {
    const Task &order1 = path[i - 1];
    const Task &order2 = path[i];

    auto order1_finish = std::find_if(
        vertices_.begin(), vertices_.end(),
        [&order1](const STN_Vertex &v) { return !v.start && v.task_id == order1.getUUID(); });
    auto order2_start = std::find_if(
        vertices_.begin(), vertices_.end(),
        [&order2](const STN_Vertex &v) { return v.start && v.task_id == order2.getUUID(); });

    assert(order1_finish != vertices_.end());
    assert(order2_start != vertices_.end());

    addEdge(*order2_start, *order1_finish, STN_Edge{0});
  }
}

void SimpleTemporalNetwork::setKinematics(const Kinematics &kinematics) {
  kinematics_ = kinematics;
}

AccumulatedUtilityDimensions SimpleTemporalNetwork::getCurrentAccumulatedUtilityDimensions() const {
  AccumulatedUtilityDimensions acc_udims;

  for (auto const &[order, udims] : included_orders_) {
    acc_udims.addToHistory(udims);
  }

  return acc_udims;
}

void SimpleTemporalNetwork::setNewestUtilityDimensionDelta(
    const AccumulatedUtilityDimensions &delta) {
  std::get<1>(included_orders_.back()).delta_execution_distance =
      delta.getAccumulatedExecutionDistance();
  std::get<1>(included_orders_.back()).delta_execution_duration =
      delta.getAccumulatedExecutionDuration();
  std::get<1>(included_orders_.back()).delta_travel_to_pickup_distance =
      delta.getAccumulatedTravelToPickupDistance();
  std::get<1>(included_orders_.back()).delta_travel_to_pickup_duration =
      delta.getAccumulatedTravelToPickupDuration();
  std::get<1>(included_orders_.back()).delta_makespan = delta.getMakespan();

  utility_evaluator_->reevaluate(std::get<1>(included_orders_.back()));
}

double SimpleTemporalNetwork::getMakespan() {
  std::vector<std::tuple<std::string, double>> solution = generateEarliestFinishSolution();
  std::vector<double> finish_times;
  std::transform(solution.begin(), solution.end(), std::back_inserter(finish_times),
                 [](const auto &tupl) { return std::get<1>(tupl); });

  return *std::max_element(finish_times.begin(), finish_times.end()) + origin_.value;
}

bool SimpleTemporalNetwork::hasOrder(const std::string &order_uuid) {
  for (auto const &[order, udim] : included_orders_) {
    if (order.getUUID() == order_uuid) {
      return true;
    }
  }
  return false;
}

}  // namespace daisi::cpps
