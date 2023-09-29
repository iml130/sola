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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_ASSIGNMENT_LAYERED_PRECEDENCE_GRAPH_COMPONENTS_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_ASSIGNMENT_LAYERED_PRECEDENCE_GRAPH_COMPONENTS_H_

#include <memory>
#include <optional>

#include "datastructure/directed_graph.tpp"
#include "material_flow/model/material_flow.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

/// @brief Enum to represent the different layers tasks can be on in this precedence graph.
/// The free layer is also referred to as T_F, the second layer as T_L, the hidden layer as T_H, and
/// the scheduled layer as T_S.
enum class PrecedenceGraphLayer { kFree, kSecond, kHidden, kScheduled, kNone };

struct LPCVertex {
  /// @brief Initializing the vertex by setting the task and everything else as invalid.
  /// @param task The task this vertex represents.
  explicit LPCVertex(material_flow::Task task) : task(std::move(task)){};

  /// @brief The task this vertex represents by giving it additional information for auction and
  /// about the layer.
  material_flow::Task task;

  /// @brief Assigning a layer to the task as presented by the set formulations in pIA.
  PrecedenceGraphLayer layer = PrecedenceGraphLayer::kNone;

  /// @brief F[t] in pIA; latest finish time of tasks that have been scheduled.
  /// std::nullopt otherwise
  std::optional<util::Duration> latest_finish = std::nullopt;

  /// @brief PC[t] in pIA; earliest valid start time of tasks whose predecessors have been
  /// scheduled. Tasks initially in T_F can be started at any time. If predecessors are not
  /// scheduled, std::nullopt is set.
  std::optional<util::Duration> earliest_valid_start = std::nullopt;

  /// @brief Flag representing that a free task has been already scheduled.
  /// The flag is not used outside of free layer tasks and disregarded on further layers.
  bool scheduled = false;

  friend bool operator==(const LPCVertex &v1, const LPCVertex &v2) { return v1.task == v2.task; }

  friend bool operator!=(const LPCVertex &v1, const LPCVertex &v2) { return v1.task != v2.task; }
};

}  // namespace daisi::cpps::logical

#endif
