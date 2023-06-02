// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <math.h>

#include "minhton/algorithms/join/join_algorithm_general.h"
#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"

namespace minhton {

///
/// First we have to intialize the worst case distance to root,
/// because we want to find the child position with the shortest horizontal
/// distance to root.
///
/// In the for loop we are looking at each child position, which is uninitialized (nullptr).
/// The distance to root gets calculated with the tree mapper. If our distance is shorter,
/// we save it. In the end we create a NodeInfo object with the calculated position.
///
/// If there is no free child position, we return nullptr.
///
minhton::NodeInfo JoinAlgorithmGeneral::calcNewChildPosition(bool use_complete_balancing) const {
  // Find the first free child position (complete balancing)
  if (use_complete_balancing) {
    for (uint16_t i = 0; i < getRoutingInfo()->getFanout(); i++) {
      // look at each free position
      if (!getRoutingInfo()->getChildren()[i].isInitialized()) {
        uint16_t current_num = getSelfNodeInfo().getNumber() * getRoutingInfo()->getFanout() + i;
        minhton::NodeInfo new_child =
            NodeInfo(getSelfNodeInfo().getLevel() + 1, current_num, getRoutingInfo()->getFanout());
        return new_child;
      }
    }
  }

  // Find the free child position closest to root (general null balancing)
  double best_diff = k_TREEMAPPER_ROOT_VALUE * 2 + 1;
  double root_value = k_TREEMAPPER_ROOT_VALUE;
  uint32_t best_num = UINT32_MAX;

  for (uint16_t i = 0; i < getRoutingInfo()->getFanout(); i++) {
    // look at each free position
    if (!getRoutingInfo()->getChildren()[i].isInitialized()) {
      uint16_t current_num = getSelfNodeInfo().getNumber() * getRoutingInfo()->getFanout() + i;
      double current_value = treeMapper(getSelfNodeInfo().getLevel() + 1, current_num,
                                        getRoutingInfo()->getFanout(), k_TREEMAPPER_ROOT_VALUE);
      double current_diff = std::abs(root_value - current_value);

      if (current_diff < best_diff) {
        best_num = current_num;
        best_diff = current_diff;
      }
    }
  }

  if (best_num < UINT32_MAX) {
    minhton::NodeInfo new_child =
        NodeInfo(getSelfNodeInfo().getLevel() + 1, best_num, getRoutingInfo()->getFanout());

    return new_child;
  }

  return minhton::NodeInfo();
}

minhton::NodeInfo JoinAlgorithmGeneral::calcAdjacentLeftOfNewChild(
    const minhton::NodeInfo &entering_node) {
  minhton::NodeInfo best_node;
  double target_value = entering_node.getPeerInfo().getHorizontalValue();

  std::vector<minhton::NodeInfo> left_neighbors =
      getRoutingInfo()->getAllUniqueKnownExistingNeighbors();

  // removing every neighbor which is to the right of the entering node
  left_neighbors.erase(std::remove_if(left_neighbors.begin(), left_neighbors.end(),
                                      [&, entering_node](const minhton::NodeInfo &node) {
                                        return entering_node.getPeerInfo() < node.getPeerInfo();
                                      }),
                       left_neighbors.end());

  // also adding ourselves because we could also be the adjacent
  left_neighbors.push_back(getSelfNodeInfo());

  double best_value = -1;
  for (auto const &node : left_neighbors) {
    double current_value = node.getPeerInfo().getHorizontalValue();
    if (best_value < current_value && current_value < target_value) {
      best_value = current_value;
      best_node = node;
    }
  }

  return best_node;
}

minhton::NodeInfo JoinAlgorithmGeneral::calcAdjacentRightOfNewChild(
    const minhton::NodeInfo &entering_node) {
  minhton::NodeInfo best_node;
  double target_value = entering_node.getPeerInfo().getHorizontalValue();

  auto right_neighbors = getRoutingInfo()->getAllUniqueKnownExistingNeighbors();

  // removing every neighbor which is to the left of the entering node
  right_neighbors.erase(std::remove_if(right_neighbors.begin(), right_neighbors.end(),
                                       [&, entering_node](const minhton::NodeInfo &node) {
                                         return entering_node.getPeerInfo() > node.getPeerInfo();
                                       }),
                        right_neighbors.end());

  // also adding ourselves because we could also be the adjacent
  right_neighbors.push_back(getSelfNodeInfo());

  double best_value = k_TREEMAPPER_ROOT_VALUE * 2 + 1;
  for (auto const &node : right_neighbors) {
    double current_value = node.getPeerInfo().getHorizontalValue();
    if (target_value < current_value && current_value < best_value) {
      best_value = current_value;
      best_node = node;
    }
  }

  return best_node;
}

/// Checking if we must send an update to the adjacent left
/// in the accept child procedure
bool JoinAlgorithmGeneral::mustSendUpdateLeft(
    const minhton::NodeInfo &entering_node_adj_right) const {
  if (!entering_node_adj_right.isValidPeer()) {
    return false;
  }

  if (entering_node_adj_right.getPeerInfo() == getSelfNodeInfo().getPeerInfo()) {
    return false;
  }

  return true;
}

/// Checking if we must send an update to the adjacent right
/// in the accept child procedure
bool JoinAlgorithmGeneral::mustSendUpdateRight(
    const minhton::NodeInfo &entering_node_adj_left) const {
  if (!entering_node_adj_left.isValidPeer()) {
    return false;
  }

  if (entering_node_adj_left.getPeerInfo() == getSelfNodeInfo().getPeerInfo()) {
    return false;
  }

  return true;
}

/// Helper method to calculatet our own adjacent left
/// in the accept child procedure
minhton::NodeInfo JoinAlgorithmGeneral::calcOurNewAdjacentLeft(
    const minhton::NodeInfo &entering_node,
    const minhton::NodeInfo &entering_node_adj_right) const {
  if (entering_node_adj_right.isValidPeer()) {
    if (entering_node_adj_right.getPeerInfo() == getSelfNodeInfo().getPeerInfo()) {
      return entering_node;
    }
  }

  return getRoutingInfo()->getAdjacentLeft();
}

/// Helper method to calculatet our own adjacent left
/// in the accept child procedure
minhton::NodeInfo JoinAlgorithmGeneral::calcOurNewAdjacentRight(
    const minhton::NodeInfo &entering_node, const minhton::NodeInfo &entering_node_adj_left) const {
  if (entering_node_adj_left.isValidPeer()) {
    if (entering_node_adj_left.getPeerInfo() == getSelfNodeInfo().getPeerInfo()) {
      return entering_node;
    }
  }

  return getRoutingInfo()->getAdjacentRight();
}

///
/// e.g. fanout 2, we are 1:0 and entering node is 2:0
/// if 2:1 and 2:2 will be rt routing table neighbors of 2:0
/// if they exist, we know them through routing table neighbor children
/// and return them
///
std::vector<minhton::NodeInfo> JoinAlgorithmGeneral::getRoutingTableNeighborsForNewChild(
    const minhton::NodeInfo &new_child) {
  if (!getSelfNodeInfo().isInitialized()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  auto children = getRoutingInfo()->getChildren();
  bool entering_node_is_our_child = std::any_of(
      children.begin(), children.end(),
      [&](const minhton::NodeInfo &node) { return node.getPeerInfo() == new_child.getPeerInfo(); });

  if (!entering_node_is_our_child) {
    throw std::logic_error("The entering node has to be one of our children");
  }

  std::vector<minhton::NodeInfo> neighbors_for_new_child = std::vector<minhton::NodeInfo>();

  std::vector<std::tuple<uint32_t, uint32_t>> rt_positions =
      calcLeftRT(new_child.getLevel(), new_child.getNumber(), getRoutingInfo()->getFanout());
  std::vector<std::tuple<uint32_t, uint32_t>> right_rt_positions =
      calcRightRT(new_child.getLevel(), new_child.getNumber(), getRoutingInfo()->getFanout());
  rt_positions.insert(rt_positions.end(), right_rt_positions.begin(), right_rt_positions.end());

  for (auto const &pos : rt_positions) {
    minhton::NodeInfo neighbor_of_child =
        getRoutingInfo()->getNodeInfoByPosition(std::get<0>(pos), std::get<1>(pos));

    if (neighbor_of_child.isInitialized()) {
      neighbors_for_new_child.push_back(neighbor_of_child);
    }
  }

  return neighbors_for_new_child;
}

// TODO must be optimized
minhton::NodeInfo JoinAlgorithmGeneral::getCloserAdjacent(
    const minhton::NodeInfo &entering_node, const minhton::NodeInfo &alleged_adjacent) const {
  // check first if alleged_adj is initialized

  if (!alleged_adjacent.isInitialized()) {
    return minhton::NodeInfo();
  }

  double entering_node_value = entering_node.getPeerInfo().getHorizontalValue();
  double alleged_adjacent_value = alleged_adjacent.getPeerInfo().getHorizontalValue();

  // up until which node we must calculate the tree
  uint32_t lower_node_level = std::max(entering_node.getLevel(), alleged_adjacent.getLevel());
  bool searching_for_adj_left = alleged_adjacent_value < entering_node_value;

  for (uint32_t level = 0; level <= lower_node_level; level++) {
    for (uint32_t number = 0; number < pow(getRoutingInfo()->getFanout(), level); number++) {
      minhton::PeerInfo temp_peer(level, number, getRoutingInfo()->getFanout());
      double temp_val = temp_peer.getHorizontalValue();

      auto children = getRoutingInfo()->getChildren();

      if (searching_for_adj_left) {
        if (alleged_adjacent_value < temp_val && temp_val < entering_node_value) {
          // and we dont have a child with this value

          bool found_closer_node_is_our_child =
              std::any_of(children.begin(), children.end(), [&](const minhton::NodeInfo &child) {
                return !child.isInitialized() && child.getPeerInfo() == temp_peer;
              });

          if (!found_closer_node_is_our_child) {
            return minhton::NodeInfo(level, number, getRoutingInfo()->getFanout());
          }
        }

      } else {
        if (entering_node_value < temp_val && temp_val < alleged_adjacent_value) {
          // and we dont have a child with this value

          bool found_closer_node_is_our_child =
              std::any_of(children.begin(), children.end(), [&](const minhton::NodeInfo &child) {
                return !child.isInitialized() && child.getPeerInfo() == temp_peer;
              });

          if (!found_closer_node_is_our_child) {
            return minhton::NodeInfo(level, number, getRoutingInfo()->getFanout());
          }
        }
      }
    }
  }

  return minhton::NodeInfo();
}

}  // namespace minhton
