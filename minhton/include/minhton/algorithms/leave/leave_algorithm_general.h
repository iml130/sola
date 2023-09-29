// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_LEAVE_ALGORITHM_GENERAL_H_
#define MINHTON_ALGORITHMS_LEAVE_ALGORITHM_GENERAL_H_

#include <memory>
#include <optional>

#include "minhton/algorithms/leave/interface_leave_algorithm.h"
#include "minhton/message/find_replacement.h"
#include "minhton/message/lock_neighbor_request.h"
#include "minhton/message/lock_neighbor_response.h"
#include "minhton/message/message.h"
#include "minhton/message/remove_neighbor.h"
#include "minhton/message/remove_neighbor_ack.h"
#include "minhton/message/replacement_ack.h"
#include "minhton/message/replacement_nack.h"
#include "minhton/message/replacement_offer.h"
#include "minhton/message/replacement_update.h"
#include "minhton/message/signoff_parent_answer.h"
#include "minhton/message/signoff_parent_request.h"
#include "minhton/message/unlock_neighbor.h"
#include "minhton/message/update_neighbors.h"

namespace minhton {

class LeaveAlgorithmGeneral : public LeaveAlgorithmInterface {
public:
  explicit LeaveAlgorithmGeneral(std::shared_ptr<AccessContainer> access)
      : LeaveAlgorithmInterface(access){};

  ~LeaveAlgorithmGeneral() override = default;

  void process(const MessageVariant &msg) override;

  /// We decide ourselves that we want to leave the network
  /// and initiate the leave procedure.
  ///
  /// This method gets called when we receive a leave signal.
  ///
  /// Typical Usage:
  /// \code
  ///   node->initiateSelfDeparture();
  /// \endcode
  ///
  void initiateSelfDeparture() override = 0;

protected:
  /// This method will be called when we receive a FIND_REPLACEMENT message.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processFindReplacement(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  virtual void processFindReplacement(const minhton::MessageFindReplacement &message) = 0;

  /// This method will be called when we receive a REPLACEMENT_OFFER message.
  ///
  /// We are the node which wants to leave the network. A node which can replace
  /// our position gave us an offer.
  /// If we send back a ReplacementAck we can just leave the network.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processReplacementOffer(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processReplacementOffer(const minhton::MessageReplacementOffer &message);

  /// This method will be called when we receive a SIGN_OFF_PARENT_REQUEST message.
  ///
  /// We are the parent of a selected successor node for a leave procedure. We might respond with a
  /// MessageSignoffParentAnswer in case we are already involved in another leave procedure.
  /// Otherwise, we continue the leave by locking our right neighbor with a
  /// MessageLockNeighborRequest. If we and the successor are the only nodes in the network, we skip
  /// the locking of our neighbors and send a MessageLockNeighborResponse to ourselves to continue.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processSignOffParentRequest(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processSignOffParentRequest(const minhton::MessageSignoffParentRequest &message);

  /// This method will be called when we receive a SIGN_OFF_PARENT_ANSWER message.
  ///
  /// We are the selected successor node for a leave procedure. If the message informs us of a
  /// failure, we abort the leave. Otherwise, we continue by calling the method
  /// signOffFromNeighborsAndAdjacents. Then we wait for all acknowledgements for the routing
  /// information updates of the nodes we sent messages to. We continue by calling the method
  /// processReceiveSignoffNeighborAdjacentsAck.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processSignOffParentAnswer(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processSignOffParentAnswer(const minhton::MessageSignoffParentAnswer &message);

  /// This method will be called when we receive a REPLACEMENT_ACK message.
  ///
  /// We are the node which is replacing the leaving node.
  /// We can now initialize the whole replacement procedure.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processReplacementAck(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processReplacementAck(const minhton::MessageReplacementAck &message);

  /// This method will be called when we receive a LOCK_NEIGHBOR_REQUEST message.
  ///
  /// We are the left or right neighbor of the parent of a successor node. We try to lock ourselves
  /// for the leave procedure and respond with a MessageLockNeighborResponse, indicating success or
  /// failure of locking ourselves.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processLockNeighborRequest(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processLockNeighborRequest(const minhton::MessageLockNeighborRequest &message);

  /// This method will be called when we receive a LOCK_NEIGHBOR_RESPONSE message.
  ///
  /// We are the parent of a successor node. When the message indicates a failure, we inform the
  /// successor about it and abort the leave. If the response comes from the right neighbor, we
  /// continue by sending a MessageLockNeighborRequest to our left neighbor. If the response comes
  /// from the left neighbor, we send a MessageRemoveNeighbor to our level neighbors and wait for
  /// all acknowledgements.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processLockNeighborResponse(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processLockNeighborResponse(const minhton::MessageLockNeighborResponse &message);

  /// This method will be called after we received all expected REMOVE_NEIGHBOR_ACK messages.
  ///
  /// We are the parent of a successor node and inform the successor about the successful updates of
  /// all our level neighbors with a MessageSignoffParentAnswer.
  void processRemoveNeighborAck();

  /// This method will be called when we receive a REPLACEMENT_UPDATE message.
  ///
  /// Removing the node of the removed position from our routing information if we have it.
  ///
  /// Replacing the PhysicalNodeInfo from the neighbor at the replaced position
  /// with those from the removed position.
  /// (Exchanging PhysicalNodeInfo of the leaving node with that of the replacing node.)
  ///
  /// Typical Usage:
  /// \code
  ///   this->processReplacementUpdate(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processReplacementUpdate(const minhton::MessageReplacementUpdate &message);

  /// We know that we can leave the network without making the tree unbalanced.
  ///
  /// For this we only have to send RemoveNeighbor messages to each
  /// neighbor who knows us and update our adjacents.
  ///
  /// We can update the adjacents by linking our adjacent left and adjacent right together.
  ///
  void performLeaveWithoutReplacement();

  ///
  /// Performing the update of every PhysicalNodeInfo in a leave with replacement
  /// while we are the node replacing the leaving node.
  ///
  /// Sending RemoveNeighbor messages to nodes who only know the removed position,
  /// and ReplacementUpdate messages to nodes who know the replaced (and removed) position.
  /// If we do not have the network information about the nodes, we send via SearchExact.
  ///
  /// We also must update the adjacents of the removed position
  ///
  /// \param leaving_node The node leaving the network and position we will replace
  /// \param neighbors_of_leaving_node All neighbors the leaving node knows
  ///
  void performLeaveWithReplacement(minhton::NodeInfo leaving_node,
                                   std::vector<minhton::NodeInfo> neighbors_of_leaving_node,
                                   bool leaving_node_was_locked, bool leaving_node_locked_right,
                                   bool leaving_node_locked_left);

  /// Helper method to prepare leaving the network in order to replace another node. This calls
  /// signOffFromParent
  ///
  /// \param node_to_replace The node leaving the network
  void prepareLeavingAsSuccessor(const minhton::NodeInfo &node_to_replace);

  /// Send a MessageSignoffParentRequest to own parent.
  /// This helper method can be called from the successor or the leaving node itself if it doesn't
  /// need a successor.
  void signOffFromParent();

  /// This method is called from the processSignOffParentAnswer method.
  ///
  /// We are a successor for a leave procedure and are now updating our neighbors. Our level
  /// neighbors receive a MessageRemoveNeighbor, which they have to acknowledge. Next, we update our
  /// adjacents by sending the left one a MessageRemoveAndUpdateNeighbors (if it is also our left
  /// neighbor) or MessageUpdateNeighbors. We send the right adjacent MessageUpdateNeighbors. The
  /// adjacents have to acknowledge their routing information update as well.
  ///
  /// \returns uint32_t number of nodes we expect an acknowledgement from
  uint32_t signOffFromNeighborsAndAdjacents();

  /// This method is called after we received all expected acknowledgements for a
  /// MessageReplacementUpdate, or directly after receiving a MessageReplacementUpdate, if we don't
  /// have to forward it.
  ///
  /// In this method we simply send a MessageRemoveNeighborAck to the node which sent us the last
  /// MessageReplacementUpdate.
  void processUpdateForwardAck();

  /// Helper method to send the replacement offer message
  /// to the node who wants to leave the network.
  ///
  /// Also setting the correct event id for the message
  /// and setting the replacement ack timeout.
  void sendReplacementOffer();

  /// This method sends a MessageReplacementNack to the successor, if we need to abort the leave
  /// procedure.
  ///
  /// \param ref_event_id ref event id of the relevant leave procedure
  void sendNackToReplacement(uint64_t ref_event_id);

  /// Replacing our own self_node_info and routing_info with the information
  /// that we got from the node to replace positions with.
  ///
  /// But we keep our current PhysicalNodeInfo!
  ///
  /// We have to iterate through the given neighbor information and determine
  /// if the neighbor is a parent, child, adjacent, routing table neighbor etc.
  ///
  /// \param node_to_replace the node position we want to replace
  /// \param neighbors_of_node_to_replace every neighbor the node to replace knew of
  ///
  /// TODO  sts
  void replaceMyself(const minhton::NodeInfo &node_to_replace,
                     std::vector<minhton::NodeInfo> neighbors_of_node_to_replace) override;

  /// This method is called after we received all acknowledgements from our neighbors and adjacents
  /// where we updated their routing information.
  ///
  /// We are the sucessor for a leave procedure and continue by sending a replacement offer to the
  /// node we want to replace. But if we can leave without a replacement, we are the leaving node
  /// and can leave directly, finishing our part of the leave by going into idle and unlocking our
  /// parent.
  void processReceiveSignoffNeighborAdjacentsAck();

  /// This method is called after we received all acknowledgements for each MessageReplacementUpdate
  /// we sent to our neighbors.
  ///
  /// We are the successor which is now at the position of the left node and we can unlock our
  /// parent.
  void processReceiveReplacementUpdateAck();

  /// This method will be called when we receive a UNLOCK_NEIGHBOR message.
  ///
  /// We are either the parent of a successor of the left/right neighbor of the parent of a
  /// successor and are now allowed to unlock ourselves. In case we are the parent, we need to
  /// forward this message to our left and right neighbor.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processUnlockNeighbor(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processUnlockNeighbor(const MessageUnlockNeighbor &msg);

  // TODO Temporary
  minhton::NodeInfo replacing_node_;
  uint64_t leaving_event_id_ = 0;
  bool in_leave_progress_ = false;
  bool locked_right_neighbor_ = false;
  bool locked_left_neighbor_ = false;
  uint32_t remaining_lock_neighbor_response_ = 0;
  std::shared_ptr<minhton::MessageSignoffParentRequest> current_signoff_request_ = nullptr;

  std::shared_ptr<minhton::MessageReplacementUpdate> last_replacement_update_ = nullptr;
  NodeInfo old_parent_;

  /// Calculating the adjacent left of the considered node,
  /// by looking which node is closest to its left,
  /// by using the tree mapper.
  ///
  /// Typical usage:
  /// \code
  ///   auto adj_left = this->routing_info_.getAdjacentLeftFromVector(considered_node,
  ///   considered_node_neighbors);
  /// \endcode
  ///
  /// \param considered_node of whom we want to calculate the adjacent left
  /// \param neighbors we know of the considered node
  ///
  /// \returns NodeInfo object which should be the adjacent left
  ///
  static minhton::NodeInfo getAdjacentLeftFromVector(const minhton::NodeInfo &considered_node,
                                                     std::vector<minhton::NodeInfo> neighbors);

  /// Calculating the adjacent right of the considered node,
  /// by looking which node is closest to its right,
  /// by using the tree mapper.
  ///
  /// Typical usage:
  /// \code
  ///   auto adj_right = this->routing_info_.getAdjacentRightFromVector(considered_node,
  ///   considered_node_neighbors);
  /// \endcode
  ///
  /// \param considered_node of whom we want to calculate the adjacent right
  /// \param neighbors we know of the considered node
  ///
  /// \returns NodeInfo object which should be the adjacent right
  ///
  static minhton::NodeInfo getAdjacentRightFromVector(const minhton::NodeInfo &considered_node,
                                                      std::vector<minhton::NodeInfo> neighbors);
};

}  // namespace minhton

#endif
