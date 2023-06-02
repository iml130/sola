// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <stdexcept>

#include "minhton/core/routing_information.h"
namespace minhton {

void RoutingInformation::initRoutingTableNeighbors() {
  uint32_t level = this->self_node_info_.getLevel();
  uint32_t number = this->self_node_info_.getNumber();
  std::vector<std::tuple<uint32_t, uint32_t>> neighbor_list =
      calcLeftRT(level, number, this->getFanout());
  std::vector<std::tuple<uint32_t, uint32_t>> neighbor_list_right =
      calcRightRT(level, number, this->getFanout());

  this->routing_table_neighbors_ = std::vector<minhton::NodeInfo>();
  this->routing_table_neighbor_children_ = std::vector<minhton::NodeInfo>();
  this->right_table_neighbor_start_index_ = neighbor_list.size();

  neighbor_list.insert(neighbor_list.end(), neighbor_list_right.begin(), neighbor_list_right.end());
  for (auto const &position : neighbor_list) {
    uint32_t pos_level = std::get<0>(position);
    uint32_t pos_number = std::get<1>(position);

    auto routing_table_neighbor = minhton::NodeInfo(pos_level, pos_number, this->getFanout());
    this->routing_table_neighbors_.push_back(routing_table_neighbor);

    this->initRoutingTableNeighborChildren(pos_level, pos_number);
  }

  std::sort(this->routing_table_neighbors_.begin(), this->routing_table_neighbors_.end());
  std::sort(this->routing_table_neighbor_children_.begin(),
            this->routing_table_neighbor_children_.end());
}

void RoutingInformation::initRoutingTableNeighborChildren(uint32_t parent_level,
                                                          uint32_t parent_number) {
  std::vector<std::tuple<uint32_t, uint32_t>> children =
      calcChildren(parent_level, parent_number, this->getFanout());

  for (auto const &position : children) {
    uint32_t pos_level = std::get<0>(position);
    uint32_t pos_number = std::get<1>(position);

    auto child = minhton::NodeInfo(pos_level, pos_number, this->getFanout());
    this->routing_table_neighbor_children_.push_back(child);
  }
}

std::vector<minhton::NodeInfo> RoutingInformation::getRoutingTableNeighbors() const {
  return this->routing_table_neighbors_;
}

std::vector<minhton::NodeInfo> RoutingInformation::getRoutingTableNeighborChildren() const {
  return this->routing_table_neighbor_children_;
}

std::vector<minhton::NodeInfo> RoutingInformation::getLeftRoutingTableNeighborsLeftToRight() const {
  std::vector<minhton::NodeInfo>::const_iterator it_begin = this->routing_table_neighbors_.begin();

  std::vector<minhton::NodeInfo>::const_iterator it_last_left =
      this->routing_table_neighbors_.begin();
  std::advance(it_last_left, this->right_table_neighbor_start_index_);

  std::vector<minhton::NodeInfo> left_neighbors(it_begin, it_last_left);
  return left_neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getRightRoutingTableNeighborsLeftToRight()
    const {
  std::vector<minhton::NodeInfo>::const_iterator it_first_right =
      this->routing_table_neighbors_.begin();
  std::advance(it_first_right, this->right_table_neighbor_start_index_);

  std::vector<minhton::NodeInfo>::const_iterator it_last = this->routing_table_neighbors_.end();

  std::vector<minhton::NodeInfo> right_neighbors(it_first_right, it_last);
  return right_neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getLeftRoutingTableNeighborChildrenLeftToRight()
    const {
  std::vector<minhton::NodeInfo>::const_iterator it_begin =
      this->routing_table_neighbor_children_.begin();

  std::vector<minhton::NodeInfo>::const_iterator it_last_left =
      this->routing_table_neighbor_children_.begin();
  std::advance(it_last_left,
               this->right_table_neighbor_start_index_ * this->self_node_info_.getFanout());

  std::vector<minhton::NodeInfo> left_neighbor_children(it_begin, it_last_left);
  return left_neighbor_children;
}

std::vector<minhton::NodeInfo> RoutingInformation::getRightRoutingTableNeighborChildrenLeftToRight()
    const {
  std::vector<minhton::NodeInfo>::const_iterator it_first_right =
      this->routing_table_neighbor_children_.begin();
  std::advance(it_first_right,
               this->right_table_neighbor_start_index_ * this->self_node_info_.getFanout());

  std::vector<minhton::NodeInfo>::const_iterator it_last =
      this->routing_table_neighbor_children_.end();

  std::vector<minhton::NodeInfo> right_neighbor_children(it_first_right, it_last);
  return right_neighbor_children;
}

std::vector<minhton::NodeInfo> RoutingInformation::getLeftRoutingTableNeighborsRightToLeft() const {
  auto neighbors = this->getLeftRoutingTableNeighborsLeftToRight();
  std::reverse(neighbors.begin(), neighbors.end());
  return neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getRightRoutingTableNeighborsRightToLeft()
    const {
  auto neighbors = this->getRightRoutingTableNeighborsLeftToRight();
  std::reverse(neighbors.begin(), neighbors.end());
  return neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getLeftRoutingTableNeighborChildrenRightToLeft()
    const {
  auto neighbors = this->getLeftRoutingTableNeighborChildrenLeftToRight();
  std::reverse(neighbors.begin(), neighbors.end());
  return neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getRightRoutingTableNeighborChildrenRightToLeft()
    const {
  auto neighbors = this->getRightRoutingTableNeighborChildrenLeftToRight();
  std::reverse(neighbors.begin(), neighbors.end());
  return neighbors;
}

// resetting by peer info
bool RoutingInformation::resetRoutingTableNeighborChild(
    const minhton::NodeInfo &routing_table_neighbor_child, uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  if (!routing_table_neighbor_child.isValidPeer()) {
    throw std::invalid_argument("PeerInfo to reset is not initialized");
  }

  auto it =
      std::lower_bound(this->routing_table_neighbor_children_.begin(),
                       this->routing_table_neighbor_children_.end(), routing_table_neighbor_child);

  // if the iterator is at the right position
  if (it != this->routing_table_neighbor_children_.end() &&
      it->getPeerInfo() == routing_table_neighbor_child.getPeerInfo()) {
    // if we would actually change something by resetting
    if (it->getNetworkInfo().isInitialized()) {
      it->setNetworkInfo(minhton::NetworkInfo());
      this->notifyNeighborChange(*it, NeighborRelationship::kRoutingTableNeighborChild,
                                 ref_event_id);
      return true;
    }
  }
  return false;
}

}  // namespace minhton
