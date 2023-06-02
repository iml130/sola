// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/leave/leave_algorithm_general.h"
#include "minhton/logging/logging.h"

namespace minhton {

NodeInfo LeaveAlgorithmGeneral::getAdjacentLeftFromVector(const NodeInfo &considered_node,
                                                          std::vector<NodeInfo> neighbors) {
  NodeInfo closest = NodeInfo();
  double considered_node_value = considered_node.getPeerInfo().getHorizontalValue();
  double best_diff = k_TREEMAPPER_ROOT_VALUE * 2;

  for (auto const &node : neighbors) {
    double node_value = node.getPeerInfo().getHorizontalValue();
    double current_diff = considered_node_value - node_value;

    if (current_diff > 0) {  /// node is actually on the left side
      if (current_diff < best_diff) {
        closest = node;
        best_diff = current_diff;
      }
    }
  }

  return closest;
}

NodeInfo LeaveAlgorithmGeneral::getAdjacentRightFromVector(const NodeInfo &considered_node,
                                                           std::vector<NodeInfo> neighbors) {
  NodeInfo closest = NodeInfo();
  double considered_node_value = considered_node.getPeerInfo().getHorizontalValue();
  double best_diff = k_TREEMAPPER_ROOT_VALUE * 2;

  for (auto const &node : neighbors) {
    double node_value = node.getPeerInfo().getHorizontalValue();
    double current_diff = node_value - considered_node_value;

    if (current_diff > 0) {  /// node is actually on the right side
      if (current_diff < best_diff) {
        closest = node;
        best_diff = current_diff;
      }
    }
  }

  return closest;
}

}  // namespace minhton
