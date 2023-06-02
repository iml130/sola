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

#include "cpps/negotiation/utils/precedence_graph.h"

#include <algorithm>
#include <limits>
#include <numeric>
#include <utility>

#include "cpps/model/agv_fleet.h"
#include "cpps/negotiation/utils/simple_temporal_network.h"

namespace daisi::cpps {

PrecedenceGraph::PrecedenceGraph(){};

void PrecedenceGraph::tightenTimeWindows2() {
  setAvgOrderDurations();
  std::unordered_map<int, std::vector<Task>> layers = calcPrecedenceLayers();

  bool changes = true;
  while (changes) {
    changes = false;
    int number = 0;
    while (layers.find(number) != layers.end()) {
      auto current_orders = layers[number];
      for (auto const &order : current_orders) {
        // might have updated time windows
        auto parents = simple_inverted_adjacency_list_[order];
        auto order_it = std::find(vertices_.begin(), vertices_.end(), order);

        for (auto const &parent : parents) {
          auto parent_it = std::find(vertices_.begin(), vertices_.end(), parent);
          if (order_it->time_window.getLatestStart() < parent_it->time_window.getLatestFinish()) {
            parent_it->time_window.constraintLatestFinish(order_it->time_window.getLatestStart());
            changes = true;
          }
        }

        for (auto const &parent : parents) {
          auto parent_it = std::find(vertices_.begin(), vertices_.end(), parent);
          if (order_it->time_window.getEarliestStart() <
              parent_it->time_window.getEarliestFinish()) {
            order_it->time_window.constraintEarliestStart(
                parent_it->time_window.getEarliestFinish());
            changes = true;
          }
        }
      }

      number++;
    }
  }
}

void PrecedenceGraph::tightenTimeWindows() {
  setAvgOrderDurations();

  std::vector<std::vector<Task>> all_paths = generatePaths();
  std::unordered_map<std::string, std::tuple<double, double>> start_window;

  for (auto const &order : vertices_) {
    start_window[order.getUUID()] = {std::numeric_limits<double>::lowest(),
                                     std::numeric_limits<double>::max()};
  }

  for (auto &path : all_paths) {
    std::reverse(path.begin(), path.end());

    SimpleTemporalNetwork temp_stn;
    for (auto const &node : path) {
      temp_stn.insertOrderWithTimeWindows(node);
    }
    temp_stn.insertPrecedenceConstraintsOnPath(path);

    temp_stn.generateDGraph();
    if (!temp_stn.isConsistent()) {
      throw std::runtime_error("Path is not schedulable");
    }

    std::vector<std::tuple<std::string, double>> earliest_start_solution =
        temp_stn.generateEarliestStartSolution();
    std::vector<std::tuple<std::string, double>> latest_finish_solution =
        temp_stn.generateLatestFinishSolution();

    for (auto const &[uuid, earliest_start] : earliest_start_solution) {
      std::get<0>(start_window[uuid]) = std::max(std::get<0>(start_window[uuid]), earliest_start);
    }

    for (auto const &[uuid, latest_finish] : latest_finish_solution) {
      std::get<1>(start_window[uuid]) = std::min(std::get<1>(start_window[uuid]), latest_finish);
    }
  }

  for (auto &order : vertices_) {
    auto const &[tighter_es, tighter_lf] = start_window[order.getUUID()];
    assert(order.time_window.getEarliestStart() <= tighter_es);
    assert(order.time_window.getLatestFinish() >= tighter_lf);

    order.time_window.constraintEarliestStart(tighter_es);
    order.time_window.constraintLatestFinish(tighter_lf);
  }
}

std::unordered_map<int, std::vector<Task>> PrecedenceGraph::calcPrecedenceLayers() {
  assert(!simple_adjacency_list_.empty());
  assert(!simple_inverted_adjacency_list_.empty());
  std::unordered_map<Task, int> order_number_map;

  for (auto const &order : vertices_) {
    order_number_map[order] = -1;
  }

  // layer 0 -> no incoming edges
  std::vector<Task> current_layer;
  int current_layer_number = 0;
  for (auto const &[order, incoming_list] : simple_inverted_adjacency_list_) {
    if (incoming_list.empty()) {
      order_number_map[order] = current_layer_number;
      current_layer.push_back(order);
    }
  }

  while (!current_layer.empty()) {
    current_layer_number++;

    std::vector<Task> previous_layer = current_layer;
    current_layer.clear();

    for (auto const &order : previous_layer) {
      const auto &children = simple_adjacency_list_[order];
      for (auto const &child : children) {
        if (order_number_map[child] < current_layer_number) {
          order_number_map[child] = current_layer_number;
          current_layer.push_back(child);
        }
      }
    }

    std::sort(current_layer.begin(), current_layer.end());
    current_layer.erase(std::unique(current_layer.begin(), current_layer.end()),
                        current_layer.end());
  }

  int max_number = current_layer_number - 1;

  // transform [Task -> int] to [int -> {Task}]
  std::unordered_map<int, std::vector<Task>> layers;
  for (auto const &[order, number] : order_number_map) {
    int new_number = max_number - number;
    layers[new_number].push_back(order);
  }

  return layers;
}

void PrecedenceGraph::setMaxOrderDurations() {
  auto durations = getMaxOrderDurations();

  for (auto &order : vertices_) {
    order.time_window.setDuration(durations[order]);

    if (!order.time_window.isValid()) {
      throw std::runtime_error("Task TimeWindow is too short");
    }
  }
}

void PrecedenceGraph::setAvgOrderDurations() {
  auto durations = getAvgOrderDurations();

  for (auto &order : vertices_) {
    order.time_window.setDuration(durations[order]);

    if (!order.time_window.isValid()) {
      throw std::runtime_error("Task TimeWindow is too short");
    }
  }
}

std::unordered_map<Task, double> PrecedenceGraph::getMaxOrderDurations() {
  std::unordered_map<Task, double> durations;

  for (auto &order : vertices_) {
    std::vector<double> durs;

    for (const auto &fit :
         AGVFleet::get().getFittingExistingAbilities(order.getAbilityRequirement())) {
      const Kinematics kinematics = AGVFleet::get().getKinematicsOfAbility(fit);
      auto const &[duration, _] = kinematics.getStartStopTimeAndDistance(
          order.getPickupLocation(), order.getDeliveryLocation(), true);

      durs.push_back(duration);
    }

    durations[order] = *std::max_element(durs.begin(), durs.end());
  }

  return durations;
}

std::unordered_map<Task, double> PrecedenceGraph::getAvgOrderDurations() {
  std::unordered_map<Task, double> durations;

  for (auto &order : vertices_) {
    std::vector<double> durs;

    for (const auto &fit :
         AGVFleet::get().getFittingExistingAbilities(order.getAbilityRequirement())) {
      const Kinematics kinematics = AGVFleet::get().getKinematicsOfAbility(fit);
      auto const &[duration, _] = kinematics.getStartStopTimeAndDistance(
          order.getPickupLocation(), order.getDeliveryLocation(), true);

      durs.push_back(duration);
    }

    durations[order] = std::accumulate(durs.begin(), durs.end(), 0) / durs.size();
  }

  return durations;
}

void PrecedenceGraph::addEdge(const Task &start, const Task &end) {
  GEdge edge;
  DirectedGraph<Task, GEdge>::addEdge(start, end, edge);
}

std::unordered_map<Task, std::vector<Task>> &PrecedenceGraph::getSimpleInvertedAdjacencyList() {
  return simple_inverted_adjacency_list_;
}

std::unordered_map<Task, std::vector<Task>> &PrecedenceGraph::getSimpleAdjacencyList() {
  return simple_adjacency_list_;
}

std::vector<Task> PrecedenceGraph::getChildren(const Task &order) {
  return simple_adjacency_list_[order];
}

std::vector<Task> PrecedenceGraph::getParents(const Task &order) {
  return simple_inverted_adjacency_list_[order];
}

void PrecedenceGraph::generateSimpleAdjacencyList() {
  simple_adjacency_list_.clear();
  for (auto const &[order, outgoing_edges] : adjacency_lists_) {
    for (auto const &[neighbor, _] : outgoing_edges) {
      auto neighbor_it = std::find(vertices_.begin(), vertices_.end(), neighbor);
      simple_adjacency_list_[order].push_back(*neighbor_it);
    }
    std::sort(simple_adjacency_list_[order].begin(), simple_adjacency_list_[order].end());
  }
}

void PrecedenceGraph::generateSimpleInvertedAdjacencyList() {
  simple_inverted_adjacency_list_.clear();
  for (auto const &[order, outgoing_edges] : adjacency_lists_) {
    simple_inverted_adjacency_list_[order];
    auto order_it = std::find(vertices_.begin(), vertices_.end(), order);
    for (auto const &[neighbor, _] : outgoing_edges) {
      simple_inverted_adjacency_list_[neighbor].push_back(*order_it);
    }
  }

  for (auto const &[order, _] : adjacency_lists_) {
    std::sort(simple_inverted_adjacency_list_[order].begin(),
              simple_inverted_adjacency_list_[order].end());
  }
}

Task PrecedenceGraph::getUpToDateOrder(const Task &order) {
  auto order_it = std::find(vertices_.begin(), vertices_.end(), order);
  return *order_it;
}

std::vector<std::vector<Task>> PrecedenceGraph::generatePaths() {
  std::vector<std::vector<Task>> paths;

  for (auto const &[order_id, incoming_edges] : simple_inverted_adjacency_list_) {
    if (incoming_edges.empty()) {
      auto order = std::find(vertices_.begin(), vertices_.end(), order_id);
      std::vector<std::vector<Task>> order_paths = generatePaths(*order);
      paths.insert(paths.begin(), order_paths.begin(), order_paths.end());
    }
  }

  return paths;
}

std::vector<std::vector<Task>> PrecedenceGraph::generatePaths(const Task &starting_node) {
  // recursion stop
  if (simple_adjacency_list_[starting_node].empty()) {
    return {{starting_node}};
  }

  std::vector<std::vector<Task>> paths;

  for (auto const &child_id : simple_adjacency_list_[starting_node]) {
    auto child = std::find(vertices_.begin(), vertices_.end(), child_id);
    std::vector<std::vector<Task>> child_paths = generatePaths(*child);
    for (auto &child_path : child_paths) {
      child_path.push_back(starting_node);
      paths.push_back(child_path);
    }
  }

  return paths;
}

}  // namespace daisi::cpps
