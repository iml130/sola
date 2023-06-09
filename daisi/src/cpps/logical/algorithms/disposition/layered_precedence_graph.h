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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_LAYERED_PRECEDENCE_GRAPH_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_LAYERED_PRECEDENCE_GRAPH_H_

#include <memory>
#include <optional>

#include "datastructure/directed_graph.h"
#include "material_flow/model/material_flow.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

enum PrecedenceGraphLayer {
  kFree,       // T_F
  kSecond,     // T_L
  kHidden,     // T_H
  kScheduled,  // T_S
  kNone
};

struct LPCVertex {
  LPCVertex(const daisi::material_flow::Task &_task) : task(_task){};

  daisi::material_flow::Task task;

  /// @brief Assigning a layer to the task as presented by the set formulations in pIA.
  PrecedenceGraphLayer layer = PrecedenceGraphLayer::kNone;

  /// @brief F[t] in pIA; latest finish time of tasks that have been scheduled.
  /// std::nullopt otherwise
  std::optional<daisi::util::Duration> latest_finish_time = std::nullopt;

  /// @brief PC[t] in pIA; earliest valid start time of tasks whose predecessors have been
  /// scheduled. Tasks initially in T_F can be started at any time. If predecessors are not
  /// scheduled, std::nullopt is set.
  std::optional<daisi::util::Duration> earliest_valid_start = std::nullopt;

  friend bool operator==(const LPCVertex &v1, const LPCVertex &v2) { return v1.task == v2.task; }

  friend bool operator!=(const LPCVertex &v1, const LPCVertex &v2) { return v1.task != v2.task; }
};

/// @brief Helper class to implement the pIA algorithm.
/// A directed graph is layered into a free, second, and hidden layer.
/// Prioritization is neglected in this modification of the algorithm.
/// Therefore, all tasks from the free layer are automatically auctionable.
///
/// The algorithm is based on the following paper:
/// McIntire, Mitchell, Ernesto Nunes, and Maria Gini. "Iterated multi-robot auctions for
/// precedence-constrained task scheduling." Proceedings of the 2016 international conference on
/// autonomous agents & multiagent systems. 2016.
class LayeredPrecedenceGraph
    : private daisi::datastructure::DirectedGraph<LPCVertex, std::monostate> {
public:
  LayeredPrecedenceGraph(std::shared_ptr<daisi::material_flow::MFDLScheduler> scheduler);

  ~LayeredPrecedenceGraph() = default;

  /// @brief Taking the next step in the algorithm. Updating the graph by assuming that all tasks of
  /// the free layer are now scheduled. Accordingly, layers from the second and hidden layer need to
  /// be updated.
  void next();

  /// @brief In this modification of pIA we do not consider prioritizations yet. Therefore, all free
  /// tasks are auctionable.
  /// @return Vector of all free tasks.
  std::vector<daisi::material_flow::Task> getAuctionableTasks();

  /// @brief Setting the earliest valid start time, in pIA represented as PC[t], of a task.
  /// @param task Task to search for the according vertex
  /// @param time Earliest valid start time
  void setEarliestValidStartTime(const daisi::material_flow::Task &task,
                                 const daisi::util::Duration &time);

  /// @brief Setting the latest finish time, in pIA represented as F[t], of a task.
  /// @param task Task to search for the according vertex
  /// @param time Earliest valid start time
  void setLatestFinishTime(const daisi::material_flow::Task &task,
                           const daisi::util::Duration &time);

  bool areAllTasksScheduled() const;

private:
  /// @brief Initializing layers of the precedence graph based on set equations from the pIA
  /// algorithm.
  void initLayers();

  /// @brief Helper method for next(). Handling Line 2 onwards of the algorithm.
  /// @param t A Vertex that previously was on the free layer and is now scheduled.
  void updateLayersSecondToFree(const LPCVertex &t);

  /// @brief Helper method for next(). Handling Line 6 onwards of the algorithm.
  /// @param t_dash A Vertex that previously was on the second layer and is now free.
  void updateLayersHiddenToSecond(const LPCVertex &t_dash);

  /// @brief Helper method to filter vertices of a certain layer.
  /// @param layer we want to filter for
  /// @return Vector of vertices with given layer.
  std::vector<LPCVertex> getLayerVertices(PrecedenceGraphLayer layer) const;
};

}  // namespace daisi::cpps::logical

#endif
