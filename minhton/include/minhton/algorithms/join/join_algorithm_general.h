// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_JOIN_ALGORITHM_GENERAL_H_
#define MINHTON_ALGORITHMS_JOIN_ALGORITHM_GENERAL_H_

#include <memory>

#include "minhton/algorithms/join/interface_join_algorithm.h"
#include "minhton/message/types_all.h"

namespace minhton {

class JoinAlgorithmGeneral : public JoinAlgorithmInterface {
public:
  JoinAlgorithmGeneral(std::shared_ptr<AccessContainer> access) : JoinAlgorithmInterface(access){};

  virtual ~JoinAlgorithmGeneral(){};

  void process(const MessageVariant &msg) override;

  /// Sending the initial Join message for a node to join the network.
  /// Only for join via nodeinfo with address, not bootstrap.
  ///
  /// \param node_info node to send initial join to
  void initiateJoin(NodeInfo &node_info) override;

  /// Sending the initial Join message for a node to join the network.
  /// Only for join via address, not bootstrap.
  ///
  /// \param p_node_info address to send initial join to
  void initiateJoin(const PhysicalNodeInfo &p_node_info) override;

protected:
  /// This method will be called when we receive a JOIN message.
  ///
  /// Forwarding JOIN Messages according to the fill join algorithm.
  ///
  /// When this node is a fitting position to accept the entering node,
  /// it will call initialize the JoinAccept procedure.
  /// Otherwise it will further forward the message to the next node.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processJoin(incoming_message);
  /// \endcode
  ///
  /// \param msg the message we received and want to process
  ///
  /// \throw Algorithm Exception if join case couldn't be handled
  virtual void processJoin(const MessageJoin &msg) = 0;

  /// This method will be called when we receive a JOIN_ACCEPT message.
  ///
  /// We previously sent a JOIN message and now get an answer from
  /// our new parent node. The message contains all relevant information
  /// about the network we need to know.
  ///
  /// In the end we must send a processJoinAcceptAck message pack to the parent.
  /// Only after that the parent will update the network informing everybody
  /// that we joined.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processJoinAccept(incoming_message);
  /// \endcode
  ///
  /// \param msg the message we received and want to process
  ///
  void processJoinAccept(const MessageJoinAccept &msg);

  /// This method will be called when we receive a JOIN_ACCEPT_ACK message.
  ///
  /// We are the parent of the entering node. Now we must send update messages
  /// to update the routing information of the whole network.
  ///
  /// For this we sent UPDATE_NEIGHBOR messages.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processJoinAcceptAck(incoming_message);
  /// \endcode
  ///
  /// \param msg the message we received and want to process
  ///
  void processJoinAcceptAck(const MessageJoinAcceptAck &msg);

  /// Initializing the procedure to accept a child.
  ///
  /// Calculating all relevant information for the \p entering_node / new child
  /// and sending those information via a JOIN_ACCEPT Message to the \p entering_node.
  ///
  /// If we do not have information to the adjacents we need to send GET_NEIGHBORS
  /// messages to get the information and pause the procedure.
  ///
  /// Typical Usage:
  /// \code
  ///   this->performAcceptChild(entering_node);
  /// \endcode
  ///
  /// \param entering_node the node who wants to enter the network
  /// \param use_complete_balancing choose if the complete balancing (necessary for the MINHTON
  /// join algorithm) has to be used
  void performAcceptChild(minhton::NodeInfo entering_node, bool use_complete_balancing = false);

  ///
  /// We had to pause the join accept procedure to get information
  /// about the correct adjacents for the entering node.
  ///
  /// We received this information in the InformAboutNeighbors message
  /// and check if we now got actually the closest adjacent.
  ///
  /// If yes we can procede and send the join accept message to the entering node.
  ///
  /// Typical Usage:
  /// \code
  ///   this->continueAcceptChildProcedure(incoming_message);
  /// \endcode
  ///
  /// \param message the message contining information about the correct adjacents
  void continueAcceptChildProcedure(const minhton::MessageInformAboutNeighbors &message) noexcept(
      false) override;

  /// Helper method for the Sweep Join Algorithm to quickly forward the JOIN message to the target
  /// node. If we do not know the target, we send via Search Exact.
  ///
  /// \param target the node we want to forward the message to
  /// \param entering_node the node which wants to join the network
  /// \param target_on_next_to_last_level true if we definitely know that the target of this
  ///         message is on the next to last level
  // void forwardJoinMessage(minhton::NodeInfo target, minhton::NodeInfo entering_node,
  //                         bool target_on_next_to_last_level);

  ///
  /// Gathering all information for the entering node
  /// and sending a JOIN_ACCEPT message.
  ///
  /// Typical Usage:
  /// \code
  ///   this->performSendJoinAccept(entering_node, entering_node_adj_left, entering_node_adj_right);
  /// \endcode
  ///
  /// \param entering_node the node who wants to enter the network
  /// \param entering_node_adj_left adjacent left neighbor of the entering node
  /// \param entering_node_adj_right adjacent right neighbor of the entering node
  void performSendJoinAccept(const minhton::NodeInfo &entering_node,
                             const minhton::NodeInfo &entering_node_adj_left,
                             const minhton::NodeInfo &entering_node_adj_right);

  /// Helper method for the perform accept child procedure.
  ///
  /// Sending UPDATE_ROUTING_TABLE_NEIGHBOR_CHILD messages with the entering_node as the node to
  /// inform to each of our routing table neighbors, because those nodes have the entering_node as a
  /// routing table neighbor child.
  ///
  /// Typical Usage:
  /// \code
  ///   this->performSendUpdateNeighborMessagesAboutEnteringNode(entering_node);
  /// \endcode
  ///
  /// \param entering_node the node who wants to enter the network
  /// \return number of nodes that we contacted with the update
  ///
  virtual uint32_t performSendUpdateNeighborMessagesAboutEnteringNode(
      minhton::NodeInfo entering_node) = 0;

  ///
  /// Returning all existing routing table neighbor information for a new given child.
  ///
  /// When we accept a new child we need to give it also its routing table information.
  ///
  /// Because of the BATON theorem we have information about all the existing routing table
  /// neighbors of the new child through out routing table neighbor children, because we are its
  /// parent.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_.getRoutingTableNeighborsForNewChild(new_child);
  /// \endcode
  ///
  /// \param new_child that we are the parent of
  ///
  /// \returns vector of all NodeInfo objects for the routing table neighbors of the new child
  ///
  std::vector<minhton::NodeInfo> getRoutingTableNeighborsForNewChild(
      const minhton::NodeInfo &new_child);

  ///
  /// Looks for the position of a new free child. The position which is horizontally
  /// closest to root is chosen when the tree has a general null balance. For complete balancing the
  /// first free position from left to right is chosen. This method is only used in the perform
  /// accept child procedure.
  ///
  /// Typical usage:
  /// \code
  ///    minhton::NodeInfo new_child = routingInfo->calcNewChildPosition();
  /// \endcode
  ///
  /// \param use_complete_balancing choose if the complete balancing (necessary for the MINHTON
  /// join algorithm) has to be used
  ///
  /// \returns NodeInfo object or nullptr if there is no free child position
  minhton::NodeInfo calcNewChildPosition(bool use_complete_balancing = false) const;

  ///
  /// Calculates the new adjacent left of the entering node, which we accept as our child.
  ///
  /// Typical usage:
  /// \code
  ///    minhton::NodeInfo entering_node_new_adj_left =
  ///    routingInfo->calcAdjacentLeftOfNewChild();
  /// \endcode
  ///
  /// \param entering_node the node that we accept as a child
  ///
  /// \returns NodeInfo object of adjacent left of new child, or uninitialized NodeInfo if
  /// entering_node doesn't have one
  minhton::NodeInfo calcAdjacentLeftOfNewChild(const minhton::NodeInfo &entering_node);

  ///
  /// Calculates the new adjacent right of the entering node, which we accept as our child.
  ///
  /// Typical usage:
  /// \code
  ///    minhton::NodeInfo entering_node_new_adj_right =
  ///    routingInfo->calcAdjacentRightOfNewChild();
  /// \endcode
  ///
  /// \param entering_node the node that we accept as a child
  ///
  /// \returns NodeInfo object of adjacent right of new child, or uninitialized NodeInfo if
  /// entering_node doesn't have one
  minhton::NodeInfo calcAdjacentRightOfNewChild(const minhton::NodeInfo &entering_node);

  ///
  /// Calculates the new adjacent left for us, which might change when we accept the entering node
  /// as a new child.
  ///
  /// For this we must have already calculated the new adjacent right of the entering node.
  ///
  /// Typical usage:
  /// \code
  ///    minhton::NodeInfo our_new_adj_left = this->routingInfo_.calcOurNewAdjacentLeft();
  /// \endcode
  ///
  /// \param entering_node the node that we accept as a child
  ///
  /// \returns NodeInfo of our new adjacent left, or our old adjacent left, if it hasn't changed
  minhton::NodeInfo calcOurNewAdjacentLeft(const minhton::NodeInfo &entering_node,
                                           const minhton::NodeInfo &entering_node_adj_right) const;

  ///
  /// Calculates the new adjacent right for us, which might change when we accept the entering node
  /// as a new child.
  ///
  /// For this we must have already calculated the new adjacent left of the entering node.
  ///
  /// Typical usage:
  /// \code
  ///    minhton::NodeInfo our_new_adj_right = this->routingInfo_.calcOurNewAdjacentRight();
  /// \endcode
  ///
  /// \param entering_node the node that we accept as a child
  ///
  /// \returns NodeInfo of our new adjacent right, or our old adjacent right, if it hasn't changed
  minhton::NodeInfo calcOurNewAdjacentRight(const minhton::NodeInfo &entering_node,
                                            const minhton::NodeInfo &entering_node_adj_left) const;

  ///
  /// Calculates whether we have to send a update left message or not.
  ///
  /// Typical usage:
  /// \code
  ///    bool send_upate_left = routingInfo->mustSendUpdateLeft(entering_node_adj_right);
  /// \endcode
  ///
  /// \param entering_node_adj_right the adjacent right of the entering node, as calculated
  /// previously
  ///
  /// \returns true if we have to send an update left message, false if not
  bool mustSendUpdateLeft(const minhton::NodeInfo &entering_node_adj_right) const;

  ///
  /// Calculates whether we have to send a update right message or not.
  ///
  /// Typical usage:
  /// \code
  ///    bool send_upate_right = routingInfo->mustSendUpdateRight(entering_node_adj_left);
  /// \endcode
  ///
  /// \param entering_node_adj_left the adjacent left of the entering node, as calculated previously
  ///
  /// \returns true if we have to send an update right message, false if not
  bool mustSendUpdateRight(const minhton::NodeInfo &entering_node_adj_left) const;

  ///
  /// Helper method for the join accept procedure.
  ///
  /// With higher fanouts in some cases the parent is not able to calculate the correct
  /// adjacent neighbor of an entering node.
  ///
  /// In this case we must check with the horizontal value if there might be another
  /// node which is closer to the entering node, than the allegedly adjacent node
  /// the parent calculated.
  ///
  /// For this our children have to be set correctly, because we may not calculate a closer
  /// adjacent as one of our children which do not exist yet.
  ///
  /// This method is probably taking A LOT OF TIME in bigger trees and must be optimized eventually!
  /// We iterate through almost the whole tree and calculate all tree mapper values.
  /// It should be possible to reduce the calculation to only a part of the tree.
  ///
  /// Typical usage:
  /// \code
  ///   auto closer_node = this->routing_info.getCloserAdjacent(entering_node,
  ///   entering_node_adj_left); if(!closer_node.isInitialized()) {
  ///       // executing procedure to get the right adjacent
  ///   }
  /// \endcode
  ///
  /// \param entering_node new child of the parent which wants to enter the network
  /// \param alleged_adjacent the closest adjacent node the parent knows for the new child
  ///
  /// \returns NodeInfo object with a closer position if one if found, otherwise uninitialized
  /// object
  ///
  minhton::NodeInfo getCloserAdjacent(const minhton::NodeInfo &entering_node,
                                      const minhton::NodeInfo &alleged_adjacent) const;

  void allUpdatesAcknowledged();

  PhysicalNodeInfo last_join_info_;
};

}  // namespace minhton

#endif
