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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_ASSIGNMENT_LAYERED_PRECEDENCE_GRAPH_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_ASSIGNMENT_LAYERED_PRECEDENCE_GRAPH_H_

#include <memory>
#include <optional>

#include "cpps/common/cpps_logger_ns3.h"
#include "datastructure/directed_graph.tpp"
#include "layered_precedence_graph_components.h"
#include "material_flow/model/material_flow.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

/// @brief Helper class to implement the pIA algorithm.
/// A directed graph is layered into a free, second, and hidden layer.
/// Prioritization is neglected in this modification of the algorithm.
/// Therefore, all tasks from the free layer are automatically auctionable.
///
/// The algorithm is based on the following paper:
/// McIntire, Mitchell, Ernesto Nunes, and Maria Gini. "Iterated multi-robot auctions for
/// precedence-constrained task scheduling." Proceedings of the 2016 international conference on
/// autonomous agents & multiagent systems. 2016.
class LayeredPrecedenceGraph : private datastructure::DirectedGraph<LPCVertex, std::monostate> {
public:
  explicit LayeredPrecedenceGraph(std::shared_ptr<material_flow::MFDLScheduler> scheduler,
                                  const std::string &connection_string);

  ~LayeredPrecedenceGraph() = default;

  /// @brief Taking the next step in the algorithm. Updating the graph by assuming that all tasks of
  /// the free layer are now scheduled. Accordingly, layers from the second and hidden layer need to
  /// be updated.
  void next();

  /// @brief In this modification of pIA we do not consider prioritizations yet. Therefore, all free
  /// tasks are auctionable.
  /// @return Vector of all free tasks.
  std::vector<material_flow::Task> getAuctionableTasks() const;

  /// @brief Setting the earliest valid start time, in pIA represented as PC[t], of a task.
  /// @param task Task to search for the according vertex
  /// @param time Earliest valid start time
  void setEarliestValidStartTime(const std::string &task_uuid, const util::Duration &time);

  /// @brief Setting the latest finish time, in pIA represented as F[t], of a task.
  /// @param task Task to search for the according vertex
  /// @param time Earliest valid start time
  void setLatestFinishTime(const std::string &task_uuid, const util::Duration &time);

  util::Duration getEarliestValidStartTime(const std::string &task_uuid) const;

  util::Duration getLatestFinishTime(const std::string &task_uuid) const;

  material_flow::Task getTask(const std::string &task_uuid) const;

  /// @brief Checks whether all tasks are on the scheduled layer. The scheduled flag is not
  /// considered in this.
  /// @return True if all tasks are on the scheduled layer.
  bool areAllTasksScheduled() const;

  /// @brief Checks whether for all free tasks the scheduled flag is set. If yes, this means that
  /// the iteration is finished.
  /// @return True if for all free tasks the scheduled flag is set.
  bool areAllFreeTasksScheduled() const;

  /// @brief Setting the scheduled flag of a task.
  /// @param task_uuid Uuid of the task we refer to
  void setTaskScheduled(const std::string &task_uuid);

  /// @brief Checking whether a free task has already been scheduled in this iteration before the
  /// layers get updated.
  /// @param task_uuid Uuid of the task we refer to
  /// @return True if the task is on the free layer and the scheduled flag is set
  bool isFreeTaskScheduled(const std::string &task_uuid) const;

  bool isTaskFree(const std::string &task_uuid) const;

  std::vector<material_flow::Task> getTasks() const;

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

  LPCVertex getVertex(const std::string &task_uuid) const;
};

}  // namespace daisi::cpps::logical

#endif
