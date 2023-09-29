// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <cmath>

#include "minhton/algorithms/find_end/minhton_find_end_algorithm.h"
#include "minhton/logging/logging.h"

namespace minhton {
static constexpr uint16_t kMaxHopCount = 64U;

void MinhtonFindEndAlgorithm::forwardRequest(const minhton::NodeInfo target,
                                             const minhton::NodeInfo request_origin,
                                             SearchProgress search_progress, uint16_t hop_count) {
  ++hop_count;
  uint16_t new_hop_count = hop_count;
  if (new_hop_count > kMaxHopCount) {
    throw std::runtime_error("MINHTON message to be forwarded exceeds max hop count");
  }

  MinhtonMessageHeader header(getSelfNodeInfo(), target);

  if (join_) {
    header.setRefEventId(access_->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));
    MessageJoin msg_to_forward(header, request_origin, search_progress, new_hop_count);
    send(msg_to_forward);
    access_->procedure_info->removeEventId(ProcedureKey::kJoinProcedure);
  } else {
    uint64_t ref_event_id = 0;
    if (access_->procedure_info->hasKey(ProcedureKey::kLeaveProcedure)) {
      ref_event_id = access_->procedure_info->loadEventId(ProcedureKey::kLeaveProcedure);
    }
    if (ref_event_id != 0U) {
      header.setRefEventId(ref_event_id);
    }

    MessageFindReplacement msg_to_forward(header, request_origin, search_progress, new_hop_count);
    send(msg_to_forward);
    if (ref_event_id == 0U) {
      auto event_id = msg_to_forward.getHeader().getEventId();
      access_->procedure_info->saveEventId(ProcedureKey::kLeaveProcedure, event_id);
      LOG_EVENT(minhton::EventType::kLeaveEvent, event_id);
    }
  }
}

void MinhtonFindEndAlgorithm::forwardToAdjacentNode(const minhton::NodeInfo request_origin,
                                                    uint16_t hop_count) {
  auto level_capacity = pow(getSelfNodeInfo().getFanout(), getSelfNodeInfo().getLevel());
  bool in_left_half = getSelfNodeInfo().getNumber() + 1 < (level_capacity / 2);
  minhton::NodeInfo target;

  // Choose the right adjacent if node is in left half of the tree,
  // otherwise (node in the middle or right helf) choose the one on the left
  if (in_left_half) {
    target = getRoutingInfo()->getAdjacentRight();
  } else {
    target = getRoutingInfo()->getAdjacentLeft();
  }

  if (!target.isInitialized()) {
    throw std::logic_error("Adjacent node isn't initialized");
  }

  forwardRequest(target, request_origin, SearchProgress::kNone, hop_count);
}

bool MinhtonFindEndAlgorithm::isCorrectParent() const {
  auto left_neighbors = getRoutingInfo()->getLeftRoutingTableNeighborsLeftToRight();
  bool own_criteria = false;

  // For Join: Check if this node has capacity for a new child
  // For Leave: Check if this node has children and right neighbors have no children
  if (join_) {
    bool has_capacity = !getRoutingInfo()->areChildrenFull();
    own_criteria = has_capacity;
  } else {
    bool has_children = !getRoutingInfo()->getInitializedChildren().empty();
    auto right_neighbor_children =
        getRoutingInfo()->getRightRoutingTableNeighborChildrenLeftToRight();
    bool no_right_neighbor_children =
        (right_neighbor_children.empty()) || !(right_neighbor_children.front().isInitialized());
    own_criteria = has_children && no_right_neighbor_children;
  }

  // Left edge of level?
  if (left_neighbors.empty()) {
    return own_criteria;
  }

  const auto &children_left_side =
      getRoutingInfo()->getLeftRoutingTableNeighborChildrenLeftToRight();
  // If the current node wants to accept the entering node / provide a replacement node and has a
  // left neighbor, the left neighbor must have children
  if (!children_left_side.empty()) {
    const auto &last_child_left_side = children_left_side.back();

    // Check if the left neighbor has no capacity left (last child exists)
    if (last_child_left_side.isInitialized()) {
      return own_criteria;
    }
  }

  return false;
}

void MinhtonFindEndAlgorithm::searchEndOnLevel(const minhton::NodeInfo request_origin,
                                               bool left_side, uint16_t hop_count) {
  std::vector<minhton::NodeInfo> chosen_side_init;
  if (left_side) {
    chosen_side_init = getRoutingInfo()->getAllInitializedLeftRoutingTableNeighborsAndChildren();
  } else {
    chosen_side_init = getRoutingInfo()->getAllInitializedRightRoutingTableNeighborsAndChildren();
  }

  if (chosen_side_init.empty()) {
    // No neighbors at the side where we are searching
    if (join_) {
      throw std::logic_error("Position at the edge should be covered by other code parts");
    }
    if (!left_side) {
      // Search right during leave failed
      // TODO: Forward to best and not drop
      return;
    }

    // Perfect tree during leave procedure, forward to own parent
    forwardRequest(getRoutingInfo()->getParent(), request_origin, SearchProgress::kSearchRight,
                   hop_count);
    return;
  }

  auto last_child = chosen_side_init.back();

  // Neighbors on chosen side have no children
  if (last_child.getLevel() == getSelfNodeInfo().getLevel()) {
    if (!join_ && !left_side) {
      // TODO Forward to left or parent and do not drop
      return;
    }
    // If we search the left side: Continue searching on the left side
    // Else: The neighbor directly on the right should be the final position
    forwardRequest(chosen_side_init.front(), request_origin, static_cast<SearchProgress>(left_side),
                   hop_count);
  } else {
    auto closest_node_to_parent = findReachableNodeClosestToParent(last_child);
    forwardRequest(closest_node_to_parent, request_origin, SearchProgress::kSearchRight, hop_count);
  }
}

minhton::NodeInfo MinhtonFindEndAlgorithm::findReachableNodeClosestToParent(
    const minhton::NodeInfo child) const {
  bool actual_target_is_on_the_right =
      join_ && ((child.getNumber() + 1) % getSelfNodeInfo().getFanout() == 0);

  // Get the nearest node to the parent of the last known child (best case: parent itself)
  auto child_number = child.getNumber() + (actual_target_is_on_the_right ? 1 : 0);
  minhton::NodeInfo last_known_null_node;

  // Repeat (after the first try with neighboring nodes) as long as the parent isn't in the routing
  // table
  // TODO: Current loop only has to be executed once or twice --> Replace with if?
  do {
    const auto [parent_level, parent_number] =
        calcParent(child.getLevel(), child_number, getSelfNodeInfo().getFanout());
    last_known_null_node = getRoutingInfo()->getNodeInfoByPosition(parent_level, parent_number);
    child_number -= getSelfNodeInfo().getFanout();
  } while (!last_known_null_node.isInitialized());

  return last_known_null_node;
}

void MinhtonFindEndAlgorithm::checkRight(const minhton::NodeInfo request_origin,
                                         uint16_t hop_count) {
  auto rightmost_neighbor = getRoutingInfo()->getRightmostNeighbor();

  // We are at rightmost possible node
  if (!rightmost_neighbor.isValidPeer()) {
    throw std::logic_error("Position at the right edge should be covered before we get there");
  }

  // Rightmost neighbor exists
  if (rightmost_neighbor.isInitialized()) {
    // Is the rightmost possible neighbor initialized and in our routing infos?
    if (isRightmostPossibleNode(rightmost_neighbor)) {
      searchEndOnLevel(request_origin, true, hop_count);
    } else {
      // Continue looking for the end of the level
      forwardRequest(rightmost_neighbor, request_origin, SearchProgress::kCheckRight, hop_count);
    }
  } else {
    // Rightmost neighbor doesn't exist yet, so hop to the level below and seach to the right there
    forwardRequest(getRoutingInfo()->getParent(), request_origin, SearchProgress::kSearchRight,
                   hop_count);
  }
}

bool MinhtonFindEndAlgorithm::isRightmostPossibleNode(const minhton::NodeInfo node) {
  bool at_rightmost_number = node.getNumber() == (pow(node.getFanout(), node.getLevel()) - 1);
  return at_rightmost_number && node.isInitialized();
}

bool MinhtonFindEndAlgorithm::decideNextStep(const minhton::NodeInfo request_origin,
                                             uint16_t hop_count) {
  // Check left neighbors
  auto leftmost_neighbor_child = getRoutingInfo()->getLeftmostNeighborChild();
  if (leftmost_neighbor_child.isValidPeer()) {
    // Does the left neighbor have all children?
    if (isCorrectParent()) {
      return true;
    }

    // Has the leftmost neighbor at least one child?
    if (leftmost_neighbor_child.isInitialized()) {
      searchEndOnLevel(request_origin, true, hop_count);
      return false;
    }
  }

  auto rightmost_neighbor = getRoutingInfo()->getRightmostNeighbor();
  if (rightmost_neighbor.isValidPeer()) {
    // Default values if we get no information about the utitilization of the current level
    minhton::NodeInfo target = rightmost_neighbor;
    auto search_progress = SearchProgress::kCheckRight;

    // Check if rightmost neighbor is not initialized (level = h - 1)
    if (!rightmost_neighbor.isInitialized()) {
      target = getRoutingInfo()->getParent();
      search_progress = SearchProgress::kSearchRight;
    }

    // Is the rightmost possible neighbor initialized and in our routing infos?
    else if (isRightmostPossibleNode(rightmost_neighbor)) {
      // level = h-2 or perfect tree
      if (isCorrectParent()) {
        return true;
      }

      // Search on a direction depending on the own capacity for children
      bool search_on_left_side = !(getRoutingInfo()->areChildrenFull());
      searchEndOnLevel(request_origin, search_on_left_side, hop_count);
      return false;
    }

    forwardRequest(target, request_origin, search_progress, hop_count);
    return false;
  }

  // TODO: Consolidate with else if?
  // No right neighbors, position is last possible number on current level
  if (isCorrectParent()) {
    return true;
  }

  searchEndOnLevel(request_origin, true, hop_count);
  return false;
}

}  // namespace minhton
