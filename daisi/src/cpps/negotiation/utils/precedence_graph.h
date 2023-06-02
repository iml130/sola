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

#ifndef DAISI_NEGOTIATION_UTILS_PRECEDENCE_GRAPH_NS3_H_
#define DAISI_NEGOTIATION_UTILS_PRECEDENCE_GRAPH_NS3_H_

#include <cassert>
#include <functional>
#include <memory>

#include "cpps/model/task.h"
#include "cpps/negotiation/utils/directed_graph.h"
#include "cpps/negotiation/utils/precedence_graph_components.h"

namespace daisi::cpps {

class PrecedenceGraph : public DirectedGraph<Task, GEdge> {
public:
  PrecedenceGraph();

  void addEdge(const Task &start, const Task &end);

  void generateSimpleAdjacencyList();
  std::unordered_map<Task, std::vector<Task>> &getSimpleAdjacencyList();

  void generateSimpleInvertedAdjacencyList();
  std::unordered_map<Task, std::vector<Task>> &getSimpleInvertedAdjacencyList();

  std::vector<Task> getChildren(const Task &order);
  std::vector<Task> getParents(const Task &order);

  void tightenTimeWindows();
  void tightenTimeWindows2();
  std::unordered_map<int, std::vector<Task>> calcPrecedenceLayers();

  Task getUpToDateOrder(const Task &order);

protected:
  std::vector<std::vector<Task>> generatePaths();
  std::vector<std::vector<Task>> generatePaths(const Task &starting_node);

  void setMaxOrderDurations();
  void setAvgOrderDurations();
  std::unordered_map<Task, double> getMaxOrderDurations();
  std::unordered_map<Task, double> getAvgOrderDurations();

  std::unordered_map<Task, std::vector<Task>> simple_adjacency_list_;
  std::unordered_map<Task, std::vector<Task>> simple_inverted_adjacency_list_;
};

}  // namespace daisi::cpps

#endif
