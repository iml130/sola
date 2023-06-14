// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/leave/leave_algorithm_general.h"

#include <cassert>
#include <cmath>

#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"
#include "minhton/message/remove_and_update_neighbor.h"
#include "minhton/message/search_exact.h"
#include "minhton/utils/uuid.h"

namespace minhton {

void LeaveAlgorithmGeneral::process(const MessageVariant &msg) {
  std::visit(
      Overload{
          [this](const MessageFindReplacement &message) { processFindReplacement(message); },
          [this](const MessageReplacementOffer &message) { processReplacementOffer(message); },
          [this](const MessageReplacementAck &message) { processReplacementAck(message); },
          [this](const MessageReplacementUpdate &message) { processReplacementUpdate(message); },
          [this](const MessageSignoffParentRequest &message) {
            processSignOffParentRequest(message);
          },
          [this](const MessageSignoffParentAnswer &message) {
            processSignOffParentAnswer(message);
          },
          [this](const MessageLockNeighborRequest &message) {
            processLockNeighborRequest(message);
          },
          [this](const MessageLockNeighborResponse &message) {
            processLockNeighborResponse(message);
          },
          [this](const MessageRemoveNeighborAck &message) { processRemoveNeighborAck(); },
          [this](const MessageUnlockNeighbor &message) { processUnlockNeighbor(message); },
          [this](const MessageReplacementNack &message) { /*Currently unhandled*/ },
          [](auto &message) {
            throw AlgorithmException("Wrong Algorithm Interface process called");
          }},
      msg);
}

void LeaveAlgorithmGeneral::processUnlockNeighbor(const MessageUnlockNeighbor &msg) {
  assert(access_->node_locked);
  access_->node_locked = false;
  current_signoff_request_ = nullptr;

  if (locked_right_neighbor_) {
    // Forward to right
    auto right_neighbor = getRoutingInfo()->getDirectRightNeighbor();

    if (right_neighbor.isValidPeer()) {
      assert(right_neighbor.getLevel() == getSelfNodeInfo().getLevel());
      assert(right_neighbor.isInitialized());
      MinhtonMessageHeader header(getSelfNodeInfo(), right_neighbor,
                                  msg.getHeader().getRefEventId());
      MessageUnlockNeighbor req(header);
      send(req);
    } else {
      // we are the last node on the level, forward the lock neigbhor request to the first node on
      // the next level
      NodeInfo info(getSelfNodeInfo().getLevel() + 1, 0, getSelfNodeInfo().getFanout());
      MinhtonMessageHeader header(getSelfNodeInfo(), info, msg.getHeader().getRefEventId());
      MessageUnlockNeighbor req(header);
      access_->perform_search_exact(info, std::make_shared<MessageSEVariant>(req));
    }
    locked_right_neighbor_ = false;
  }

  if (locked_left_neighbor_) {
    assert(getSelfNodeInfo().getLevel() != 0);  // We cannot be root
    // Forward to left
    auto left_neighbor = getRoutingInfo()->getDirectLeftNeighbor();

    if (left_neighbor.isValidPeer()) {
      assert(left_neighbor.isInitialized());
      MinhtonMessageHeader header(getSelfNodeInfo(), left_neighbor,
                                  msg.getHeader().getRefEventId());
      MessageUnlockNeighbor req(header);
      send(req);
    } else {
      // we are the last node on the level, forward the lock neigbhor request to the first node on
      // the next level
      NodeInfo info(getSelfNodeInfo().getLevel() - 1,
                    pow(getSelfNodeInfo().getFanout(), getSelfNodeInfo().getLevel() - 1) - 1,
                    getSelfNodeInfo().getFanout());
      MinhtonMessageHeader header(getSelfNodeInfo(), info, msg.getHeader().getRefEventId());
      MessageUnlockNeighbor req(header);
      access_->perform_search_exact(info, std::make_shared<MessageSEVariant>(req));
    }
    locked_left_neighbor_ = false;
  }
}

void LeaveAlgorithmGeneral::processReplacementOffer(const MessageReplacementOffer &message) {
  // just sending an ACK back
  // maybe in the future checking some conditions

  access_->cancel_timeout(TimeoutType::kReplacementOfferResponseTimeout);

  MinhtonMessageHeader header(getSelfNodeInfo(), message.getSender(),
                              message.getHeader().getRefEventId());

  // Every neighbor that we know of
  MessageReplacementAck message_replacement_offer_ack(
      header, getRoutingInfo()->getAllUniqueKnownExistingNeighbors(),
      MessageReplacementAck::LockedStates{access_->node_locked, locked_right_neighbor_,
                                          locked_left_neighbor_});
  send(message_replacement_offer_ack);

  // resetting all of our information, so that we cannot do anything anymore
  // but the last position has to be logged for consistency with join logging
  getRoutingInfo()->resetPosition(message.getHeader().getRefEventId());
  LOG_NODE(getSelfNodeInfo());

  // Node lock was transfered to successor
  access_->node_locked = false;
  locked_right_neighbor_ = false;
  locked_left_neighbor_ = false;

  current_signoff_request_ = nullptr;  // TODO: All info transferred to successor?
  replacing_node_ = NodeInfo();
}

void LeaveAlgorithmGeneral::processReplacementUpdate(const MessageReplacementUpdate &message) {
  assert(message.getShouldAcknowledge());

  // Removing the removed position from our routing information (Position of the replacing node)
  // (we might not have the position)
  getRoutingInfo()->removeNeighbor(message.getRemovedPositionNode(),
                                   message.getHeader().getRefEventId());

  // Replacing the PhysicalNodeInfo about the replaced position
  // with those of the removed position
  auto replaced_neighbor = message.getReplacedPositionNode();
  replaced_neighbor.setPhysicalNodeInfo(
      message.getRemovedPositionNode().getPhysicalNodeInfo());  // new physical address
  replaced_neighbor.setLogicalNodeInfo(message.getNewLogicalNodeInfo());

  // Updating the node at the replaced position with the new PhysicalNodeInfo
  NodeInfo old_node = getRoutingInfo()->getNodeInfoByPosition(replaced_neighbor.getLevel(),
                                                              replaced_neighbor.getNumber());
  if (!old_node.isInitialized()) {
    return;  // old_node has left already -> Do not update
  }
  getRoutingInfo()->updateNeighbor(replaced_neighbor, message.getHeader().getRefEventId());

  uint32_t min_number = getSelfNodeInfo().getNumber() * getSelfNodeInfo().getFanout();
  uint32_t ack_replacement_update_forward = 0;
  if (message.getSender().getLevel() == getRoutingInfo()->getSelfNodeInfo().getLevel() + 1 &&
      message.getSender().getNumber() >= min_number &&
      message.getSender().getNumber() <= min_number + getSelfNodeInfo().getFanout() - 1) {
    // Node to be replaced is our child. Therefore, forward this MessageReplacementUpdate to all our
    // neighbors as they also know our (old) child
    for (const auto &neighbor : getRoutingInfo()->getRoutingTableNeighbors()) {
      MinhtonMessageHeader header(getSelfNodeInfo(), neighbor, message.getHeader().getRefEventId());
      MessageReplacementUpdate msg(header, message.getRemovedPositionNode(),
                                   message.getReplacedPositionNode(),
                                   message.getNewLogicalNodeInfo(), message.getShouldAcknowledge());
      send(msg);
      ack_replacement_update_forward++;
    }
  }

  last_replacement_update_ = std::make_shared<MessageReplacementUpdate>(message);
  access_->wait_for_acks(ack_replacement_update_forward, [this]() { processUpdateForwardAck(); });
}

void LeaveAlgorithmGeneral::processReplacementAck(const MessageReplacementAck &message) {
  access_->cancel_timeout(TimeoutType::kReplacementAckResponseTimeout);
  access_->procedure_info->saveEventId(ProcedureKey::kLeaveProcedure,
                                       message.getHeader().getRefEventId());
  auto [locked, locked_right, locked_left] = message.getLockedStates();
  performLeaveWithReplacement(message.getSender(), message.getNeighbors(), locked, locked_right,
                              locked_left);
}

void LeaveAlgorithmGeneral::performLeaveWithoutReplacement() {
  if (in_leave_progress_ || replacing_node_.isInitialized() ||
      access_->get_fsm_state() == kWaitForReplacementOffer ||
      access_->get_fsm_state() == kSignOffFromInlevelNeighbors) {
    // Already replacing someone else
    access_->set_new_fsm(FiniteStateMachine(kConnected));
    return;
  }

  leaving_event_id_ = access_->procedure_info->loadEventId(ProcedureKey::kLeaveProcedure);
  signOffFromParent();
}

void LeaveAlgorithmGeneral::performLeaveWithReplacement(
    NodeInfo leaving_node, std::vector<NodeInfo> neighbors_of_leaving_node,
    bool leaving_node_was_locked, bool leaving_node_locked_right, bool leaving_node_locked_left) {
  auto removed_position_node = getSelfNodeInfo();
  auto replaced_position_node = leaving_node;
  LOG_INFO("PerformLeaveWithReplacement: RemovedPosition " + getSelfNodeInfo().getString() +
           " - ReplacedPosition " + replaced_position_node.getString());

  // neighbors of removing node
  auto existing_symmetrical_neighbors_of_removed_position =
      getRoutingInfo()->getAllUniqueSymmetricalExistingNeighbors();

  // storing adjacents of removed position for updating later
  auto removed_position_adjacent_left = getRoutingInfo()->getAdjacentLeft();
  auto removed_position_adjacent_right = getRoutingInfo()->getAdjacentRight();

  // Store old parent for unlock
  old_parent_ = getRoutingInfo()->getParent();

  // we are leaving the removed position and going to the replaced position
  // and rebuilding our routing information
  replaceMyself(leaving_node, neighbors_of_leaving_node);

  assert(!access_->node_locked);
  access_->node_locked = leaving_node_was_locked;
  assert(!locked_right_neighbor_);
  locked_right_neighbor_ = leaving_node_locked_right;
  assert(!locked_left_neighbor_);
  locked_left_neighbor_ = leaving_node_locked_left;

  // neighbors of replaced node
  auto existing_symmetrical_neighbors_of_replaced_position =
      getRoutingInfo()->getAllUniqueSymmetricalExistingNeighbors();

  // sorting vectors for est difference
  std::sort(existing_symmetrical_neighbors_of_replaced_position.begin(),
            existing_symmetrical_neighbors_of_replaced_position.end());

  if (removed_position_adjacent_left.getLogicalNodeInfo() ==
      getSelfNodeInfo().getLogicalNodeInfo()) {
    removed_position_adjacent_left = getSelfNodeInfo();
  }
  if (removed_position_adjacent_right.getLogicalNodeInfo() ==
      getSelfNodeInfo().getLogicalNodeInfo()) {
    removed_position_adjacent_right = getSelfNodeInfo();
  }

  uint32_t acks_replacement_update = 0;
  // sending ReplacementUpdate to every neighbor of the leaving node
  // if PhysicalNodeInfo is not known we use SearchExact
  for (auto const &replaced_pos_neighbor : existing_symmetrical_neighbors_of_replaced_position) {
    // not sending to removed position
    if (replaced_pos_neighbor.getLogicalNodeInfo() == removed_position_node.getLogicalNodeInfo()) {
      continue;
    }

    if (!replaced_pos_neighbor.isInitialized()) {
      throw std::runtime_error("not initialized on ReplacementUpdate");
    }

    // sending ReplacementUpdate
    MinhtonMessageHeader temp_header(getSelfNodeInfo(), replaced_pos_neighbor, leaving_event_id_);
    MessageReplacementUpdate message_replacement_update(
        temp_header, removed_position_node, replaced_position_node,
        getSelfNodeInfo().getLogicalNodeInfo(), true);

    LOG_INFO("Sending ReplacementUpdate to " + replaced_pos_neighbor.getString());

    acks_replacement_update++;
    send(message_replacement_update);
  }

  access_->wait_for_acks(acks_replacement_update,
                         [this]() { processReceiveReplacementUpdateAck(); });

  access_->procedure_info->removeEventId(ProcedureKey::kLeaveProcedure);
}

uint32_t LeaveAlgorithmGeneral::signOffFromNeighborsAndAdjacents() {
  uint32_t remaining_acknowledge_signoff_neighbor_adjacent = 0;
  auto our_symmetrical_neighbors = getRoutingInfo()->getAllUniqueSymmetricalExistingNeighbors();
  for (auto const &neighbor : our_symmetrical_neighbors) {
    if (neighbor == access_->routing_info->getAdjacentLeft() ||
        neighbor == access_->routing_info->getAdjacentRight()) {
      // Do not send MessageRemoveNeighbor to exclusive adjacents (nodes which are only our
      // adjacent). These nodes are directly updated with their new adjacent later.
      continue;
    }
    // it is not required to send RemoveNeighbor messages right of us
    if (getSelfNodeInfo().getNumber() < neighbor.getNumber()) {
      assert(getSelfNodeInfo().getLevel() == neighbor.getLevel());
      continue;
    }

    if (neighbor == getRoutingInfo()->getParent()) {
      assert(getSelfNodeInfo().getFanout() > 3);
      continue;  // Already received via parent signoff
    }

    remaining_acknowledge_signoff_neighbor_adjacent++;
    assert(neighbor.isInitialized());
    LOG_INFO("Sending RemoveNeighbor to " + neighbor.getString());

    MinhtonMessageHeader header(getSelfNodeInfo(), neighbor, leaving_event_id_);
    MessageRemoveNeighbor message_remove_neighbor(header, getSelfNodeInfo(), true);
    send(message_remove_neighbor);
  }

  // Updating adjacents
  // Linking our adjacent left and adjacent right together
  if (getRoutingInfo()->getAdjacentLeft().isInitialized()) {
    LOG_INFO("Sending UpdateRight to " + getRoutingInfo()->getAdjacentLeft().getString());

    MinhtonMessageHeader header(getSelfNodeInfo(), getRoutingInfo()->getAdjacentLeft(),
                                leaving_event_id_);

    if (getRoutingInfo()->getAdjacentLeft() == access_->routing_info->getDirectLeftNeighbor()) {
      // left adjacent is our first left neighbor!

      remaining_acknowledge_signoff_neighbor_adjacent++;

      MessageRemoveNeighbor remove_msg(header, getSelfNodeInfo());
      MessageUpdateNeighbors message_update_neighbors(
          header, {std::make_tuple(getRoutingInfo()->getAdjacentRight(),
                                   NeighborRelationship::kAdjacentRight)});
      MessageRemoveAndUpdateNeighbors remove_and_update_msg(header, remove_msg,
                                                            message_update_neighbors, true);
      assert(!remove_and_update_msg.getMessageRemoveNeighbor().getShouldAcknowledge());
      assert(!remove_and_update_msg.getMessageUpdateNeighbors().getShouldAcknowledge());
      send(remove_and_update_msg);
    } else {
      assert(getRoutingInfo()->getAdjacentLeft().getLevel() != getSelfNodeInfo().getLevel());
      remaining_acknowledge_signoff_neighbor_adjacent++;
      MessageUpdateNeighbors message_update_neighbors(
          header,
          {std::make_tuple(getRoutingInfo()->getAdjacentRight(),
                           NeighborRelationship::kAdjacentRight)},
          true);
      send(message_update_neighbors);
    }
  }
  if (getRoutingInfo()->getAdjacentRight().isInitialized()) {
    LOG_INFO("Sending UpdateLeft to " + getRoutingInfo()->getAdjacentRight().getString());

    if (getRoutingInfo()->getAdjacentRight().getLevel() >= getSelfNodeInfo().getLevel()) {
      return remaining_acknowledge_signoff_neighbor_adjacent;  // Our adjacent already received the
                                                               // message as it must be our neighbor
                                                               // // TODO ?
    }
    assert(getRoutingInfo()->getAdjacentRight().getLevel() != getSelfNodeInfo().getLevel());

    MinhtonMessageHeader header(getSelfNodeInfo(), getRoutingInfo()->getAdjacentRight(),
                                leaving_event_id_);
    MessageUpdateNeighbors message_update_neighbors(
        header,
        {std::make_tuple(getRoutingInfo()->getAdjacentLeft(), NeighborRelationship::kAdjacentLeft)},
        true);
    send(message_update_neighbors);

    remaining_acknowledge_signoff_neighbor_adjacent++;
  }

  return remaining_acknowledge_signoff_neighbor_adjacent;
}

void LeaveAlgorithmGeneral::prepareLeavingAsSuccessor(const NodeInfo &node_to_replace) {
  const uint64_t current_leaving_id =
      access_->procedure_info->loadEventId(ProcedureKey::kLeaveProcedure);

  if (in_leave_progress_ || replacing_node_.isInitialized() ||
      access_->get_fsm_state() == kWaitForReplacementOffer ||
      access_->get_fsm_state() == kSignOffFromInlevelNeighbors ||
      access_->get_fsm_state() == kConnectedWaitingParentResponseDirectLeaveWoReplacement ||
      access_->get_fsm_state() == kConnectedWaitingParentResponse) {
    // Already replacing someone else
    MinhtonMessageHeader header(getSelfNodeInfo(), node_to_replace, current_leaving_id);
    MessageReplacementNack nack(header);
    send(nack);
    return;
  }

  leaving_event_id_ = current_leaving_id;
  replacing_node_ = node_to_replace;
  signOffFromParent();
}

void LeaveAlgorithmGeneral::signOffFromParent() {
  in_leave_progress_ = true;
  const NodeInfo &parent = getRoutingInfo()->getParent();

  // TODO: remove adjacent from signoff and wait until parent has locked itself and its right
  // neighbor
  MinhtonMessageHeader header(getSelfNodeInfo(), parent, leaving_event_id_);
  MessageSignoffParentRequest req(header);
  send(req);
  // TODO Set timeout
}

void LeaveAlgorithmGeneral::processSignOffParentRequest(
    const MessageSignoffParentRequest &message) {
  if (access_->node_locked ||
      (current_signoff_request_ && current_signoff_request_->getSender().isInitialized())) {
    MinhtonMessageHeader header(getSelfNodeInfo(), message.getSender(),
                                message.getHeader().getRefEventId());
    MessageSignoffParentAnswer ans(header, false);
    send(ans);
    return;
  }

  current_signoff_request_ = std::make_shared<MessageSignoffParentRequest>(message);
  access_->node_locked = true;

  if (getRoutingInfo()->getSelfNodeInfo().getLevel() == 0 &&
      getRoutingInfo()->getSelfNodeInfo().getNumber() == 0 && message.getSender().getLevel() == 1 &&
      message.getSender().getNumber() == 0) {
    // When we are the root node and the successor is 1:0, we don't need to lock other nodes, since
    // there are no other nodes in the network
    NodeInfo dummy_node = getSelfNodeInfo();
    dummy_node.setLogicalNodeInfo(LogicalNodeInfo());
    MinhtonMessageHeader header(dummy_node, dummy_node);
    MessageLockNeighborResponse resp(header, true);
    remaining_lock_neighbor_response_ = 1;
    processLockNeighborResponse(resp);
  } else {
    // Lock right node // TODO: maybe only needed if all our children are full (protect for joins)
    auto right_neighbor = getRoutingInfo()->getDirectRightNeighbor();
    locked_right_neighbor_ = true;

    if (right_neighbor.isValidPeer()) {
      assert(right_neighbor.isInitialized());
      MinhtonMessageHeader header(getSelfNodeInfo(), right_neighbor,
                                  current_signoff_request_->getHeader().getRefEventId());
      MessageLockNeighborRequest req(header);
      send(req);
    } else {
      // we are the last node on the level, forward the lock neighbor request to the first node on
      // the next level
      NodeInfo info(getSelfNodeInfo().getLevel() + 1, 0, getSelfNodeInfo().getFanout());
      MinhtonMessageHeader header(getSelfNodeInfo(), info,
                                  current_signoff_request_->getHeader().getRefEventId());
      MessageLockNeighborRequest req(header);
      access_->perform_search_exact(info, std::make_shared<MessageSEVariant>(req));
    }
  }
  remaining_lock_neighbor_response_ = 2;
}

void LeaveAlgorithmGeneral::processLockNeighborRequest(const MessageLockNeighborRequest &message) {
  MinhtonMessageHeader header(getSelfNodeInfo(), message.getSender(),
                              message.getHeader().getRefEventId());

  bool successful = false;
  if (access_->node_locked /*|| !getRoutingInfo()->getInitializedChildren().empty()*/) {
    successful = false;
  } else {
    successful = true;
    access_->node_locked = true;
  }

  MessageLockNeighborResponse resp(header, successful);
  send(resp);
}

void LeaveAlgorithmGeneral::processLockNeighborResponse(
    const MessageLockNeighborResponse &message) {
  assert(current_signoff_request_);
  assert(access_->node_locked);

  if (remaining_lock_neighbor_response_ == 2) {
    // Response is from right
    if (!message.wasSuccessful()) {
      MinhtonMessageHeader header(getSelfNodeInfo(), current_signoff_request_->getSender(),
                                  current_signoff_request_->getHeader().getRefEventId());
      MessageSignoffParentAnswer ans(header, message.wasSuccessful());
      send(ans);
      current_signoff_request_ = nullptr;
      access_->node_locked = false;
      locked_right_neighbor_ = false;
      remaining_lock_neighbor_response_ = 0;
      return;
    }
  }

  if (remaining_lock_neighbor_response_ == 1) {
    // Response is from left or nothing needed to be locked
    assert(message.wasSuccessful());  // TODO Unlock right again if this ever happens
  }

  assert(remaining_lock_neighbor_response_ > 0);
  remaining_lock_neighbor_response_--;

  if (remaining_lock_neighbor_response_ == 1) {
    // Lock left node // TODO Maybe only needed if we only have one children remaining (procet for
    // other leaves)
    auto target = getRoutingInfo()->getLeftRoutingTableNeighborsRightToLeft();
    if (!target.empty()) {
      locked_left_neighbor_ = true;
      assert(target[0].isInitialized());
      assert(target[0].getLevel() != 0);
      MinhtonMessageHeader header(getSelfNodeInfo(), target[0],
                                  current_signoff_request_->getHeader().getRefEventId());
      MessageLockNeighborRequest req(header);
      send(req);
    } else if (getSelfNodeInfo().getLevel() > 0) {  // Not root
      // we are the first node on the level, forward the lock neighbor request to the last node on
      // the previous level
      NodeInfo info(getSelfNodeInfo().getLevel() - 1,
                    pow(getSelfNodeInfo().getFanout(), getSelfNodeInfo().getLevel() - 1) - 1,
                    getSelfNodeInfo().getFanout());
      MinhtonMessageHeader header(getSelfNodeInfo(), info,
                                  current_signoff_request_->getHeader().getRefEventId());
      MessageLockNeighborRequest req(header);
      locked_left_neighbor_ = true;

      access_->perform_search_exact(info, std::make_shared<MessageSEVariant>(req));
    } else {
      remaining_lock_neighbor_response_ = 0;
    }
  }

  if (remaining_lock_neighbor_response_ != 0) {
    return;
  }

  MinhtonMessageHeader temp_header(current_signoff_request_->getSender(), getSelfNodeInfo(),
                                   current_signoff_request_->getHeader().getRefEventId());

  // TODO Embed MessageRemoveNeighbor in MessageSignoffParentRequest
  MessageRemoveNeighbor remove(temp_header, current_signoff_request_->getSender(), true);

  access_->recv(remove);
  uint32_t ack_remove_neighbor = getRoutingInfo()->getRoutingTableNeighbors().size();
  access_->wait_for_acks(ack_remove_neighbor, [this]() { processRemoveNeighborAck(); });
}

void LeaveAlgorithmGeneral::processUpdateForwardAck() {
  assert(last_replacement_update_);
  MinhtonMessageHeader header(getSelfNodeInfo(), last_replacement_update_->getSender(),
                              last_replacement_update_->getHeader().getRefEventId());
  MessageRemoveNeighborAck ack(header);
  send(ack);
  last_replacement_update_ = nullptr;
}

void LeaveAlgorithmGeneral::processRemoveNeighborAck() {
  assert(current_signoff_request_);
  MinhtonMessageHeader header(getSelfNodeInfo(), current_signoff_request_->getSender(),
                              current_signoff_request_->getHeader().getRefEventId());
  MessageSignoffParentAnswer ans(header, true);
  send(ans);
}

void LeaveAlgorithmGeneral::processSignOffParentAnswer(const MessageSignoffParentAnswer &message) {
  if (!message.wasSuccessful()) {
    if (replacing_node_.isInitialized()) sendNackToReplacement(leaving_event_id_);
    replacing_node_ = NodeInfo();
    in_leave_progress_ = false;
    return;
  }

#if 0
  std::cout << "\t CURRENT SIGNOFF " << getSelfNodeInfo().getLevel() << ":" << getSelfNodeInfo().getNumber() << std::endl;
#endif

  uint32_t acks_signoff_neighbor_adjacent = signOffFromNeighborsAndAdjacents();

  access_->wait_for_acks(acks_signoff_neighbor_adjacent,
                         [this]() { processReceiveSignoffNeighborAdjacentsAck(); });
}

void LeaveAlgorithmGeneral::processReceiveSignoffNeighborAdjacentsAck() {
  if (replacing_node_.isInitialized()) {
    sendReplacementOffer();
  } else {
    access_->set_new_fsm(FiniteStateMachine(kIdle));

    // Unlock parent
    MinhtonMessageHeader header(getSelfNodeInfo(), getRoutingInfo()->getParent(),
                                leaving_event_id_);
    MessageUnlockNeighbor unlock(header);
    send(unlock);

    getRoutingInfo()->resetPosition(leaving_event_id_);
    LOG_NODE(getSelfNodeInfo());

    assert(!current_signoff_request_ ||
           !current_signoff_request_->getSender().isInitialized());  // Should never happen
    in_leave_progress_ = false;
    replacing_node_ = NodeInfo();
    leaving_event_id_ = 0;
  }
}

void LeaveAlgorithmGeneral::processReceiveReplacementUpdateAck() {
  // Unlock parent
  MinhtonMessageHeader header(getSelfNodeInfo(), old_parent_, leaving_event_id_);
  if (old_parent_.getLogicalNodeInfo() == getSelfNodeInfo().getLogicalNodeInfo()) {
    header.setTarget(getSelfNodeInfo());
  }
  MessageUnlockNeighbor unlock(header);
  send(unlock);

  in_leave_progress_ = false;
  leaving_event_id_ = 0;
  replacing_node_ = NodeInfo();
}

void LeaveAlgorithmGeneral::sendNackToReplacement(uint64_t ref_event_id) {
  MinhtonMessageHeader header(getSelfNodeInfo(), replacing_node_, ref_event_id);
  MessageReplacementNack nack(header);
  send(nack);
}

void LeaveAlgorithmGeneral::sendReplacementOffer() {
  MinhtonMessageHeader header(getSelfNodeInfo(), replacing_node_, leaving_event_id_);
  MessageReplacementOffer message_replacement_offer(header);

  // triggering timeout if we dont receive a replacement ack back within a certain period of time
  access_->set_timeout(TimeoutType::kReplacementAckResponseTimeout);

  send(message_replacement_offer);
}

void LeaveAlgorithmGeneral::forwardFindReplacementMessage(NodeInfo forward_to,
                                                          NodeInfo node_to_replace,
                                                          SearchProgress leave_case) {
  uint64_t ref_event_id = 0;
  if (access_->procedure_info->hasKey(ProcedureKey::kLeaveProcedure)) {
    ref_event_id = access_->procedure_info->loadEventId(ProcedureKey::kLeaveProcedure);
  }
  MinhtonMessageHeader header(getSelfNodeInfo(), forward_to, ref_event_id);

  MessageFindReplacement message_find_replacement(header, node_to_replace, leave_case);
  if (ref_event_id == 0) {
    auto event_id = message_find_replacement.getHeader().getEventId();
    access_->procedure_info->saveEventId(ProcedureKey::kLeaveProcedure, event_id);
    LOG_EVENT(EventType::kLeaveEvent, event_id);
  }
  send(message_find_replacement);
}

void LeaveAlgorithmGeneral::replaceMyself(NodeInfo node_to_replace,
                                          std::vector<NodeInfo> neighbors_of_node_to_replace) {
  auto ref_event_id = access_->procedure_info->loadEventId(ProcedureKey::kLeaveProcedure);
  auto old_self_node_info = getSelfNodeInfo();

  getRoutingInfo()->setNodeStatus(NodeStatus::kLeft, ref_event_id);

  getRoutingInfo()->setPosition(LogicalNodeInfo(node_to_replace.getLogicalNodeInfo().getLevel(),
                                                node_to_replace.getLogicalNodeInfo().getNumber(),
                                                node_to_replace.getLogicalNodeInfo().getFanout()));

  // replacing self_node_info position (PhysicalNodeInfo stays the same!), resetting routing_info
  // we are a different peer now
  getRoutingInfo()->setNodeStatus(NodeStatus::kRunning, ref_event_id);
  LOG_NODE(getSelfNodeInfo());

  neighbors_of_node_to_replace.erase(
      std::remove_if(neighbors_of_node_to_replace.begin(), neighbors_of_node_to_replace.end(),
                     [&](const NodeInfo &node) {
                       return node.getLogicalNodeInfo() == old_self_node_info.getLogicalNodeInfo();
                     }),
      neighbors_of_node_to_replace.end());

  /// Determining what role each neighbor has: parent, child, routing table neighor, routing table
  /// neighbor child
  for (auto &neighbor : neighbors_of_node_to_replace) {
    getRoutingInfo()->updateNeighbor(neighbor, ref_event_id);
  }

  /// Updating our Adjacent Information separately
  auto our_new_adj_left = LeaveAlgorithmGeneral::getAdjacentLeftFromVector(
      getSelfNodeInfo(), neighbors_of_node_to_replace);
  auto our_new_adj_right = LeaveAlgorithmGeneral::getAdjacentRightFromVector(
      getSelfNodeInfo(), neighbors_of_node_to_replace);

  if (our_new_adj_left.isInitialized()) {
    getRoutingInfo()->setAdjacentLeft(our_new_adj_left, ref_event_id);
  }

  if (our_new_adj_right.isInitialized()) {
    getRoutingInfo()->setAdjacentRight(our_new_adj_right, ref_event_id);
  }
}

}  // namespace minhton
