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

  initLayers();
}

void LayeredPrecedenceGraph::initLayers() {
  // free layer = no incoming edges
  // T_F = { t_k in T | there exists no t_j in T : (t_j, t_k) in E }
  std::vector<LPCVertex> temp_free_layer_vertices;
  for (auto &vertex : vertices_) {
    auto incoming_edges = getIncomingEdges(vertex);
    if (incoming_edges.empty()) {
      vertex.layer = Layer::kFree;
      temp_free_layer_vertices.push_back(vertex);
    }
  }

  // second layer = neighbors of free layer
  // T_L = { t_k in T | there exists t_j in T_F : (t_j, t_k) in E }
  std::vector<LPCVertex> temp_second_layer_vertices;
  for (const auto &free_vertex : temp_free_layer_vertices) {
    for (auto &vertex : vertices_) {
      if (free_vertex != vertex) {
        if (hasEdge(free_vertex, vertex)) {
          if (vertex.layer == Layer::kFree) {
            throw std::runtime_error("Conceptionally, the algorithm should not allow this state.");
          }

          if (vertex.layer != Layer::kSecond) {
            vertex.layer = Layer::kSecond;
            temp_second_layer_vertices.push_back(vertex);
          }
        }
      }
    }
  }

  // remaining vertices are in hidden layer
  // T_H = T - (T_F union T_L)
  for (auto &vertex : vertices_) {
    if (vertex.layer != Layer::kFree && vertex.layer != Layer::kSecond) {
      vertex.layer = Layer::kHidden;
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
    if (vertex.layer == Layer::kFree) {
      vertex.layer = Layer::kScheduled;
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
    if (t_dash.layer == Layer::kSecond && hasEdge(t, t_dash)) {
      auto parents_of_t_dash = getIncomingEdges(t_dash);
      bool parents_of_t_dash_are_subset = std::all_of(
          parents_of_t_dash.begin(), parents_of_t_dash.end(), [](const auto &parent_and_edge) {
            return parent_and_edge.first.layer == Layer::kScheduled;
          });

      // 3: if parents(t') subset of T_S then
      // if all parents of a task are scheduled, the task can be auctioned next
      if (parents_of_t_dash_are_subset) {
        // 4: Move t': T_L -> T_F
        t_dash.layer = Layer::kFree;

        // 5: PC[t'] = max_{t'' in parents(t')} (F[t''])
        // PC represents the earliest valid start time for a task

        std::vector<daisi::util::Duration> parent_finish_times;
        std::transform(parents_of_t_dash.begin(), parents_of_t_dash.end(),
                       std::back_inserter(parent_finish_times), [](const auto &parent_and_edge) {
                         return parent_and_edge.first.finish_time.value();
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
    if (t_dash_dash.layer == Layer::kHidden && hasEdge(t_dash, t_dash_dash)) {
      // 7: if parents(t'') subset (T_S union T_F) then
      auto parents_of_t_dash_dash = getIncomingEdges(t_dash_dash);
      bool parents_of_t_dash_dash_are_subset =
          std::all_of(parents_of_t_dash_dash.begin(), parents_of_t_dash_dash.end(),
                      [](const auto &parent_and_edge) {
                        return parent_and_edge.first.layer == Layer::kScheduled ||
                               parent_and_edge.first.layer == Layer::kFree;
                      });

      if (parents_of_t_dash_dash_are_subset) {
        t_dash_dash.layer = Layer::kSecond;
      }
    }
  }
}

std::vector<daisi::material_flow::Task> LayeredPrecedenceGraph::getAuctionableTasks() {
  auto vertices = getLayerVertices(Layer::kFree);
  std::vector<daisi::material_flow::Task> tasks;

  std::transform(vertices.begin(), vertices.end(), std::back_inserter(tasks),
                 [](const auto &vertex) { return vertex.task; });

  return tasks;
}

std::vector<LPCVertex> LayeredPrecedenceGraph::getLayerVertices(Layer layer) const {
  std::vector<LPCVertex> vertices_of_layer;

  std::copy_if(vertices_.begin(), vertices_.end(), std::back_inserter(vertices_of_layer),
               [&layer](const auto &vertex) { return vertex.layer == layer; });

  return vertices_of_layer;
}

}  // namespace daisi::cpps::logical