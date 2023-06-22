// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/join/join_algorithm_general.h"

#include <cassert>

#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"
#include "minhton/message/types_all.h"

namespace minhton {

void JoinAlgorithmGeneral::process(const MessageVariant &msg) {
  std::visit(
      Overload{
          [this](const MessageJoin &specific_msg) { processJoin(specific_msg); },
          [this](const MessageJoinAccept &specific_msg) { processJoinAccept(specific_msg); },
          [this](const MessageJoinAcceptAck &specific_msg) { processJoinAcceptAck(specific_msg); },
          [](auto &specific_msg) {
            throw AlgorithmException("Wrong Algorithm Interface process called");
          }},
      msg);
}

void JoinAlgorithmGeneral::initiateJoin(NodeInfo &node_info) {
  if (node_info.getAddress() == "255.255.255.255") {  // TODO 255. workaround
    // Should be retry
    assert(last_join_info_.isInitialized());
    node_info.setPhysicalNodeInfo(last_join_info_);
  } else {
    last_join_info_ = node_info.getPhysicalNodeInfo();
  }

  this->access_->set_timeout(TimeoutType::kJoinAcceptResponseTimeout);

  MinhtonMessageHeader header(getRoutingInfo()->getSelfNodeInfo(), node_info);
  MessageJoin msg_join(header, getRoutingInfo()->getSelfNodeInfo());
  this->send(msg_join);
}

void JoinAlgorithmGeneral::initiateJoin(const PhysicalNodeInfo &p_node_info) {
  NodeInfo target_node_info;
  target_node_info.setPhysicalNodeInfo(p_node_info);
  initiateJoin(target_node_info);
}

void JoinAlgorithmGeneral::performAcceptChild(NodeInfo entering_node,
                                              bool use_complete_balancing) noexcept(false) {
  /// CHECK

  /// We need to check here first that our FSM state is kConnected. Nothing else!
  /// E.g. if we are in the kConnectedReplacing, we are just trying to replace another node, and
  /// accepting a new child at the same time would be a very bad idea. How would we forward the join
  /// message backwards if this happens?

  /// Also if we are in kConnectedAcceptingChild state, we might be accepting too many
  /// children at the same time.
  /// Important Question:
  /// What should we do, if the network is getting
  /// too many join requests at the same time, and the join algorithm
  /// is forwarding all of them to the same node - so that all join messages
  /// end up at the same node, who thinks he could accept all of them.
  assert(!access_->node_locked);
  access_->node_locked = true;

  // get and set position of new child
  auto new_child_position = calcNewChildPosition(use_complete_balancing);
  if (!new_child_position.isValidPeer()) {
    throw AlgorithmException(
        AlgorithmType::kJoinAlgorithm,
        "We have no free child position, but want to accept this child. Caused "
        "by an error in sweep join procedure!");
  }
  entering_node.setPosition(new_child_position.getLogicalNodeInfo());

  // calculate new adjacents for entering node locally
  // they are the clostest that we know
  // but might be wrong and we need to check
  auto entering_node_adjacent_left = calcAdjacentLeftOfNewChild(entering_node);
  auto entering_node_adjacent_right = calcAdjacentRightOfNewChild(entering_node);

  // checking if there are closer adjacents for entering node we might not know about locally
  auto closer_adj_left = getCloserAdjacent(entering_node, entering_node_adjacent_left);
  auto closer_adj_right = getCloserAdjacent(entering_node, entering_node_adjacent_right);

  bool adj_left_wrong = closer_adj_left.isValidPeer();
  bool adj_right_wrong = closer_adj_right.isValidPeer();

  if (adj_left_wrong && adj_right_wrong) {
    throw AlgorithmException(
        AlgorithmType::kUpdatingAlgorithm,
        "Both adjacents may not be wrong at the same time in join accept procedure!");
  }

  // this key should not be in use yet
  // because we may not do two join accept procedures at the same time
  if (this->access_->procedure_info->hasKey(ProcedureKey::kAcceptChildProcedure)) {
    throw AlgorithmException(AlgorithmType::kUpdatingAlgorithm,
                             "There is already a join accept procedure state saved.");
  }

  // saving our join accept procedure information for when we received the needed information
  auto state_nodes = {entering_node, entering_node_adjacent_left, entering_node_adjacent_right};
  this->access_->procedure_info->save(ProcedureKey::kAcceptChildProcedure, state_nodes);

  if (!adj_left_wrong && !adj_right_wrong) {
    // both adjacents are correct
    // and we can simply send the join accept message
    this->performSendJoinAccept(entering_node, entering_node_adjacent_left,
                                entering_node_adjacent_right);

  } else {
    // one adjacent is wrong

    NodeInfo target;
    NeighborRelationship relationship;
    if (adj_left_wrong) {
      target = entering_node_adjacent_left;
      relationship = NeighborRelationship::kAdjacentRight;
    }

    if (adj_right_wrong) {
      target = entering_node_adjacent_right;
      relationship = NeighborRelationship::kAdjacentLeft;
    }

    // send get neighbor to get information about the correct adjacent
    MinhtonMessageHeader header(
        getSelfNodeInfo(), target,
        this->access_->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));
    MessageGetNeighbors message_get_neighbors(header, getSelfNodeInfo(), {relationship});
    this->send(message_get_neighbors);
  }
}

void JoinAlgorithmGeneral::continueAcceptChildProcedure(
    const MessageInformAboutNeighbors &message) noexcept(false) {
  // loading the information from the started join accept procedure
  auto state = this->access_->procedure_info->load(ProcedureKey::kAcceptChildProcedure);
  auto entering_node = state[0];
  auto entering_node_adj_left_local = state[1];
  auto entering_node_adj_right_local = state[2];

  // getting the new information
  auto requested_adjacent = message.getRequestedNeighbors()[0];

  // checking if there is really no closer adjacent
  // otherwise we might still not have the closest adjacent
  auto closer_adj = getCloserAdjacent(entering_node, requested_adjacent);
  if (closer_adj.isValidPeer()) {
    throw AlgorithmException(AlgorithmType::kUpdatingAlgorithm,
                             "We still do not have the closest adjacent.");
  }

  // ordering our information
  NodeInfo correct_adj_left;
  NodeInfo correct_adj_right;

  // left adjacent of entering node
  if (requested_adjacent.getLogicalNodeInfo() < entering_node.getLogicalNodeInfo()) {
    correct_adj_left = requested_adjacent;
    correct_adj_right = entering_node_adj_right_local;
  } else {
    // right adjacent of entering node
    correct_adj_left = entering_node_adj_left_local;
    correct_adj_right = requested_adjacent;
  }

  // updating the procedure state just in case if still something wrong might happen
  auto new_state = {entering_node, correct_adj_left, correct_adj_right};
  this->access_->procedure_info->update(ProcedureKey::kAcceptChildProcedure, new_state);

  // sending join accept
  this->performSendJoinAccept(entering_node, correct_adj_left, correct_adj_right);
}

void JoinAlgorithmGeneral::performSendJoinAccept(const NodeInfo &entering_node,
                                                 const NodeInfo &entering_node_adj_left,
                                                 const NodeInfo &entering_node_adj_right) {
  MinhtonMessageHeader header(getSelfNodeInfo(), entering_node,
                              access_->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));

  // calculating the routing table neighbors
  // we have information to all of them through our routing table neighbor childs
  // and the baton theorem
  auto entering_node_routing_table_neighbors = getRoutingTableNeighborsForNewChild(entering_node);

  // setting timeout
  this->access_->set_timeout(TimeoutType::kJoinAcceptAckResponseTimeout);

  MessageJoinAccept message_join_accept(header, getSelfNodeInfo().getFanout(),
                                        entering_node_adj_left, entering_node_adj_right,
                                        entering_node_routing_table_neighbors);
  this->send(message_join_accept);
}

/// We are the entering node and received this message from our new parent.
///
/// Target contains the logical position about ourselves
/// which our parent calculated for us.
///
/// The sender is our parent.
void JoinAlgorithmGeneral::processJoinAccept(const MessageJoinAccept &msg) {
  this->access_->cancel_timeout(TimeoutType::kJoinAcceptResponseTimeout);

  auto our_new_parent = msg.getSender();
  auto our_new_position = msg.getTarget();
  auto our_new_adjacent_left = msg.getAdjacentLeft();
  auto our_new_adjacent_right = msg.getAdjacentRight();
  auto routing_table_neighbors = msg.getRoutingTableNeighbors();

  // set our position
  getRoutingInfo()->setPosition(our_new_position.getLogicalNodeInfo());
  // logging that we are now an existing peer
  LOG_NODE(getSelfNodeInfo());

  getRoutingInfo()->setNodeStatus(kRunning, msg.getHeader().getRefEventId());

  // setting our parent
  getRoutingInfo()->setParent(our_new_parent, msg.getHeader().getRefEventId());

  // setting our adjacents
  if (our_new_adjacent_left.isInitialized()) {
    getRoutingInfo()->setAdjacentLeft(our_new_adjacent_left, msg.getHeader().getRefEventId());
  }
  if (our_new_adjacent_right.isInitialized()) {
    getRoutingInfo()->setAdjacentRight(our_new_adjacent_right, msg.getHeader().getRefEventId());
  }

  // setting our routing table neighbors
  for (auto const &routing_table_neighbor : routing_table_neighbors) {
    getRoutingInfo()->updateRoutingTableNeighbor(routing_table_neighbor,
                                                 msg.getHeader().getRefEventId());
  }

  // sending join accept ack message back to our parent
  MinhtonMessageHeader header(getSelfNodeInfo(), getRoutingInfo()->getParent(),
                              msg.getHeader().getRefEventId());
  MessageJoinAcceptAck msg_temp(header);
  this->send(msg_temp);

  last_join_info_ = PhysicalNodeInfo();
}

/// We are the parent of the node that has entered the network
/// and now need to send around information through the network
/// to inform other nodes about the entered node
/// and update our own adjacent information.
void JoinAlgorithmGeneral::processJoinAcceptAck(const MessageJoinAcceptAck &msg) noexcept(false) {
  auto entering_node = msg.getSender();

  if (!this->access_->procedure_info->hasKey(ProcedureKey::kAcceptChildProcedure)) {
    throw AlgorithmException(AlgorithmType::kUpdatingAlgorithm,
                             "There is no saved join accept procedure state");
  }

  // received join accept ack, therefore cancelling the timeout
  this->access_->cancel_timeout(TimeoutType::kJoinAcceptAckResponseTimeout);

  // loading our saved procedure information
  auto state = this->access_->procedure_info->load(ProcedureKey::kAcceptChildProcedure);
  auto entering_node_adjacent_left = state[1];
  auto entering_node_adjacent_right = state[2];

  // calculate whether we have send messages to update the adjacent
  bool send_update_adjacent_left = this->mustSendUpdateLeft(entering_node_adjacent_right);
  bool send_update_adjacent_right = this->mustSendUpdateRight(entering_node_adjacent_left);

  uint32_t required_acks = 0;

  // sending UPDATE_LEFT and UPDATE_RIGHT messages
  if (send_update_adjacent_right) {
    MinhtonMessageHeader header(getSelfNodeInfo(), entering_node_adjacent_left,
                                access_->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));
    MessageUpdateNeighbors message_update_neighbors(
        header, {std::make_tuple(entering_node, NeighborRelationship::kAdjacentRight)}, true);
    this->send(message_update_neighbors);

    required_acks++;
  }
  if (send_update_adjacent_left) {
    MinhtonMessageHeader header(getSelfNodeInfo(), entering_node_adjacent_right,
                                access_->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));
    MessageUpdateNeighbors message_update_neighbors(
        header, {std::make_tuple(entering_node, NeighborRelationship::kAdjacentLeft)}, true);
    this->send(message_update_neighbors);

    required_acks++;
  }

  // calculate our new adjacents, depending on the adjacents of the entering node
  auto our_adj_left = calcOurNewAdjacentLeft(entering_node, entering_node_adjacent_right);
  auto our_adj_right = calcOurNewAdjacentRight(entering_node, entering_node_adjacent_left);

  // set our new information
  uint16_t child_index =
      entering_node.getLogicalNodeInfo().getNumber() % getSelfNodeInfo().getFanout();
  getRoutingInfo()->setChild(entering_node, child_index, msg.getHeader().getRefEventId());

  if (our_adj_left.isInitialized()) {
    getRoutingInfo()->setAdjacentLeft(our_adj_left, msg.getHeader().getRefEventId());
  }
  if (our_adj_right.isInitialized()) {
    getRoutingInfo()->setAdjacentRight(our_adj_right, msg.getHeader().getRefEventId());
  }

  // updating the network by sending UPDATE_NEIGHBOR messages
  required_acks += this->performSendUpdateNeighborMessagesAboutEnteringNode(entering_node);

  access_->wait_for_acks(required_acks, [this]() { this->allUpdatesAcknowledged(); });
}

void JoinAlgorithmGeneral::allUpdatesAcknowledged() {
  this->access_->node_locked = false;

  // releasing the procedure key
  // we are open again to accept new children
  this->access_->procedure_info->remove(ProcedureKey::kAcceptChildProcedure);
  this->access_->procedure_info->removeEventId(ProcedureKey::kJoinProcedure);
}

}  // namespace minhton
