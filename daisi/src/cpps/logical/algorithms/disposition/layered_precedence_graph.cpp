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

#include "layered_precedence_graph.h"

#include <algorithm>

namespace daisi::cpps::logical {

LayeredPrecedenceGraph::LayeredPrecedenceGraph(
    std::shared_ptr<daisi::material_flow::MFDLScheduler> scheduler) {
  // TODO transform scheduler content to vertices and edges

  // hard coded test tasks
  {
    material_flow::TransportOrderStep pickup1(
        "tos11", {}, material_flow::Location("0x0", "type", util::Position(10, 10)));
    material_flow::TransportOrderStep delivery1(
        "tos12", {}, material_flow::Location("0x0", "type", util::Position(10, 20)));
    material_flow::TransportOrder to1("to1", {pickup1}, delivery1);
    material_flow::Task task1("task1", {to1}, {});

    material_flow::TransportOrderStep pickup2(
        "tos21", {}, material_flow::Location("0x0", "type", util::Position(20, 10)));
    material_flow::TransportOrderStep delivery2(
        "tos22", {}, material_flow::Location("0x0", "type", util::Position(10, 20)));
    material_flow::TransportOrder to2("to2", {pickup2}, delivery2);
    material_flow::Task task2("task2", {to2}, {});

    material_flow::TransportOrderStep pickup3(
        "tos31", {}, material_flow::Location("0x0", "type", util::Position(10, 20)));
    material_flow::TransportOrderStep delivery3(
        "tos32", {}, material_flow::Location("0x0", "type", util::Position(5, 5)));
    material_flow::TransportOrder to3("to3", {pickup3}, delivery3);
    material_flow::Task task3("task3", {to3}, {});

    amr::AmrStaticAbility ability1(amr::LoadCarrier(amr::LoadCarrier::Types::kPackage), 20);
    amr::AmrStaticAbility ability2(amr::LoadCarrier(amr::LoadCarrier::Types::kEuroBox), 20);

    task1.setAbilityRequirement(ability1);
    task2.setAbilityRequirement(ability2);
    task2.setAbilityRequirement(ability1);

    LPCVertex v1(task1);
    LPCVertex v2(task2);
    LPCVertex v3(task3);

    addVertex(v1);
    addVertex(v2);
    addVertex(v3);

    addEdge(v1, v3, std::monostate());
    addEdge(v2, v3, std::monostate());
  }

  initLayers();
}

void LayeredPrecedenceGraph::initLayers() {
  // free layer = no incoming edges
  // T_F = { t_k in T | there exists no t_j in T : (t_j, t_k) in E }
  std::vector<LPCVertex> temp_free_layer_vertices;
  for (auto &vertex : vertices_) {
    auto incoming_edges = getIncomingEdges(vertex);
    if (incoming_edges.empty()) {
      vertex.layer = PrecedenceGraphLayer::kFree;
      temp_free_layer_vertices.push_back(vertex);
    }
  }

  // second layer = neighbors of free layer
  // T_L = { t_k in T | there exists t_j in T_F : (t_j, t_k) in E }
  std::vector<LPCVertex> temp_second_layer_vertices;
  for (const auto &free_vertex : temp_free_layer_vertices) {
    for (auto &vertex : vertices_) {
      if (free_vertex != vertex) {
        if (this->hasEdge(free_vertex, vertex)) {
          if (vertex.layer == PrecedenceGraphLayer::kFree) {
            throw std::runtime_error("Conceptionally, the algorithm should not allow this state.");
          }

          if (vertex.layer != PrecedenceGraphLayer::kSecond) {
            vertex.layer = PrecedenceGraphLayer::kSecond;
            temp_second_layer_vertices.push_back(vertex);
          }
        }
      }
    }
  }

  // remaining vertices are in hidden layer
  // T_H = T - (T_F union T_L)
  for (auto &vertex : vertices_) {
    if (vertex.layer != PrecedenceGraphLayer::kFree &&
        vertex.layer != PrecedenceGraphLayer::kSecond) {
      vertex.layer = PrecedenceGraphLayer::kHidden;
    }
  }
}

/// @brief Implements pseudocode of Algorithm 2 "UpdatePrecGraph" from pIA paper
/// All free tasks are scheduled.
void LayeredPrecedenceGraph::next() {
  std::vector<LPCVertex> temp_previously_free_layer;

  // 1: Move t: T_F -> T_S
  // all free tasks are scheduled now
  for (auto &vertex : vertices_) {
    if (vertex.layer == PrecedenceGraphLayer::kFree) {
      vertex.layer = PrecedenceGraphLayer::kScheduled;
      temp_previously_free_layer.push_back(vertex);
    }
  }

  // implicitly: for all t in T_auct (=T_F) do: UpdatePrecGraph(t)
  for (const auto &t : temp_previously_free_layer) {
    updateLayersSecondToFree(t);
  }
}

void LayeredPrecedenceGraph::updateLayersSecondToFree(const LPCVertex &t) {
  // 2: for all t' in (T_L intersection children(t)) do
  // if a child of a free task is in the second layer, it could move to the free layer next

  for (auto &t_dash : vertices_) {
    if (t_dash.layer == PrecedenceGraphLayer::kSecond && this->hasEdge(t, t_dash)) {
      auto parents_of_t_dash = this->getIncomingEdges(t_dash);
      bool parents_of_t_dash_are_subset = std::all_of(
          parents_of_t_dash.begin(), parents_of_t_dash.end(), [](const auto &parent_and_edge) {
            return parent_and_edge.first.layer == PrecedenceGraphLayer::kScheduled;
          });

      // 3: if parents(t') subset of T_S then
      // if all parents of a task are scheduled, the task can be auctioned next
      if (parents_of_t_dash_are_subset) {
        // 4: Move t': T_L -> T_F
        t_dash.layer = PrecedenceGraphLayer::kFree;

        // 5: PC[t'] = max_{t'' in parents(t')} (F[t''])
        // PC represents the earliest valid start time for a task

        std::vector<daisi::util::Duration> parent_finish_times;
        std::transform(parents_of_t_dash.begin(), parents_of_t_dash.end(),
                       std::back_inserter(parent_finish_times), [](const auto &parent_and_edge) {
                         return parent_and_edge.first.latest_finish.value();
                       });

        t_dash.earliest_valid_start =
            *std::max_element(parent_finish_times.begin(), parent_finish_times.end());

        updateLayersHiddenToSecond(t_dash);
      }
    }
  }
}

void LayeredPrecedenceGraph::updateLayersHiddenToSecond(const LPCVertex &t_dash) {
  for (auto &t_dash_dash : vertices_) {
    // 6: for all t' in (T_H intersection children(t')) do
    // t' is free now, therefore children from hidden layer can go to the second layer
    if (t_dash_dash.layer == PrecedenceGraphLayer::kHidden && this->hasEdge(t_dash, t_dash_dash)) {
      // 7: if parents(t'') subset (T_S union T_F) then
      auto parents_of_t_dash_dash = this->getIncomingEdges(t_dash_dash);
      bool parents_of_t_dash_dash_are_subset =
          std::all_of(parents_of_t_dash_dash.begin(), parents_of_t_dash_dash.end(),
                      [](const auto &parent_and_edge) {
                        return parent_and_edge.first.layer == PrecedenceGraphLayer::kScheduled ||
                               parent_and_edge.first.layer == PrecedenceGraphLayer::kFree;
                      });

      if (parents_of_t_dash_dash_are_subset) {
        t_dash_dash.layer = PrecedenceGraphLayer::kSecond;
      }
    }
  }
}

std::vector<daisi::material_flow::Task> LayeredPrecedenceGraph::getAuctionableTasks() {
  auto free_vertices = getLayerVertices(PrecedenceGraphLayer::kFree);
  std::vector<daisi::material_flow::Task> tasks;

  for (const auto &vertex : free_vertices) {
    if (!vertex.scheduled) {
      tasks.push_back(vertex.task);
    }
  }

  return tasks;
}

std::vector<LPCVertex> LayeredPrecedenceGraph::getLayerVertices(PrecedenceGraphLayer layer) const {
  std::vector<LPCVertex> vertices_of_layer;

  std::copy_if(vertices_.begin(), vertices_.end(), std::back_inserter(vertices_of_layer),
               [&layer](const auto &vertex) { return vertex.layer == layer; });

  return vertices_of_layer;
}

void LayeredPrecedenceGraph::setEarliestValidStartTime(const std::string &task_uuid,
                                                       const daisi::util::Duration &time) {
  auto it = std::find_if(vertices_.begin(), vertices_.end(),
                         [&](const auto &vertex) { return vertex.task.getUuid() == task_uuid; });
  if (it == vertices_.end()) {
    throw std::runtime_error("Vertex for task uuid not found in LayeredPrecedenceGraph.");
  }

  if (it->layer != PrecedenceGraphLayer::kFree) {
    throw std::runtime_error(
        "Earliest start time of Task can only be set if the task is on the free layer.");
  }

  it->earliest_valid_start = time;
}

void LayeredPrecedenceGraph::setLatestFinishTime(const std::string &task_uuid,
                                                 const daisi::util::Duration &time) {
  auto it = std::find_if(vertices_.begin(), vertices_.end(),
                         [&](const auto &vertex) { return vertex.task.getUuid() == task_uuid; });
  if (it == vertices_.end()) {
    throw std::runtime_error("Vertex for task uuid not found in LayeredPrecedenceGraph.");
  }

  if (it->layer != PrecedenceGraphLayer::kFree) {
    throw std::runtime_error("Latest start time of Task can only be set if the task is on the free "
                             "layer after its scheduling.");
  }

  it->latest_finish = time;
}

bool LayeredPrecedenceGraph::areAllTasksScheduled() const {
  return std::all_of(vertices_.begin(), vertices_.end(), [](const auto &vertex) {
    return vertex.layer == PrecedenceGraphLayer::kScheduled;
  });
}

bool LayeredPrecedenceGraph::areAllFreeTasksScheduled() const {
  return std::all_of(vertices_.begin(), vertices_.end(), [](const auto &vertex) {
    return vertex.layer != PrecedenceGraphLayer::kFree || vertex.scheduled;
  });
}

LPCVertex LayeredPrecedenceGraph::getVertex(const std::string &task_uuid) const {
  auto it = std::find_if(vertices_.begin(), vertices_.end(), [&task_uuid](const auto &vertex) {
    return vertex.task.getUuid() == task_uuid;
  });

  if (it == vertices_.end()) {
    throw std::invalid_argument("Vertex with task uuid does not exist.");
  }

  return *it;
}

daisi::material_flow::Task LayeredPrecedenceGraph::getTask(const std::string &task_uuid) const {
  return getVertex(task_uuid).task;
}

daisi::util::Duration LayeredPrecedenceGraph::getEarliestValidStartTime(
    const std::string &task_uuid) const {
  auto vertex = getVertex(task_uuid);

  if (!vertex.earliest_valid_start.has_value()) {
    throw std::runtime_error("Earliest valid start of vertex is not set.");
  }

  return vertex.earliest_valid_start.value();
}

daisi::util::Duration LayeredPrecedenceGraph::getLatestFinishTime(
    const std::string &task_uuid) const {
  auto vertex = getVertex(task_uuid);

  if (!vertex.latest_finish.has_value()) {
    throw std::runtime_error("Latest finish of vertex is not set.");
  }

  return vertex.latest_finish.value();
}

void LayeredPrecedenceGraph::setTaskScheduled(const std::string &task_uuid) {
  auto it = std::find_if(vertices_.begin(), vertices_.end(), [&task_uuid](const auto &vertex) {
    return vertex.task.getUuid() == task_uuid;
  });

  if (it == vertices_.end()) {
    throw std::invalid_argument("Vertex with task uuid does not exist.");
  }

  if (it->layer != PrecedenceGraphLayer::kFree) {
    throw std::runtime_error("Only vertices from free layer can be set as scheduled. ");
  }

  it->scheduled = true;
}

bool LayeredPrecedenceGraph::isFreeTaskScheduled(const std::string &task_uuid) const {
  auto vertex = getVertex(task_uuid);

  if (vertex.layer != PrecedenceGraphLayer::kFree) {
    throw std::runtime_error("Task is not free");
  }

  return vertex.scheduled;
}

}  // namespace daisi::cpps::logical
