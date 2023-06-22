// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "minhton/core/routing_information.h"

namespace minhton {

std::vector<minhton::NodeInfo> RoutingInformation::getInitializedChildren() const {
  std::vector<minhton::NodeInfo> initialized_children;

  std::copy_if(this->children_.begin(), this->children_.end(),
               std::back_inserter(initialized_children),
               [](const minhton::NodeInfo &child) { return child.isInitialized(); });

  return initialized_children;
}

bool RoutingInformation::areChildrenFull() const {
  bool one_child_not_initialized =
      std::any_of(this->children_.begin(), this->children_.end(),
                  [](const minhton::NodeInfo &child) { return !child.isInitialized(); });

  return !one_child_not_initialized;
}

bool RoutingInformation::areRoutingTableNeighborsFull() const {
  bool one_routing_table_neighbors_not_initialized =
      std::any_of(this->routing_table_neighbors_.begin(), this->routing_table_neighbors_.end(),
                  [](const minhton::NodeInfo &node) { return !node.isInitialized(); });

  return !one_routing_table_neighbors_not_initialized;
}

bool RoutingInformation::areRoutingTableNeighborChildrenEmpty() const {
  bool one_routing_table_neighbor_child_initialized = std::any_of(
      this->routing_table_neighbor_children_.begin(), this->routing_table_neighbor_children_.end(),
      [](const minhton::NodeInfo &node) { return node.isInitialized(); });

  return !one_routing_table_neighbor_child_initialized;
}

bool RoutingInformation::areRoutingTableNeighborChildrenFull() const {
  bool one_routing_table_neighbor_child_not_initialized = std::any_of(
      this->routing_table_neighbor_children_.begin(), this->routing_table_neighbor_children_.end(),
      [](const minhton::NodeInfo &node) { return !node.isInitialized(); });

  return !one_routing_table_neighbor_child_not_initialized;
}

std::vector<minhton::NodeInfo> RoutingInformation::getRoutingTableNeighborsAndChildren() const {
  auto all_rt_neighbors = this->routing_table_neighbors_;

  all_rt_neighbors.insert(all_rt_neighbors.end(), this->routing_table_neighbor_children_.begin(),
                          this->routing_table_neighbor_children_.end());

  return all_rt_neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getInitializedRoutingTableNeighborsAndChildren()
    const {
  auto neighbors = this->getRoutingTableNeighborsAndChildren();

  neighbors.erase(
      std::remove_if(neighbors.begin(), neighbors.end(),
                     [](const minhton::NodeInfo &node) { return !node.isInitialized(); }),
      neighbors.end());

  return neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getAllLeftRoutingTableNeighborsAndChildren()
    const {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  auto left_neighbors = this->getLeftRoutingTableNeighborsLeftToRight();
  auto left_neighbor_children = this->getLeftRoutingTableNeighborChildrenLeftToRight();

  left_neighbors.insert(left_neighbors.end(), left_neighbor_children.begin(),
                        left_neighbor_children.end());

  return left_neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getAllRightRoutingTableNeighborsAndChildren()
    const {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  auto right_neighbors = this->getRightRoutingTableNeighborsLeftToRight();
  auto right_neighbor_children = this->getRightRoutingTableNeighborChildrenLeftToRight();

  right_neighbors.insert(right_neighbors.end(), right_neighbor_children.begin(),
                         right_neighbor_children.end());

  return right_neighbors;
}

void RoutingInformation::updateRoutingTableNeighbor(minhton::NodeInfo routing_table_neighbor,
                                                    uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  if (!routing_table_neighbor.isInitialized()) {
    throw std::invalid_argument("Neighbor Node to update is not initialized");
  }

  auto it = std::lower_bound(this->routing_table_neighbors_.begin(),
                             this->routing_table_neighbors_.end(), routing_table_neighbor);

  // if the iterator is at the right position
  if (it != this->routing_table_neighbors_.end() &&
      it->getLogicalNodeInfo() == routing_table_neighbor.getLogicalNodeInfo()) {
    // if the network info to update is actually different
    if (it->getPhysicalNodeInfo() != routing_table_neighbor.getPhysicalNodeInfo()) {
      minhton::NodeInfo old_node = *it;
      // Setting the LogicalNodeInfo is necessary because the UUID needs to be updated
      it->setLogicalNodeInfo(routing_table_neighbor.getLogicalNodeInfo());
      it->setPhysicalNodeInfo(routing_table_neighbor.getPhysicalNodeInfo());
      this->notifyNeighborChange(*it, kRoutingTableNeighbor, ref_event_id,
                                 old_node);  // TODO Index
    }
  }
}

void RoutingInformation::updateRoutingTableNeighborChild(
    minhton::NodeInfo routing_table_neighbor_child, uint64_t ref_event_id) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  if (!routing_table_neighbor_child.isInitialized()) {
    throw std::invalid_argument("Child to update is not initialized");
  }

  if (routing_table_neighbor_child.getFanout() != this->getFanout()) {
    throw std::invalid_argument("Child has wrong Fanout set");
  }

  // if the iterator is at the right position
  auto it =
      std::lower_bound(this->routing_table_neighbor_children_.begin(),
                       this->routing_table_neighbor_children_.end(), routing_table_neighbor_child);
  if (it != this->routing_table_neighbor_children_.end() &&
      it->getLogicalNodeInfo() == routing_table_neighbor_child.getLogicalNodeInfo()) {
    // if the network info to update is actually different
    if (it->getPhysicalNodeInfo() != routing_table_neighbor_child.getPhysicalNodeInfo()) {
      // Setting the LogicalNodeInfo is necessary because the UUID needs to be updated
      it->setLogicalNodeInfo(routing_table_neighbor_child.getLogicalNodeInfo());
      it->setPhysicalNodeInfo(routing_table_neighbor_child.getPhysicalNodeInfo());
      this->notifyNeighborChange(routing_table_neighbor_child, kRoutingTableNeighborChild,
                                 ref_event_id);
    }
  }
}

void RoutingInformation::removeNeighbor(const minhton::NodeInfo &position_to_remove,
                                        uint64_t ref_event_id) {
  // we may not remove the parent. it wouldnt make sense! the whole tree would fall apart
  if (this->parent_.getLogicalNodeInfo() == position_to_remove.getLogicalNodeInfo()) {
    throw std::logic_error("Parent cannot be removed from routing information. Only updated.");
  }

  if (this->adjacent_left_.getLogicalNodeInfo() == position_to_remove.getLogicalNodeInfo()) {
    this->resetAdjacentLeft(ref_event_id);
  } else if (this->adjacent_right_.getLogicalNodeInfo() ==
             position_to_remove.getLogicalNodeInfo()) {
    this->resetAdjacentRight(ref_event_id);
  }

  if (position_to_remove.getLevel() == this->self_node_info_.getLevel()) {
    this->resetRoutingTableNeighbor(position_to_remove, ref_event_id);
  } else if (position_to_remove.getLevel() == this->self_node_info_.getLevel() + 1) {
    this->resetChildOrRoutingTableNeighborChild(position_to_remove, ref_event_id);
  }
}

void RoutingInformation::updateNeighbor(const minhton::NodeInfo &position_to_update,
                                        uint64_t ref_event_id) {
  if (!position_to_update.isInitialized()) {
    throw std::invalid_argument("Node to Update has to be initialized");
  }

  if (this->adjacent_left_.getLogicalNodeInfo() == position_to_update.getLogicalNodeInfo()) {
    this->setAdjacentLeft(position_to_update, ref_event_id);
  } else if (this->adjacent_right_.getLogicalNodeInfo() ==
             position_to_update.getLogicalNodeInfo()) {
    this->setAdjacentRight(position_to_update, ref_event_id);
  }

  if (this->parent_.getLogicalNodeInfo() == position_to_update.getLogicalNodeInfo()) {
    this->setParent(position_to_update, ref_event_id);
  }

  if (position_to_update.getLevel() == this->self_node_info_.getLevel()) {
    this->updateRoutingTableNeighbor(position_to_update, ref_event_id);
  } else if (position_to_update.getLevel() == this->self_node_info_.getLevel() + 1) {
    // child or routing table neighbor child
    auto children_positions =
        calcChildren(this->self_node_info_.getLevel(), this->self_node_info_.getNumber(),
                     this->self_node_info_.getFanout());

    bool is_own_child = false;
    for (auto const &child_pos : children_positions) {
      if (std::get<1>(child_pos) == position_to_update.getNumber()) {
        is_own_child = true;
        this->setChild(position_to_update, position_to_update.getNumber() % this->getFanout(),
                       ref_event_id);
        break;
      }
    }

    if (!is_own_child) {
      this->updateRoutingTableNeighborChild(position_to_update, ref_event_id);
    }
  }
}

std::vector<minhton::NodeInfo> RoutingInformation::calcRoutingTableNeighborParents(
    const minhton::NodeInfo &node) {
  std::vector<minhton::NodeInfo> rt_neighbor_parents;

  if (node.getLevel() == 0) {
    return rt_neighbor_parents;
  }

  std::tuple<uint32_t, uint32_t> parent_pos =
      calcParent(node.getLevel(), node.getNumber(), node.getFanout());
  uint32_t parent_level = std::get<0>(parent_pos);
  uint32_t parent_number = std::get<1>(parent_pos);

  std::vector<std::tuple<uint32_t, uint32_t>> rt_parent_positions =
      calcLeftRT(parent_level, parent_number, node.getFanout());
  std::vector<std::tuple<uint32_t, uint32_t>> right_rt_parent_positions =
      calcRightRT(parent_level, parent_number, node.getFanout());
  rt_parent_positions.insert(rt_parent_positions.end(), right_rt_parent_positions.begin(),
                             right_rt_parent_positions.end());

  uint16_t fanout = node.getFanout();
  for (auto const &node_pos : rt_parent_positions) {
    minhton::NodeInfo parent(std::get<0>(node_pos), std::get<1>(node_pos), fanout);
    rt_neighbor_parents.push_back(parent);
  }

  return rt_neighbor_parents;
}

std::vector<minhton::NodeInfo> RoutingInformation::getAllInitializedRoutingTableNeighborChildren()
    const {
  std::vector<minhton::NodeInfo> initialized_routing_table_neighbor_children =
      std::vector<minhton::NodeInfo>();

  std::copy_if(this->routing_table_neighbor_children_.begin(),
               this->routing_table_neighbor_children_.end(),
               std::back_inserter(initialized_routing_table_neighbor_children),
               [](const minhton::NodeInfo &node) { return node.isInitialized(); });

  return initialized_routing_table_neighbor_children;
}

std::vector<minhton::NodeInfo>
RoutingInformation::getAllInitializedLeftRoutingTableNeighborsAndChildren() const {
  std::vector<minhton::NodeInfo> initialized_neighbors;
  auto all_left_routing_table_neighbors_and_children =
      this->getAllLeftRoutingTableNeighborsAndChildren();

  std::copy_if(all_left_routing_table_neighbors_and_children.begin(),
               all_left_routing_table_neighbors_and_children.end(),
               std::back_inserter(initialized_neighbors),
               [](const minhton::NodeInfo &node) { return node.isInitialized(); });

  return initialized_neighbors;
}

std::vector<minhton::NodeInfo>
RoutingInformation::getAllInitializedRightRoutingTableNeighborsAndChildren() const {
  std::vector<minhton::NodeInfo> initialized_neighbors;
  auto all_right_routing_table_neighbors_and_children =
      this->getAllRightRoutingTableNeighborsAndChildren();

  std::copy_if(all_right_routing_table_neighbors_and_children.begin(),
               all_right_routing_table_neighbors_and_children.end(),
               std::back_inserter(initialized_neighbors),
               [](const minhton::NodeInfo &node) { return node.isInitialized(); });

  return initialized_neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getAllInitializedRoutingTableNeighbors() const {
  std::vector<minhton::NodeInfo> initialized_neighbors;

  std::copy_if(this->routing_table_neighbors_.begin(), this->routing_table_neighbors_.end(),
               std::back_inserter(initialized_neighbors),
               [](const minhton::NodeInfo &node) { return node.isInitialized(); });

  return initialized_neighbors;
}

minhton::NodeInfo RoutingInformation::getDirectLeftNeighbor() const {
  std::vector<minhton::NodeInfo> left_neighbors = getLeftRoutingTableNeighborsLeftToRight();
  if (left_neighbors.empty()) {
    return NodeInfo();
  }
  return left_neighbors.back();
}

minhton::NodeInfo RoutingInformation::getDirectRightNeighbor() const {
  std::vector<minhton::NodeInfo> right_neighbors = getRightRoutingTableNeighborsLeftToRight();
  if (right_neighbors.empty()) {
    return NodeInfo();
  }
  return right_neighbors.front();
}

minhton::NodeInfo RoutingInformation::getLeftmostNeighbor() const {
  std::vector<minhton::NodeInfo> left_neighbors = getLeftRoutingTableNeighborsLeftToRight();
  if (left_neighbors.empty()) {
    return NodeInfo();
  }
  return left_neighbors.front();
}

minhton::NodeInfo RoutingInformation::getRightmostNeighbor() const {
  std::vector<minhton::NodeInfo> right_neighbors = getRightRoutingTableNeighborsLeftToRight();
  if (right_neighbors.empty()) {
    return NodeInfo();
  }
  return right_neighbors.back();
}

minhton::NodeInfo RoutingInformation::getLeftmostNeighborChild() const {
  std::vector<minhton::NodeInfo> left_neighbor_children =
      getLeftRoutingTableNeighborChildrenLeftToRight();
  if (left_neighbor_children.empty()) {
    return NodeInfo();
  }
  return left_neighbor_children.front();
}

///
/// If there is a duplicate, we will chose the node where PhysicalNodeInfo is initialized
///
std::vector<minhton::NodeInfo> RoutingInformation::combiningNodeVectorsWithoutDuplicate(
    std::vector<minhton::NodeInfo> v1, const std::vector<minhton::NodeInfo> &v2) {
  auto result = v2;

  for (auto const &node1 : v1) {
    auto same_pos_it =
        std::find_if(result.begin(), result.end(), [&](const minhton::NodeInfo &node) {
          return node1.getLogicalNodeInfo() == node.getLogicalNodeInfo();
        });

    if (same_pos_it != result.end()) {
      minhton::NodeInfo same_pos_node_in_result = *same_pos_it;

      if (same_pos_node_in_result.isValidPeer() &&
          !same_pos_node_in_result.getPhysicalNodeInfo().isInitialized() &&
          node1.getPhysicalNodeInfo().isInitialized()) {
        // updaing node in result with network info
        same_pos_it->setPhysicalNodeInfo(node1.getPhysicalNodeInfo());
      }

    } else {
      result.push_back(node1);
    }
  }

  return result;
}

std::vector<minhton::NodeInfo> RoutingInformation::getAllUniqueKnownExistingNeighbors() {
  auto neighbors = this->getAllUniqueSymmetricalExistingNeighbors();

  auto initialized_routing_table_children = this->getAllInitializedRoutingTableNeighborChildren();
  neighbors.insert(neighbors.end(), initialized_routing_table_children.begin(),
                   initialized_routing_table_children.end());

  return neighbors;
}

// TODO unittest
minhton::NodeInfo RoutingInformation::getNodeInfoByPosition(uint32_t level, uint32_t number) {
  if (!this->self_node_info_.isValidPeer()) {
    throw std::logic_error("RoutingInformation not initialized");
  }

  if (!isPositionValid(level, number, this->getFanout())) {
    throw std::invalid_argument("Invalid Node Position");
  }

  auto neighbors = this->getAllUniqueKnownExistingNeighbors();
  auto same_pos_it =
      std::find_if(neighbors.begin(), neighbors.end(), [&](const minhton::NodeInfo &node) {
        return node.getLevel() == level && node.getNumber() == number;
      });

  if (same_pos_it != neighbors.end()) {
    return *same_pos_it;
  }

  return minhton::NodeInfo();
}

std::vector<minhton::NodeInfo> RoutingInformation::removeRoutingTableNeighborChildrenFromVector(
    const minhton::NodeInfo node, std::vector<minhton::NodeInfo> neighbors) {
  auto routing_sequence = calcRoutingSequence(node.getLevel(), node.getFanout());

  auto is_rt_neighbor_child_of_node = [&, node, routing_sequence](const minhton::NodeInfo &n) {
    return n.getLevel() == node.getLevel() + 1 &&
           std::find(routing_sequence.begin(), routing_sequence.end(),
                     std::abs(int((n.getNumber() / node.getFanout()) - node.getNumber()))) !=
               routing_sequence.end();
  };

  neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(), is_rt_neighbor_child_of_node),
                  neighbors.end());

  return neighbors;
}

std::vector<minhton::NodeInfo> RoutingInformation::getAllUniqueSymmetricalExistingNeighbors() {
  std::vector<minhton::NodeInfo> neighbors = std::vector<minhton::NodeInfo>();

  if (this->self_node_info_.getLevel() != 0) {
    neighbors.push_back(this->getParent());
  }

  auto existing_children = this->getInitializedChildren();
  neighbors.insert(neighbors.end(), existing_children.begin(), existing_children.end());

  auto initialized_routing_table_neighbors = this->getAllInitializedRoutingTableNeighbors();
  neighbors.insert(neighbors.end(), initialized_routing_table_neighbors.begin(),
                   initialized_routing_table_neighbors.end());

  if (this->adjacent_left_.isInitialized()) {
    bool adj_left_contained =
        std::any_of(neighbors.begin(), neighbors.end(), [=](const minhton::NodeInfo &node) {
          return this->getAdjacentLeft().getLogicalNodeInfo() == node.getLogicalNodeInfo();
        });

    if (!adj_left_contained) {
      neighbors.push_back(this->adjacent_left_);
    }
  }

  if (this->adjacent_right_.isInitialized()) {
    bool adj_right_contained =
        std::any_of(neighbors.begin(), neighbors.end(), [=](const minhton::NodeInfo &node) {
          return this->getAdjacentRight().getLogicalNodeInfo() == node.getLogicalNodeInfo();
        });

    if (!adj_right_contained) {
      neighbors.push_back(this->adjacent_right_);
    }
  }

  return neighbors;
}

bool RoutingInformation::atLeastOneChildExists() const {
  return std::any_of(this->children_.begin(), this->children_.end(),
                     [](const minhton::NodeInfo &node) { return node.isInitialized(); });
}

bool RoutingInformation::atLeastOneChildIsFree() const {
  return std::any_of(this->children_.begin(), this->children_.end(),
                     [](const minhton::NodeInfo &node) { return !node.isInitialized(); });
}

bool RoutingInformation::atLeastOneRoutingTableNeighborChildExists() const {
  return std::any_of(this->routing_table_neighbor_children_.begin(),
                     this->routing_table_neighbor_children_.end(),
                     [](const minhton::NodeInfo &node) { return node.isInitialized(); });
}

bool RoutingInformation::atLeastOneRoutingTableNeighborChildIsFree() const {
  return std::any_of(this->routing_table_neighbor_children_.begin(),
                     this->routing_table_neighbor_children_.end(),
                     [](const minhton::NodeInfo &node) { return !node.isInitialized(); });
}

minhton::NodeInfo RoutingInformation::getLowestNode() const {
  minhton::NodeInfo lowest = minhton::NodeInfo();

  if (this->adjacent_left_.isInitialized()) {
    if (!lowest.isInitialized() ||
        this->adjacent_left_.getLogicalNodeInfo().isDeeperThanOrSameLevel(
            lowest.getLogicalNodeInfo())) {
      lowest = this->adjacent_left_;
    }
  }

  if (this->adjacent_right_.isInitialized()) {
    if (!lowest.isInitialized() ||
        this->adjacent_right_.getLogicalNodeInfo().isDeeperThanOrSameLevel(
            lowest.getLogicalNodeInfo())) {
      lowest = this->adjacent_right_;
    }
  }

  if (lowest.isInitialized() &&
      lowest.getLogicalNodeInfo().isDeeperThan(this->self_node_info_.getLogicalNodeInfo())) {
    return lowest;
  }

  auto is_initialized = [](const minhton::NodeInfo &node) { return node.isInitialized(); };

  // Case lowest is on same level then we are -> search for children

  // check left routing table neighbor children
  auto left_rt_neighbor_children = this->getLeftRoutingTableNeighborChildrenRightToLeft();
  auto it1 = std::find_if(left_rt_neighbor_children.begin(), left_rt_neighbor_children.end(),
                          is_initialized);
  if (it1 != left_rt_neighbor_children.end()) {
    return *it1;
  }

  // check right routing table neighbor children
  auto right_rt_neighbor_children = this->getRightRoutingTableNeighborChildrenLeftToRight();
  auto it2 = std::find_if(right_rt_neighbor_children.begin(), right_rt_neighbor_children.end(),
                          is_initialized);
  if (it2 != right_rt_neighbor_children.end()) {
    return *it2;
  }

  // check own children
  auto it3 = std::find_if(this->children_.begin(), this->children_.end(), is_initialized);
  if (it3 != this->children_.end()) {
    return *it3;
  }

  // only our level is left where we can search for nodes

  // check left routing table neighbors
  auto left_rt_neighbors = this->getLeftRoutingTableNeighborsRightToLeft();
  auto it4 = std::find_if(left_rt_neighbors.begin(), left_rt_neighbors.end(), is_initialized);
  if (it4 != left_rt_neighbors.end()) {
    return *it4;
  }

  // check right routing table neighbors
  auto right_rt_neighbors = this->getRightRoutingTableNeighborsLeftToRight();
  auto it5 = std::find_if(right_rt_neighbors.begin(), right_rt_neighbors.end(), is_initialized);
  if (it5 != right_rt_neighbors.end()) {
    return *it5;
  }

  // no initialized node that is lower
  return minhton::NodeInfo();
}

bool RoutingInformation::areWeDSN() {
  auto dsn_set = getDSNSet(self_node_info_.getLevel(), self_node_info_.getFanout());
  return std::find(dsn_set.begin(), dsn_set.end(), self_node_info_.getNumber()) != dsn_set.end();
}

bool RoutingInformation::areWeTempDSN() {
  if (areWeDSN()) {
    return false;
  }

  // we are not a proper dsn
  auto dsn_set_level_below = getDSNSet(self_node_info_.getLevel() + 1, self_node_info_.getFanout());
  if (dsn_set_level_below.empty()) {
    return false;
  }

  // a child is dsn and is not initialized
  for (auto const &child : children_) {
    bool child_is_dsn = std::find(dsn_set_level_below.begin(), dsn_set_level_below.end(),
                                  child.getNumber()) != dsn_set_level_below.end();
    if (child_is_dsn && !child.isInitialized()) {
      // is there anybody on the level below?
      auto lowest_node = this->getLowestNode();
      if (lowest_node.getLevel() > self_node_info_.getLevel()) {
        return true;
      }
    }
  }

  return false;
}

NodeInfo RoutingInformation::getCoveringDSNOrTempDSN() {
  uint16_t fanout = self_node_info_.getFanout();

  auto const [covering_level, covering_number] =
      getCoveringDSN(self_node_info_.getLevel(), self_node_info_.getNumber(), fanout);
  auto covering_dsn = getNodeInfoByPosition(covering_level, covering_number);

  if (covering_dsn.isInitialized()) {
    return covering_dsn;
  }

  // if we are on the level below, the covering dsn has to exist anyways
  if (self_node_info_.getLevel() != covering_level) {
    return NodeInfo(covering_level, covering_number, fanout);
  }

  auto dsn_set = getDSNSet(covering_level, fanout);
  bool covering_dsn_is_first = *dsn_set.begin() == covering_number;

  // if covering dsn is first dsn on level
  // parent of dsn
  if (covering_dsn_is_first) {
    auto const [parent_level, parent_number] = calcParent(covering_level, covering_number, fanout);
    return NodeInfo(parent_level, parent_number, fanout);
  }

  // if covering dsn is not first on level
  // dsn before

  auto it = std::lower_bound(dsn_set.begin(), dsn_set.end(), covering_number - 1);
  return NodeInfo(covering_level, *it, fanout);
}

bool RoutingInformation::nextDSNExists() {
  // precondition: we are a dsn
  return getNextDSN().isInitialized();
}

NodeInfo RoutingInformation::getNextDSN() {
  // precondition: we are a dsn

  auto dsn_set = getDSNSet(self_node_info_.getLevel(), self_node_info_.getFanout());
  auto it = std::lower_bound(dsn_set.begin(), dsn_set.end(), self_node_info_.getNumber());

  if (it == dsn_set.end()) {
    throw std::invalid_argument("We are not a DSN. This method should not be called");
  }

  if (*it == dsn_set[dsn_set.size() - 1]) {
    // if we are last dsn
    return NodeInfo();
  }

  auto next_dsn_number = *std::next(it, 1);
  auto next_dsn = getNodeInfoByPosition(self_node_info_.getLevel(), next_dsn_number);

  return next_dsn;
}

}  // namespace minhton
