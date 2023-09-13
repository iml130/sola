// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/leave/minhton_leave_algorithm.h"

#include <cassert>
#include <cstring>

#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"

namespace minhton {

bool MinhtonLeaveAlgorithm::canLeaveWithoutReplacement() {
  // If the right neighbor is not initialized, then the current node is the last one of the network
  // But if the current node is at the right edge of the level, there needs to be a check somewhere
  // else whether a replacement node is needed
  auto right_neighbor = getRoutingInfo()->getDirectRightNeighbor();
  if (right_neighbor.isValidPeer()) {
    // If there is no right neighbor and there is room for more nodes at the right we are at the
    // last node of the network
    return !(right_neighbor.isInitialized());
  }

  auto leftmost_neighbor_child = getRoutingInfo()->getLeftmostNeighborChild();
  if (leftmost_neighbor_child.isValidPeer()) {
    // If we are at the right edge we can check instead if we have the leftmost possible neighbor
    // child in our routing infos and if it exists
    if (leftmost_neighbor_child.getNumber() == 0) {
      return !(leftmost_neighbor_child.isInitialized());
    }
  }

  return false;
}

void MinhtonLeaveAlgorithm::initiateSelfDeparture() {
  if (canLeaveWithoutReplacement()) {
    solanet::UUID id = solanet::generateUUID();
    uint64_t event_id = 0;
    std::memcpy(&event_id, id.data(), 8);
    this->access_->procedure_info->saveEventId(ProcedureKey::kLeaveProcedure, event_id);
    LOG_EVENT(minhton::EventType::kLeaveEvent, event_id);
    this->performLeaveWithoutReplacement();
  } else {
    this->performFindReplacement();
    this->access_->set_timeout(TimeoutType::kReplacementOfferResponseTimeout);
  }
  this->access_->procedure_info->removeEventId(ProcedureKey::kLeaveProcedure);
}

void MinhtonLeaveAlgorithm::processFindReplacement(const minhton::MessageFindReplacement &message) {
  auto event_id = message.getHeader().getRefEventId();
  if (event_id == 0U) {
    event_id = message.getHeader().getEventId();
  }
  this->access_->procedure_info->saveEventId(ProcedureKey::kLeaveProcedure, event_id);

  this->performFindReplacement(message);
  this->access_->procedure_info->removeEventId(ProcedureKey::kLeaveProcedure);
}

void MinhtonLeaveAlgorithm::performFindReplacement() {
  // Make sure that the tree stays complete
  // (missing nodes are only at the last level and fill the levels from left to right without gaps)
  auto leaving_node = getSelfNodeInfo();
  bool is_correct_parent = false;

  // Check if the start position is not a null node
  if (getRoutingInfo()->areChildrenFull()) {
    find_end_helper_.forwardToAdjacentNode(leaving_node, 0);
  } else {
    is_correct_parent = find_end_helper_.decideNextStep(leaving_node, 0);
  }

  // Currently at the parent of the last node of the network
  if (is_correct_parent) {
    auto last_child = getRoutingInfo()->getInitializedChildren().back();
    // Forward to last child with own SearchProgress just for this case
    find_end_helper_.forwardRequest(last_child, leaving_node, SearchProgress::kReplacementNode, 0);
  }
}

void MinhtonLeaveAlgorithm::performFindReplacement(const minhton::MessageFindReplacement &msg) {
  // Make sure that the tree stays complete
  // (missing nodes are only at the last level and fill the levels from left to right without gaps)
  auto leaving_node = msg.getNodeToReplace();
  bool is_correct_parent = false;

  SearchProgress prog = msg.getSearchProgress();

  if (msg.getSender().getLevel() == getSelfNodeInfo().getLevel() &&
      msg.getSender().getNumber() > getSelfNodeInfo().getNumber()) {
    if (!getRoutingInfo()->getChild(0).isInitialized() && prog == kSearchRight) {
      prog = kSearchLeft;
    }
  }

  // Decide where to go next
  switch (prog) {
    // Start position or directly after adjacency jump
    case SearchProgress::kNone:
      LOG_INFO("FR Case 1");
      // Check if it's not a null node
      if (getRoutingInfo()->areChildrenFull()) {
        find_end_helper_.forwardToAdjacentNode(leaving_node, msg.getHopCount());
      } else {
        is_correct_parent = find_end_helper_.decideNextStep(leaving_node, msg.getHopCount());
      }
      break;

    // Found correct level (h-2 or perfect tree), now search in the previously decided direction
    case SearchProgress::kSearchLeft:
    case SearchProgress::kSearchRight:
      LOG_INFO("FR Case 2");
      // Check if we already are at the correct node to join
      is_correct_parent = find_end_helper_.isCorrectParent();
      if (!is_correct_parent) {
        find_end_helper_.searchEndOnLevel(leaving_node, static_cast<bool>(prog), msg.getHopCount());
      }
      break;

    // Find out whether the current level is completely filled (level h-2 or perfect tree) or not
    // (level h-1)
    case SearchProgress::kCheckRight:
      LOG_INFO("FR Case 3");
      find_end_helper_.checkRight(leaving_node, msg.getHopCount());
      break;

    // The current node emerged as the last node of the network
    case SearchProgress::kReplacementNode:
      LOG_INFO("FR Case 4");
      if (msg.getTarget().getLogicalNodeInfo() != getSelfNodeInfo().getLogicalNodeInfo()) {
        return;  // DROP DO NOT FORWARD TO INVALID NODES
      }

      if (getSelfNodeInfo() != leaving_node) {
        this->prepareLeavingAsSuccessor(leaving_node);
      } else {
        // The leaving node is the last node of the network and the last possible one on a level
        this->performLeaveWithoutReplacement();
        this->access_->cancel_timeout(
            TimeoutType::kReplacementOfferResponseTimeout);  // otherwise the timeout would be
                                                             // called after we left
      }
      break;

    default:
      throw AlgorithmException(AlgorithmType::kLeaveAlgorithm,
                               "search_progress_ is set to an invalid value");
  }

  // Currently at the parent of the last node of the network
  if (is_correct_parent) {
    auto last_child = getRoutingInfo()->getInitializedChildren().back();
    // Forward to last child with own SearchProgress just for this case
    find_end_helper_.forwardRequest(last_child, leaving_node, SearchProgress::kReplacementNode,
                                    msg.getHopCount());
  }
}

}  // namespace minhton
