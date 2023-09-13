// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/join/minhton_join_algorithm.h"

#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"

namespace minhton {

void MinhtonJoinAlgorithm::processJoin(const MessageJoin &msg) {
  if (access_->node_locked) {
    // TODO send join deny
    // drop for now
    return;
  }

  if (msg.getHeader().getRefEventId() == 0) {
    access_->procedure_info->saveEventId(ProcedureKey::kJoinProcedure,
                                         msg.getHeader().getEventId());
    LOG_EVENT(EventType::kJoinEvent, msg.getHeader().getEventId());
  } else {
    access_->procedure_info->saveEventId(ProcedureKey::kJoinProcedure,
                                         msg.getHeader().getRefEventId());
  }

  NodeInfo entering_node = msg.getEnteringNode();
  bool accept = false;

  // Decide how to handle the request depending on search_progress of msg
  switch (msg.getSearchProgress()) {
    // Currently at start position or directly after adjacency jump
    case SearchProgress::kNone:
      // Check if it's not a null node
      if (getRoutingInfo()->areChildrenFull()) {
        find_end_helper_.forwardToAdjacentNode(entering_node, msg.getHopCount());
      } else {
        accept = find_end_helper_.decideNextStep(entering_node, msg.getHopCount());
      }
      break;

    // Found correct level (h-2 or perfect tree), now search in the previously decided direction
    case SearchProgress::kSearchLeft:
    case SearchProgress::kSearchRight:
      // Check if we already are at the correct node to join
      accept = find_end_helper_.isCorrectParent();
      if (!accept) {
        // The SearchProgress is either left (1) or right (0) in this case
        auto search_left = static_cast<bool>(msg.getSearchProgress());
        find_end_helper_.searchEndOnLevel(entering_node, search_left, msg.getHopCount());
      }
      break;

    // Find out whether the current level is completely filled (level h-2 or perfect tree) or not
    // (level h-1)
    case SearchProgress::kCheckRight:
      find_end_helper_.checkRight(entering_node, msg.getHopCount());
      break;

    default:
      throw AlgorithmException(AlgorithmType::kJoinAlgorithm,
                               "search_progress is set to an invalid value");
  }

  // Current node is the correct parent, accept entering node as child
  if (accept) {
    performAcceptChild(entering_node, true);
  }
}

uint32_t MinhtonJoinAlgorithm::performSendUpdateNeighborMessagesAboutEnteringNode(
    NodeInfo entering_node) {
  // Each one of our routing table neighbors need to get informed
  // about our new child, because all of our children are
  // routing table neighbor children for them.

  uint32_t updates_sent = 0;

  // sending to each position UPDATE_NEIGHBOR message
  for (auto const &neighbor : getRoutingInfo()->getRoutingTableNeighbors()) {
    if (neighbor.isInitialized()) {
      MinhtonMessageHeader header(
          getSelfNodeInfo(), neighbor,
          access_->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));
      MessageUpdateNeighbors message_update_neighbors(
          header,
          {std::make_tuple(entering_node, NeighborRelationship::kRoutingTableNeighborChild)}, true);
      send(message_update_neighbors);

      updates_sent++;
    }
  }

  // Many of our routing table neighbor children need to get
  // informed about our child, because they might be
  // routing table neighbors.
  // We have information about all of them if they exist.

  // Calculating positions of all routing table neighbor positions
  // of the entering node
  std::vector<std::tuple<uint32_t, uint32_t>> entering_node_routing_table_neighbor_positions =
      calcLeftRT(entering_node.getLevel(), entering_node.getNumber(),
                 getSelfNodeInfo().getFanout());
  std::vector<std::tuple<uint32_t, uint32_t>> right_entering_node_routing_table_neighbor_positions =
      calcRightRT(entering_node.getLevel(), entering_node.getNumber(),
                  getSelfNodeInfo().getFanout());

  entering_node_routing_table_neighbor_positions.insert(
      entering_node_routing_table_neighbor_positions.end(),
      right_entering_node_routing_table_neighbor_positions.begin(),
      right_entering_node_routing_table_neighbor_positions.end());

  // sending to each position UPDATE_NEIGHBOR message
  for (auto const &position : entering_node_routing_table_neighbor_positions) {
    // finding entry to the position in our routing information
    NodeInfo child_or_routing_table_neighbor_child =
        getRoutingInfo()->getNodeInfoByPosition(std::get<0>(position), std::get<1>(position));

    if (child_or_routing_table_neighbor_child.isInitialized()) {
      MinhtonMessageHeader header(
          getSelfNodeInfo(), child_or_routing_table_neighbor_child,
          access_->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));
      MessageUpdateNeighbors message_update_neighbors(
          header, {std::make_tuple(entering_node, NeighborRelationship::kRoutingTableNeighbor)},
          true);
      send(message_update_neighbors);

      updates_sent++;
    }
  }
  return updates_sent;
}

}  // namespace minhton
