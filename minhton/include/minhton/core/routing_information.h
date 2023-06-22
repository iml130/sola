// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_ROUTING_INFORMATION_H_
#define MINHTON_CORE_ROUTING_INFORMATION_H_

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "minhton/core/constants.h"
#include "minhton/core/node_info.h"
#include "minhton/core/routing_calculations.h"
#include "minhton/logging/logger.h"

namespace minhton {

///
/// The RoutingInformation contain all information relevant to the routing in the network.
/// This includes information about ourselves and about our neighbors
/// (parent, children, right and left adjacents, and right and left routing tables).
///
class RoutingInformation {
public:
  ///
  /// Constructor for invalid, non-existent RoutingInformation.
  /// To be used only for initialization purposes.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_ = RoutingInformation();
  /// \endcode
  ///
  RoutingInformation();

  ///
  /// Constructor for RoutingInformation. To be re-initialized
  /// as soon as the own position in the tree is known.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_ = RoutingInformation(&node);
  /// \endcode
  ///
  /// \param self_node_info reference to the own node information
  /// \param logger reference to the logger instance
  ///
  RoutingInformation(const minhton::NodeInfo &self_node_info, const Logger &logger);

  /// Destructor
  ~RoutingInformation() {}

  void resetPosition(uint64_t event_id);

  /// Calling this method if our position has changed.
  /// It resets all neighbors, but we keep our PhysicalNodeInfo!
  ///
  /// \param peer_position information about our new position
  /// and fanout has to be set
  void setPosition(const minhton::LogicalNodeInfo &peer_position);

  /// @brief Setting the status of our NodeInfo object
  /// @param status NodeStatus that the node will be set to
  /// @param event_id EventId which triggered this change (used only for logging)
  void setNodeStatus(NodeStatus status, uint64_t event_id);

  /// \returns our NodeInfo object
  minhton::NodeInfo getSelfNodeInfo() const;

  /// \returns the Fanout of the Network
  /// by using the information of our NodeInfo object
  uint16_t getFanout() const;

  /// \returns a string for simple debugging
  std::string getString() const;

  /// \returns the parent node
  minhton::NodeInfo getParent() const;

  /// Sets the parent node. The LogicalNodeInfo need to have set the correct position.
  /// RoutingInformation needs to be initialized properly beforehand.
  ///
  /// We cannot set a parent if we are root.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.setParent(new_parent);
  /// \endcode
  ///
  /// \param parent new information about the parent node with the correct position, and needs to
  /// be initialized
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void setParent(const minhton::NodeInfo &parent, uint64_t ref_event_id = 0);

  /// \param child_position position of the node, 0 <= child_position < fanout
  /// \returns the child at the given position
  minhton::NodeInfo getChild(uint16_t child_position);

  /// \returns a vector of all children
  std::vector<minhton::NodeInfo> getChildren() const;

  /// Setting information about one new or updated child.
  /// For this we also need the position of the child.
  ///
  /// E.g. with fanout 2 a parent has the positions 0 and 1 for the children.
  /// At 0 is the left-most child and 1 the right-most child of this parent.
  ///
  /// We are also checking here if the logical position of the child is correct.
  ///
  /// RoutingInformation needs to be initialized properly beforehand.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.setChild(new_child, 1);
  /// \endcode
  ///
  /// \param child reference to NodeInfo of the new child, and needs to be initialized
  /// \param position position of the new child within the parent
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void setChild(const minhton::NodeInfo &child, uint16_t position, uint64_t ref_event_id = 0);

  /// \returns the adjacent left neighbor of this node,
  /// uninitialized if we dont have a left adjacent
  minhton::NodeInfo getAdjacentLeft() const;

  /// \returns the adjacent right neighbor of this node,
  /// uninitialized if we dont have a right adjacent
  minhton::NodeInfo getAdjacentRight() const;

  ///
  /// Sets the adjacent left routing table neighbor.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.setAdjacentLeft(new_adjacent_left);
  /// \endcode
  ///
  /// \param adjacent_left new information about the adjacent left node, and needs to be
  /// initialized
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void setAdjacentLeft(const minhton::NodeInfo &adjacent_left, uint64_t ref_event_id = 0);

  ///
  /// Sets the adjacent right routing table neighbor.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.setAdjacentLeft(new_adjacent_right);
  /// \endcode
  ///
  /// \param adjacent_right new information about the adjacent right node, and needs to be
  /// initialized
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void setAdjacentRight(const minhton::NodeInfo &adjacent_right, uint64_t ref_event_id = 0);

  /// \returns vector of all routing table neighbors,
  /// left and right, initialized and uninitialized
  std::vector<minhton::NodeInfo> getRoutingTableNeighbors() const;

  /// \returns vector of all routing table neighbor children,
  /// left and right, initialized and uninitialized
  std::vector<minhton::NodeInfo> getRoutingTableNeighborChildren() const;

  /// \returns vector of all routing table neighbors and routing table neighbor children,
  /// left and right
  std::vector<minhton::NodeInfo> getInitializedRoutingTableNeighborsAndChildren() const;

  /// \returns vector of NodeInfo object of all left routing table neighbors
  /// (including uninitialized ones), ordered from left to right.
  std::vector<minhton::NodeInfo> getLeftRoutingTableNeighborsLeftToRight() const;

  /// \returns vector of NodeInfo object of all left routing table neighbors
  /// (including uninitialized ones), ordered from right to left.
  std::vector<minhton::NodeInfo> getRightRoutingTableNeighborsLeftToRight() const;

  /// \returns vector of NodeInfo object of all left routing table neighbor children
  /// (including uninitialized ones), ordered from left to right.
  std::vector<minhton::NodeInfo> getLeftRoutingTableNeighborChildrenLeftToRight() const;

  /// \returns vector of NodeInfo object of all right routing table neighbor children
  /// (including uninitialized ones), ordered from left to right.
  std::vector<minhton::NodeInfo> getRightRoutingTableNeighborChildrenLeftToRight() const;

  /// \returns vector of NodeInfo object of all left routing table neighbors
  /// (including uninitialized ones), ordered from right to left.
  std::vector<minhton::NodeInfo> getLeftRoutingTableNeighborsRightToLeft() const;

  /// \returns vector of NodeInfo object of all right routing table neighbors
  /// (including uninitialized ones), ordered from right to left.
  std::vector<minhton::NodeInfo> getRightRoutingTableNeighborsRightToLeft() const;

  /// \returns vector of NodeInfo object of all left routing table neighbor children
  /// (including uninitialized ones), ordered from right to left.
  std::vector<minhton::NodeInfo> getLeftRoutingTableNeighborChildrenRightToLeft() const;

  /// \returns vector of NodeInfo object of all right routing table neighbor children
  /// (including uninitialized ones), ordered from right ot left.
  std::vector<minhton::NodeInfo> getRightRoutingTableNeighborChildrenRightToLeft() const;

  /// \returns a vector of all routing table neighbors and routing table neighbor children from the
  /// left routing table
  std::vector<minhton::NodeInfo> getAllLeftRoutingTableNeighborsAndChildren() const;

  /// \returns a vector of all routing table neighbors and routing table neighbor children from the
  /// right routing table
  std::vector<minhton::NodeInfo> getAllRightRoutingTableNeighborsAndChildren() const;

  /// \returns a vector of all routing table neighbor and their children from the left and right
  /// routing table
  std::vector<minhton::NodeInfo> getRoutingTableNeighborsAndChildren() const;

  /// \returns a vector of all initialized routing table neighbors and routing table neighbor
  /// children from the left routing table
  std::vector<minhton::NodeInfo> getAllInitializedLeftRoutingTableNeighborsAndChildren() const;

  /// \returns a vector of all initialized routing table neighbors and routing table neighbor
  /// children from the right routing table
  std::vector<minhton::NodeInfo> getAllInitializedRightRoutingTableNeighborsAndChildren() const;

  /// \returns a vector of all initialized routing table neighbors from the left and right routing
  /// table
  std::vector<minhton::NodeInfo> getAllInitializedRoutingTableNeighbors() const;

  /// \returns a vector of all initialized routing table neighbor children from the left and right
  /// routing table
  std::vector<minhton::NodeInfo> getAllInitializedRoutingTableNeighborChildren() const;

  /// \returns the node which is positioned directly to the left of the current node, e.g., if 3:5
  /// calls this method, it returns 3:4. If there are no left neighbors, the method returns a
  /// default NodeInfo.
  minhton::NodeInfo getDirectLeftNeighbor() const;

  /// \returns the node which is positioned directly to the right of the current node, e.g., if 3:5
  /// calls this method, it returns 3:6. If there are no right neighbors, the method returns a
  /// default NodeInfo.
  minhton::NodeInfo getDirectRightNeighbor() const;

  /// \returns the neighbor node which is the furthest away to the left, i.e., has the number 0 or
  /// is the closest to the number 0. If there are no left neighbors, the method returns a
  /// default NodeInfo.
  minhton::NodeInfo getLeftmostNeighbor() const;

  /// \returns the neighbor node which is the furthest away to the right, i.e., has the number m^l -
  /// 1 or is the closest to the number m^l - 1. If there are no right neighbors, the method returns
  /// a default NodeInfo.
  minhton::NodeInfo getRightmostNeighbor() const;

  /// \returns the neighbor node child which is the furthest away to the left, i.e., has the number
  /// 0 or is the closest to the number 0. If there are no left neighbor children, the method
  /// returns a default NodeInfo.
  minhton::NodeInfo getLeftmostNeighborChild() const;

  /// \returns true if at least one position in children is initialized
  bool atLeastOneChildExists() const;

  /// \returns true if at least one position in children is uninitialized
  bool atLeastOneChildIsFree() const;

  /// \returns true if at least one position in the routing table children is initialized
  bool atLeastOneRoutingTableNeighborChildExists() const;

  /// \returns true if at least one position in the routing table children is uninitialized
  bool atLeastOneRoutingTableNeighborChildIsFree() const;

  /// \returns a vector of all initialized children
  std::vector<minhton::NodeInfo> getInitializedChildren() const;

  /// \returns true if every child is initialized / existent,
  ///           false if at least one child is not initialized
  bool areChildrenFull() const;

  /// \returns true, if every node entry in the left and right routing tables is initialized
  bool areRoutingTableNeighborsFull() const;

  /// \returns true, if no routing table neighbor child is initialized
  bool areRoutingTableNeighborChildrenEmpty() const;

  /// \returns true, if every node entry in the left and right routing tables children is
  /// initialized
  bool areRoutingTableNeighborChildrenFull() const;

  /// Updates a routing table neighbor entry with the given PhysicalNodeInfo information.
  ///
  /// Is being used when we receive a new information about our routing table neighbors.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_.updateRoutingTableNeighbor(updated_routing_table_neighbor);
  /// \endcode
  ///
  /// \param routing_table_neighbor the NodeInfo with new PhysicalNodeInfo, and needs to be
  /// initialized
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void updateRoutingTableNeighbor(minhton::NodeInfo routing_table_neighbor,
                                  uint64_t ref_event_id = 0);

  ///
  /// Updates a routing table neighbor child entry with the given PhysicalNodeInfo information.
  ///
  /// Is being used when we receive a new information about our routing table neighbors children.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_.updateRoutingTableNeighborChild(updated_routing_table_neighbor_child);
  /// \endcode
  ///
  /// \param routing_table_neighbor_child the NodeInfo with new PhysicalNodeInfo, and needs to be
  /// initialized
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void updateRoutingTableNeighborChild(minhton::NodeInfo routing_table_neighbor_child,
                                       uint64_t ref_event_id = 0);

  /// Resetting information about our child at the given position.
  /// We will only reset the PhysicalNodeInfo. LogicalNodeInfo always stays the same.
  ///
  /// RoutingInformation needs to be initialized properly beforehand.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.resetChild(0);
  /// \endcode
  ///
  /// \param position position of the new child within the parent
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void resetChild(uint16_t position, uint64_t ref_event_id = 0);

  /// Resetting information about our adjacent right.
  /// We will reset PhysicalNodeInfo and LogicalNodeInfo.
  ///
  /// RoutingInformation needs to be initialized properly beforehand.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.resetAdjacentRight();
  /// \endcode
  ///
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void resetAdjacentRight(uint64_t ref_event_id = 0);

  /// Resetting information about our adjacent left.
  /// We will reset PhysicalNodeInfo and LogicalNodeInfo.
  ///
  /// RoutingInformation needs to be initialized properly beforehand.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.resetAdjacentLeft();
  /// \endcode
  ///
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void resetAdjacentLeft(uint64_t ref_event_id = 0);

  /// Resetting information about a routing table neighbor.
  /// We do not need to know whether its in the left or right routing table.
  /// Only LogicalNodeInfo is relevant, we ignore PhysicalNodeInfo.
  /// We will only reset the PhysicalNodeInfo. LogicalNodeInfo always stays the same.
  ///
  /// RoutingInformation needs to be initialized properly beforehand.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.resetRoutingTableNeighbor(rt_neighbor);
  /// \endcode
  ///
  /// \param routing_table_neighbor with the LogicalNodeInfo position which we want to reset
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  bool resetRoutingTableNeighbor(const minhton::NodeInfo &routing_table_neighbor,
                                 uint64_t ref_event_id = 0);

  /// Resetting information about a routing table neighbor child or child.
  /// We do not need to know whether its in the left or right routing table or if its a child.
  /// Only LogicalNodeInfo is relevant, we ignore PhysicalNodeInfo.
  /// We will only reset the PhysicalNodeInfo. LogicalNodeInfo always stays the same.
  ///
  /// RoutingInformation needs to be initialized properly beforehand.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info.resetChildOrRoutingTableNeighborChild(node);
  /// \endcode
  ///
  /// \param routing_table_neighbor_child_or_child with the LogicalNodeInfo position which we want
  /// to reset
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  bool resetChildOrRoutingTableNeighborChild(
      const minhton::NodeInfo &routing_table_neighbor_child_or_child, uint64_t ref_event_id = 0);

  /// Searches in all of our routing information for the lowest node we know,
  /// but only in nodes which are on the same level as us or lower than us in the tree (higher level
  /// number).
  ///
  /// If such a node does not exist, an uninitialized node is returned.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_.getLowestNode();
  /// \endcode
  ///
  /// \returns NodeInfo object of the lowest prio node, or uninitialized if we don't know one
  ///
  minhton::NodeInfo getLowestNode() const;

  ///
  /// Returns a vector of every neighbor that we know which exists.
  /// Every neighbor will occur only once.
  /// If a neighbor is not initialized, it will not be added in the vector.
  ///
  /// This will be used in the replacement process to tell the replacing node
  /// about every neighbor of the leaving node.
  ///
  /// Typical usage:
  /// \code
  ///   auto neighbors = this->routing_info_.getAllUniqueKnownExistingNeighbors();
  /// \endcode
  ///
  /// \returns a vector of every existing/initialized neighbor we know
  ///
  std::vector<minhton::NodeInfo> getAllUniqueKnownExistingNeighbors();

  ///
  /// Returns a vector of every neighbor that we know which exists.
  /// Additionally the neighbor needs to be symmetrical. This means that if we know neigbhor A,
  /// neighbor A also needs to know us.
  /// Every neighbor will occur only once.
  /// If a neighbor is not initialized, it will not be added in the vector.
  ///
  /// This will be used in the leaving process when sending node departure messages
  /// to address every neighbor which needs this information.
  ///
  /// Typical usage:
  /// \code
  ///   auto neighbors = this->routing_info_.getAllUniqueKnownExistingNeighbors();
  /// \endcode
  ///
  /// \returns a vector of every existing/initialized symmetrical neighbor we know
  ///
  std::vector<minhton::NodeInfo> getAllUniqueSymmetricalExistingNeighbors();

  ///
  /// Uninitializing the given position to remove.
  /// We need to have the LogicalNodeInfo position set somewhere.
  ///
  /// When we remove an adjacent, we reset the full NodeInfo.
  /// When we remove a non adjacent, we reset only PhysicalNodeInfo.
  /// If a node has multiple roles (e.g. child and adjacent),
  /// both will be reset appropriately.
  ///
  /// We cannot remove the parent, because that would not make any sense.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_.removeNeighbor(node);
  /// \endcode
  ///
  /// \param position_to_remove NodeInfo with the position to remove
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void removeNeighbor(const minhton::NodeInfo &position_to_remove, uint64_t ref_event_id = 0);

  ///
  /// Setting the new PhysicalNodeInfo at the appropriate position.
  /// We need to have the LogicalNodeInfo position set somewhere.
  ///
  /// If a node has multiple roles (e.g. child and adjacent),
  /// both will be updated.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_.updateNeighbor(node);
  /// \endcode
  ///
  /// \param position_to_update NodeInfo with the position to update
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  void updateNeighbor(const minhton::NodeInfo &position_to_update, uint64_t ref_event_id = 0);

  /// Removing a routing table neighbor child by its position.
  /// This entails setting the node info object at the given position
  /// in the routing table neighbor children vector to uninitialized
  ///
  /// \param routing_table_neighbor_child position that we want to remove
  /// \param ref_event_id ID of the event which is the reason for this change
  ///
  bool resetRoutingTableNeighborChild(const minhton::NodeInfo &routing_table_neighbor_child,
                                      uint64_t ref_event_id = 0);

  /// Helper method for various procedures
  /// to find a NodeInfo object in the Routing Information
  /// with the given level and number.
  /// If such a node does not exist an uninitialized NodeInfo
  /// object is being returned.
  ///
  /// \param level level of the required node
  /// \param number number of the required node
  /// \return required NodeInfo object or uninitialized
  minhton::NodeInfo getNodeInfoByPosition(uint32_t level, uint32_t number);

  /// Helper method to calculate all routing table neighbor parents
  /// (those neighbors who know one node as a routing table neighbor child,
  /// but the node does not always know those neighbors (asymmetrical)).
  ///
  /// Creates a vector of all of those neighbors, but all PhysicalNodeInfos are uninitialized.
  ///
  /// Typical usage:
  /// \code
  ///   auto neighbors = this->routing_info_.calcRoutingTableNeighborParents();
  /// \endcode
  ///
  /// \param node of which to get the parents
  ///
  /// \returns vector of uninitialized NodeInfos
  ///
  static std::vector<minhton::NodeInfo> calcRoutingTableNeighborParents(
      const minhton::NodeInfo &node);

  ///
  /// Helper method to easily add vectors of NodeInfo objects together.
  /// If there are dublicates, only one object will be added.
  /// And if one of those duplicates has PhysicalNodeInfo initialized,
  /// the one with initialized PhysicalNodeInfo will be chosen.
  ///
  /// Typical usage:
  /// \code
  ///   auto added = this->routing_info_.combiningNodeVectorsWithoutDuplicate(v1, v2);
  /// \endcode
  ///
  /// \param v1 first vector of NodeInfo objects
  /// \param v2 second vector of NodeInfo objects
  ///
  /// \returns vector of unique combined NodeInfo objects
  ///
  static std::vector<minhton::NodeInfo> combiningNodeVectorsWithoutDuplicate(
      std::vector<minhton::NodeInfo> v1, const std::vector<minhton::NodeInfo> &v2);

  /// Helper method to remove routing table neighbor children from a given nodes perspective
  /// from a given vector of nodes.
  ///
  /// \param node the node from whichs perspective we want to remove the routing table neighbor
  /// children
  /// \param neighbors vector of known neighbors from which the routing table neighbor
  /// children need to be removed
  ///
  /// \returns vector of nodes without the routing table neighbor children
  ///
  /// TODO unittests
  static std::vector<minhton::NodeInfo> removeRoutingTableNeighborChildrenFromVector(
      minhton::NodeInfo node, std::vector<minhton::NodeInfo> neighbors);

  void addNeighborChangeSubscription(
      const std::function<void(const minhton::NodeInfo &new_node, NeighborRelationship relationship,
                               const minhton::NodeInfo &old_node, uint16_t position)> &callback);

  void addNodeInfoChangeSubscription(
      const std::function<void(const minhton::NodeInfo &previous_node_info,
                               const minhton::NodeInfo &new_node_info)> &callback);

  bool areWeDSN();
  bool areWeTempDSN();

  NodeInfo getCoveringDSNOrTempDSN();

  bool nextDSNExists();

  NodeInfo getNextDSN();

private:
  /// self_node_info of the minhton_node object
  minhton::NodeInfo self_node_info_;

  /// logger instance
  minhton::Logger logger_;

  /// parent of this node
  minhton::NodeInfo parent_;

  /// adjacent left of this node
  minhton::NodeInfo adjacent_left_;

  /// adjacent left of this node
  minhton::NodeInfo adjacent_right_;

  /// vector of the children of this node
  std::vector<minhton::NodeInfo> children_;

  // vector of routing table neighbors on our level with specific horizontal distances
  std::vector<minhton::NodeInfo> routing_table_neighbors_;

  // vector of children of our routing table neighbors
  std::vector<minhton::NodeInfo> routing_table_neighbor_children_;

  // at this index the first routing table neighbor of the right side starts
  // simultanously the first routing table neighbor child starts at
  // (right_table_neighbor_start_index_ * fanout)
  uint32_t right_table_neighbor_start_index_ = 0;

  std::vector<
      std::function<void(const minhton::NodeInfo &new_node, NeighborRelationship relationship,
                         const minhton::NodeInfo &old_node, uint16_t position)>>
      neighbor_change_subscription_callbacks_;

  std::vector<std::function<void(const minhton::NodeInfo &previous_node_info,
                                 const minhton::NodeInfo &new_node_info)>>
      node_info_change_subscription_callbacks_;

  ///
  /// Our RoutingInformation object will call this function,
  /// each time a neighbor connection is changing.
  ///
  /// This way we get notified by a change and can log it.
  ///
  /// \param neighbor those connection has changed in some way
  /// \param relationship we have to this neighbor
  ///
  void notifyNeighborChange(const minhton::NodeInfo &new_node, NeighborRelationship relationship,
                            uint64_t ref_event_id,
                            const minhton::NodeInfo &old_node = minhton::NodeInfo(),
                            uint16_t index = 0);  // TODO Remove default argument

  void notifyNodeInfoChange(const minhton::NodeInfo &previous_node_info,
                            const minhton::NodeInfo &new_node_info);

  ///
  /// Helper method to initialize the routing table vector within the Constructor,
  /// by using the routing calculations.
  ///
  /// Only to be used in the Constructor.
  ///
  /// For this the logical position of the node has to be known.
  ///
  /// Typical usage:
  /// \code
  ///   this->initRoutingTableNeighbors();
  /// \endcode
  ///
  void initRoutingTableNeighbors();

  ///
  /// Helper method to initialize the routing tables neighbor childs within the Constructor,
  /// by using the routing calculations.
  ///
  /// Only to be used in the Constructor.
  ///
  /// For this the logical position of the node has to be known.
  ///
  /// Typical usage:
  /// \code
  ///   this->initRoutingTableNeighbors(parent_level, parent_number);
  /// \endcode
  ///
  void initRoutingTableNeighborChildren(uint32_t parent_level, uint32_t parent_number);

  ///
  /// Helper method to set the right LogicalNodeInfo positions into
  /// the parent and children neighbors.
  ///
  /// We leave the adjacents empty because their position can change.
  /// The position of parent and children may never change.
  ///
  /// Here we are making use of the RoutingCalculations
  ///
  /// Typical usage:
  /// \code
  /// \endcode
  ///   this->initParentAndChildren();
  /// \param
  ///
  void initParentAndChildren();

  void clearRoutingTable();
};
}  // namespace minhton
#endif
