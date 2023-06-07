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

#include <optional>

#include "datastructure/directed_graph.h"
#include "material_flow/model/task.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

enum Layer { kFree, kSecond, kHidden, kScheduled, kNone };

struct LPCVertex {
  daisi::material_flow::Task task;
  Layer layer;
  std::optional<daisi::util::Duration> finish_time;
  std::optional<daisi::util::Duration> earliest_valid_start;

  friend bool operator==(const LPCVertex &v1, const LPCVertex &v2) { return v1.task == v2.task; }

  friend bool operator!=(const LPCVertex &v1, const LPCVertex &v2) { return v1.task != v2.task; }
};

class MFDL;

/// @brief Helper class to implement the pIA algorithm.
/// A directed graph is layered into a free, second, and hidden layer.
/// Prioritization is neglected in this modification of the algorithm.
///
/// The algorithm is based on the following paper:
/// McIntire, Mitchell, Ernesto Nunes, and Maria Gini. "Iterated multi-robot auctions for
/// precedence-constrained task scheduling." Proceedings of the 2016 international conference on
/// autonomous agents & multiagent systems. 2016.
class LayeredPrecedenceGraph
    : private daisi::datastructure::DirectedGraph<LPCVertex, std::monostate> {
public:
  LayeredPrecedenceGraph(const MFDL &mfdl);

  ~LayeredPrecedenceGraph() = default;

  void initLayers();
  void updateLayers();

  std::vector<daisi::material_flow::Task> getAuctionableTasks();

private:
  // helper method for updateLayers
  void updateLayersSecondToFree(const LPCVertex &t);
  void updateLayersHiddenToSecond(const LPCVertex &t_dash);

  std::vector<LPCVertex> getLayerVertices(Layer layer) const;
};

}  // namespace daisi::cpps::logical

#endif
