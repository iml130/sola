// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/search_exact/minhton_search_exact_algorithm.h"

#include <cmath>

#include "minhton/logging/logging.h"

namespace minhton {

minhton::NodeInfo MinhtonSearchExactAlgorithm::calcClosestRedirect(double dest_value) {
  double self_value = getSelfNodeInfo().getPeerInfo().getHorizontalValue();
  auto neighbors = getRoutingInfo()->getAllUniqueKnownExistingNeighbors();

  if (self_value < dest_value) {
    // node is to the right of us
    // removing every neighbor from the left
    neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                                   [&, self_value](const minhton::NodeInfo &node) {
                                     return self_value > node.getPeerInfo().getHorizontalValue();
                                   }),
                    neighbors.end());

  } else {
    // node is to the left of us
    // removing every neighbor from the right
    neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                                   [&, self_value](const minhton::NodeInfo &node) {
                                     return self_value < node.getPeerInfo().getHorizontalValue();
                                   }),
                    neighbors.end());
  }

  // choose the closest node in the neighbors
  double best_diff = std::fabs(self_value - dest_value);
  auto best_node = getSelfNodeInfo();
  for (auto const &link : neighbors) {
    double node_value = link.getPeerInfo().getHorizontalValue();
    double current_diff = std::fabs(node_value - dest_value);
    if (current_diff < best_diff) {
      best_diff = current_diff;
      best_node = link;
    }
  }

  return best_node;
}

}  // namespace minhton
