// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/routing_information.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "minhton/logging/logging.h"

namespace minhton {

RoutingInformation::RoutingInformation(minhton::NodeInfo self_node_info, Logger logger)
    : self_node_info_(std::move(self_node_info)), logger_(std::move(logger)) {
  if (self_node_info_.isValidPeer()) {
    initParentAndChildren();
    initRoutingTableNeighbors();
    adjacent_left_ = NodeInfo();
    adjacent_right_ = NodeInfo();
  }
}

void RoutingInformation::resetPosition(uint64_t event_id) {
  auto temp_previous = self_node_info_;
  self_node_info_.setLogicalNodeInfo(LogicalNodeInfo());
  clearRoutingTable();

  this->logger_.logNodeLeft({temp_previous.getLogicalNodeInfo().getUuid(), event_id});
  notifyNodeInfoChange(temp_previous, self_node_info_);
}

void RoutingInformation::setPosition(const minhton::LogicalNodeInfo &peer_position) {
  auto temp_previous = self_node_info_;
  self_node_info_.setPosition(peer_position);

  if (self_node_info_.isInitialized()) {
    initParentAndChildren();
    initRoutingTableNeighbors();
    adjacent_left_ = NodeInfo();
    adjacent_right_ = NodeInfo();

  } else {
    clearRoutingTable();
  }

  notifyNodeInfoChange(temp_previous, self_node_info_);
}

void RoutingInformation::clearRoutingTable() {
  parent_ = NodeInfo();
  children_ = std::vector<NodeInfo>();
  adjacent_left_ = NodeInfo();
  adjacent_right_ = NodeInfo();
  routing_table_neighbors_ = std::vector<NodeInfo>();
  routing_table_neighbor_children_ = std::vector<NodeInfo>();
}

void RoutingInformation::initParentAndChildren() {
  // parent
  this->parent_ =
      minhton::NodeInfo();  // overwriting parent if there might have been something set before
  if (this->self_node_info_.getLevel() > 0) {
    auto [parent_level, parent_number] = calcParent(
        this->self_node_info_.getLevel(), this->self_node_info_.getNumber(), this->getFanout());
    this->parent_ = minhton::NodeInfo(parent_level, parent_number, this->getFanout());
  }

  // children
  std::vector<std::tuple<uint32_t, uint32_t>> children_pos =
      calcChildren(this->self_node_info_.getLevel(), this->self_node_info_.getNumber(),
                   this->self_node_info_.getFanout());

  if (this->self_node_info_.getFanout() != children_pos.size()) {
    throw std::logic_error("Calculated children vector for Node is not the same as Fanout; Error "
                           "in RoutingCalculations");
  }

  this->children_ = std::vector<minhton::NodeInfo>();
  for (uint32_t i = 0; i < this->self_node_info_.getFanout(); i++) {
    this->children_.emplace_back(std::get<0>(children_pos[i]), std::get<1>(children_pos[i]),
                                 this->self_node_info_.getFanout());
  }
}

uint16_t RoutingInformation::getFanout() const {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("We don't know our NodeInfo. Therefore we can't know the fanout.");
  }

  return this->self_node_info_.getFanout();
}

std::string RoutingInformation::getString() const {
  std::string text =
      "\t\tParent: " + (this->parent_.isInitialized() ? this->parent_.getString() : " --- ") + "\n";

  text = text + "\t\tchildren: \n";
  for (size_t i = 0; i < this->children_.size(); i++) {
    text = text + "\t\t\tChild " + std::to_string(i) + ": " +
           (this->children_[i].isInitialized() ? this->children_[i].getString() : " --- ") + "\n";
  }

  text = text + "\t\tAdjacent Left: " +
         (this->adjacent_left_.isInitialized() ? this->adjacent_left_.getString() : " --- ") + "\n";
  text = text + "\t\tAdjacent Right: " +
         (this->adjacent_right_.isInitialized() ? this->adjacent_right_.getString() : " --- ") +
         "\n";

  return text;
}

void RoutingInformation::notifyNeighborChange(const minhton::NodeInfo &new_node,
                                              NeighborRelationship relationship,
                                              uint64_t ref_event_id,
                                              const minhton::NodeInfo &old_node, uint16_t index) {
  logger_.logNeighbor(minhton::LoggerInfoAddNeighbor{
      ref_event_id, this->self_node_info_.getLogicalNodeInfo().getUuid(),
      new_node.getLogicalNodeInfo().getUuid(), relationship});

  for (const auto &subscription_callback : this->neighbor_change_subscription_callbacks_) {
    subscription_callback(new_node, relationship, old_node, index);
  }
}

void RoutingInformation::notifyNodeInfoChange(const minhton::NodeInfo &previous_node_info,
                                              const minhton::NodeInfo &new_node_info) {
  if (previous_node_info == new_node_info) {
    return;
  }

  for (const auto &subscription_callback : this->node_info_change_subscription_callbacks_) {
    subscription_callback(previous_node_info, new_node_info);
  }
}

minhton::NodeInfo RoutingInformation::getSelfNodeInfo() const { return this->self_node_info_; }

void RoutingInformation::setNodeStatus(NodeStatus status, uint64_t event_id) {
  // Log changed status
  switch (status) {
    case NodeStatus::kUninit:
      this->logger_.logNodeUninit({this->self_node_info_.getLogicalNodeInfo().getUuid(), event_id});
      break;
    case NodeStatus::kRunning:
      this->logger_.logNodeRunning(
          {this->self_node_info_.getLogicalNodeInfo().getUuid(), event_id});
      break;
    case NodeStatus::kLeft:
      this->logger_.logNodeLeft({this->self_node_info_.getLogicalNodeInfo().getUuid(), event_id});
      break;
    default:
      break;
  }

  // Set status
  this->self_node_info_.setStatus(status);
}

void RoutingInformation::setParent(const minhton::NodeInfo &parent, uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("Cannot set Parent for uninitialized RoutingInformation.");
  }

  if (this->self_node_info_.getLevel() == 0) {
    throw std::logic_error("Root cannot have a parent");
  }

  if (this->parent_.getLogicalNodeInfo() != parent.getLogicalNodeInfo()) {
    throw std::invalid_argument(
        "The NodeInfo needs to have the correct LogicalNodeInfo position in the tree.");
  }

  if (this->getFanout() != parent.getFanout()) {
    throw std::invalid_argument(
        "The Fanout needs to be set properly in the LogicalNodeInfo object");
  }

  if (!parent.isInitialized()) {
    throw std::invalid_argument("Parent must be initialized / must exist");
  }

  if (parent.getPhysicalNodeInfo() != this->parent_.getPhysicalNodeInfo()) {
    minhton::NodeInfo node = parent;
    std::swap(this->parent_, node);
    this->notifyNeighborChange(this->parent_, NeighborRelationship::kParent, ref_event_id, node);
  }
}

minhton::NodeInfo RoutingInformation::getParent() const { return this->parent_; }

std::vector<minhton::NodeInfo> RoutingInformation::getChildren() const { return this->children_; }

void RoutingInformation::setChild(const minhton::NodeInfo &child, uint16_t position,
                                  uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("Cannot set Child for uninitialized RoutingInformation.");
  }

  if (position >= this->self_node_info_.getFanout()) {
    throw std::out_of_range("Position not in range");
  }

  if (this->children_[position].getLogicalNodeInfo() != child.getLogicalNodeInfo()) {
    throw std::invalid_argument(
        "The NodeInfo needs to have the correct LogicalNodeInfo position in the tree.");
  }

  if (this->getFanout() != child.getFanout()) {
    throw std::invalid_argument(
        "The Fanout needs to be set properly in the LogicalNodeInfo object");
  }

  if (!child.isInitialized()) {
    throw std::invalid_argument("Child must be initialized / must exist");
  }

  if (child.getPhysicalNodeInfo() != this->children_[position].getPhysicalNodeInfo()) {
    minhton::NodeInfo node = child;
    std::swap(this->children_[position], node);
    this->notifyNeighborChange(this->children_[position], NeighborRelationship::kChild,
                               ref_event_id, node, position);
  }
}

minhton::NodeInfo RoutingInformation::getChild(uint16_t child_position) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("Cannot get Child for uninitialized RoutingInformation.");
  }

  if (child_position >= this->self_node_info_.getFanout()) {
    throw std::out_of_range("Position not in range");
  }

  return this->children_[child_position];
}

minhton::NodeInfo RoutingInformation::getAdjacentLeft() const { return this->adjacent_left_; }

minhton::NodeInfo RoutingInformation::getAdjacentRight() const { return this->adjacent_right_; }

void RoutingInformation::setAdjacentLeft(const minhton::NodeInfo &adjacent_left,
                                         uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("Cannot set Adjacent for uninitialized RoutingInformation.");
  }

  if (adjacent_left.getFanout() != this->getFanout()) {
    throw std::invalid_argument(
        "The Fanout needs to be set properly in the LogicalNodeInfo object");
  }

  if (!adjacent_left.isInitialized()) {
    throw std::invalid_argument("Adjacent must be initialized / must exist");
  }

  // node is not to the left of us
  if (adjacent_left.getLogicalNodeInfo().getHorizontalValue() >=
      this->self_node_info_.getLogicalNodeInfo().getHorizontalValue()) {
    throw std::invalid_argument("Adjacent is not to the left of us");
  }

  // if there is a difference in PhysicalNodeInfo or LogicalNodeInfo
  if (this->adjacent_left_ != adjacent_left) {
    minhton::NodeInfo node = adjacent_left;
    std::swap(this->adjacent_left_, node);
    this->notifyNeighborChange(this->adjacent_left_, NeighborRelationship::kAdjacentLeft,
                               ref_event_id, node);
  }
}

void RoutingInformation::setAdjacentRight(const minhton::NodeInfo &adjacent_right,
                                          uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("Cannot set Adjacent for uninitialized RoutingInformation.");
  }

  if (adjacent_right.getFanout() != this->getFanout()) {
    throw std::invalid_argument(
        "The Fanout needs to be set properly in the LogicalNodeInfo object");
  }

  if (!adjacent_right.isInitialized()) {
    throw std::invalid_argument("Adjacent must be initialized / must exist");
  }

  // node is not to the right of us
  if (adjacent_right.getLogicalNodeInfo().getHorizontalValue() <=
      this->self_node_info_.getLogicalNodeInfo().getHorizontalValue()) {
    throw std::invalid_argument("Adjacent is not to the right of us");
  }

  // if there is a difference in PhysicalNodeInfo or LogicalNodeInfo
  if (this->adjacent_right_ != adjacent_right) {
    minhton::NodeInfo node = adjacent_right;
    std::swap(this->adjacent_right_, node);
    this->notifyNeighborChange(this->adjacent_right_, NeighborRelationship::kAdjacentRight,
                               ref_event_id, node);
  }
}

void RoutingInformation::resetAdjacentRight(uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  if (this->adjacent_right_.isInitialized()) {
    minhton::NodeInfo node;
    std::swap(this->adjacent_right_, node);
    this->notifyNeighborChange(this->adjacent_right_, NeighborRelationship::kAdjacentRight,
                               ref_event_id, node);
  }
}

void RoutingInformation::resetAdjacentLeft(uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  if (this->adjacent_left_.isInitialized()) {
    minhton::NodeInfo node;
    std::swap(this->adjacent_left_, node);
    this->notifyNeighborChange(this->adjacent_left_, NeighborRelationship::kAdjacentLeft,
                               ref_event_id, node);
  }
}

bool RoutingInformation::resetRoutingTableNeighbor(const minhton::NodeInfo &routing_table_neighbor,
                                                   uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  if (!routing_table_neighbor.isValidPeer()) {
    throw std::invalid_argument("LogicalNodeInfo to reset is not initialized");
  }

  auto it = std::lower_bound(this->routing_table_neighbors_.begin(),
                             this->routing_table_neighbors_.end(), routing_table_neighbor);

  // if the iterator is at the right position
  if (it != this->routing_table_neighbors_.end() &&
      it->getLogicalNodeInfo() == routing_table_neighbor.getLogicalNodeInfo()) {
    // if we would actually change something by resetting
    if (it->getPhysicalNodeInfo().isInitialized()) {
      minhton::NodeInfo node = *it;
      it->setPhysicalNodeInfo(minhton::PhysicalNodeInfo());
      this->notifyNeighborChange(*it, NeighborRelationship::kRoutingTableNeighbor, ref_event_id,
                                 node);
      return true;
    }
  }

  return false;
}

bool RoutingInformation::resetChildOrRoutingTableNeighborChild(
    const minhton::NodeInfo &routing_table_neighbor_child_or_child, uint64_t ref_event_id) {
  if (!routing_table_neighbor_child_or_child.isValidPeer()) {
    throw std::invalid_argument("Childs LogicalNodeInfo to reset is not initialized");
  }

  if (routing_table_neighbor_child_or_child.getLevel() != this->self_node_info_.getLevel() + 1) {
    throw std::invalid_argument("Childs to reset is not on one level lower");
  }

  // if its our child, depening on the number
  if (routing_table_neighbor_child_or_child.getNumber() / this->self_node_info_.getFanout() ==
      this->self_node_info_.getNumber()) {
    auto it = std::find_if(this->children_.begin(), this->children_.end(),
                           [&](const minhton::NodeInfo &child) {
                             return child.isInitialized() &&
                                    child.getLogicalNodeInfo() ==
                                        routing_table_neighbor_child_or_child.getLogicalNodeInfo();
                           });
    if (it != this->children_.end()) {
      this->resetChild(it->getNumber() % this->getFanout(), ref_event_id);
      return true;
    }

  } else {
    // its not our child, so it could only be our routing table neighbor child
    return this->resetRoutingTableNeighborChild(routing_table_neighbor_child_or_child,
                                                ref_event_id);
  }

  return false;
}

void RoutingInformation::resetChild(uint16_t position, uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  if (position >= this->getFanout()) {
    throw std::invalid_argument("Invalid Position to reset Child");
  }
  if (this->children_[position].getPhysicalNodeInfo().isInitialized()) {
    minhton::NodeInfo node = this->children_[position];
    this->children_[position].setPhysicalNodeInfo(minhton::PhysicalNodeInfo());
    this->notifyNeighborChange(this->children_[position], NeighborRelationship::kChild,
                               ref_event_id, node, position);
  }
}

void RoutingInformation::addNeighborChangeSubscription(
    const std::function<void(const minhton::NodeInfo &new_node, NeighborRelationship relationship,
                             const minhton::NodeInfo &old_node, uint16_t position)> &callback) {
  this->neighbor_change_subscription_callbacks_.push_back(callback);
}

void RoutingInformation::addNodeInfoChangeSubscription(
    const std::function<void(const minhton::NodeInfo &previous_node_info,
                             const minhton::NodeInfo &new_node_info)> &callback) {
  this->node_info_change_subscription_callbacks_.push_back(callback);
}

}  // namespace minhton
