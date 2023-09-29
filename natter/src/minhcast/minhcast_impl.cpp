// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhcast_impl.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

#include "core/natter_check.h"
#include "logging/logger.h"
#include "minhcast_message.h"
#include "natter/message.h"
#include "utils/tree_helper.h"

namespace natter::minhcast {

DEFINE_CRTP_METHODS(NatterMinhcast)

static constexpr Level maxLevelNumber() { return std::numeric_limits<Level>::max(); }

// TODO Replace with C++20 contains
// Check if key is contained in container
template <typename T>
inline static bool contains(const T &container, const typename T::key_type &key) {
  return container.find(key) != container.end();
}

NatterMinhcast::Impl::Impl(MsgReceiveFct recv_callback,
                           [[maybe_unused]] MsgMissingFct missing_callback,
                           std::vector<logging::LoggerPtr> logger, solanet::UUID node_uuid)
    : uuid_(node_uuid),
      msg_recv_callback_(std::move(recv_callback)),
      network_([this](const MinhcastMessage &msg) -> void { processMessage(msg); }) {
  std::for_each(logger.begin(), logger.end(), [this](const logging::LoggerPtr &logger) {
    logger->setApplicationUUID(uuid_);
    logger_.addLogger(logger);
  });
}

void NatterMinhcast::Impl::processMessage(const MinhcastMessage &msg) {
  NATTER_CHECK(contains(own_node_info_, msg.getTopic()),
               "not subscribed to topic but received topic message.");

  logger_.logReceiveFullMsg(msg.getMessageID(), msg.getLastNodeUUID(), getUUID());

  // Pass message to application
  Message outgoing_message(msg.getTopic(), msg.getInitialNodeUUID(), msg.getMessageID(),
                           msg.getContent(), msg.getRound());
  msg_recv_callback_({outgoing_message});
  logger_.logReceivedMessages(uuid_, msg.getInitialNodeUUID(), msg.getMessageID(), msg.getRound());

  // Nodes that received a message as inner forward never needs to forward them
  if (!msg.isInnerForward()) {
    broadcast(createBroadcastInfo(msg));
  }
}

BroadcastInfo NatterMinhcast::Impl::createBroadcastInfo(const MinhcastMessage &msg) const {
  LevelNumber own_pos = own_node_info_.at(msg.getTopic()).position;

  // Check that fanouts of all nodes match
  NATTER_CHECK(std::get<2>(own_pos) == std::get<2>(msg.getLastNodePos()),
               "Fanout of own and last node do not match");
  NATTER_CHECK(std::get<2>(own_pos) == std::get<2>(msg.getInitialNodePos()),
               "Fanout of own and initial node do not match");

  return {{getUUID(), own_pos},
          {msg.getLastNodeUUID(), msg.getLastNodePos()},
          {msg.getInitialNodeUUID(), msg.getInitialNodePos()},
          msg.getForwardingLimit(),
          msg.getTopic(),
          msg.getMessageID(),
          msg.getContent(),
          msg.getRound() + 1};
}

bool NatterMinhcast::Impl::hasChildren(LevelNumber node, const std::set<NodeInfo> &other_peers) {
  // As MINHTON trees are complete, if a node has children it leftmost child must exist.
  const auto [level, number, fanout] = node;
  const uint32_t child_level = level + 1;
  const uint32_t first_child_number = number * fanout;
  return contains(other_peers, {{child_level, first_child_number, fanout}});
}

void NatterMinhcast::Impl::addRoutingTableToReceiver(LevelNumber own_node,
                                                     const std::set<NodeInfo> &other_peers,
                                                     std::vector<NodeInfo> &receiver) {
  addRRT(own_node, other_peers, receiver);
  addLRT(own_node, other_peers, receiver);
}

void NatterMinhcast::Impl::addLRT(LevelNumber own_node, const std::set<NodeInfo> &other_peers,
                                  std::vector<NodeInfo> &receiver) {
  auto lrt = calculateLRT(own_node);
  for (auto &node : lrt) {
    auto it = other_peers.find({node});
    NATTER_CHECK(it != other_peers.end(),
                 "Do not know node in LRT. Nodes in LRT should always be known (complete tree).");
    receiver.emplace_back(*it);
  }
}

void NatterMinhcast::Impl::addRRT(LevelNumber own_node, const std::set<NodeInfo> &other_peers,
                                  std::vector<NodeInfo> &receiver) {
  auto rrt = calculateRRT(own_node);
  for (const auto &node : rrt) {
    auto it = other_peers.find({node});
    // Only add node if it is known
    if (it != other_peers.end()) {
      receiver.emplace_back(*it);
    } else {
      // There can no more nodes be right of this
      return;
    }
  }
}

void NatterMinhcast::Impl::addLeftForwardNodes(LevelNumber own_node, LevelNumber last_node,
                                               const std::set<NodeInfo> &other_peers,
                                               std::vector<NodeInfo> &receiver) {
  std::set<LevelNumber> our_lrt = calculateLRT(own_node);
  if (our_lrt.empty()) return;  // No nodes to forward to

  // Calculate lower boundary
  std::set<LevelNumber> last_sender_lrt;
  auto lower_boundary = last_sender_lrt.end();
  if (std::get<0>(last_node) == std::get<0>(own_node)) {
    last_sender_lrt = calculateLRT(last_node);
    auto it = last_sender_lrt.find(own_node);
    NATTER_CHECK(it != last_sender_lrt.end(), "Cannot find ourselves in LRT from last node");
    lower_boundary = --it;  // Lower boundary is the next left node
  }

  // First node on level or no other peers left of us that already received the message
  // -> Send to all nodes in LRT
  if (lower_boundary == last_sender_lrt.end()) {
    for (auto &node : our_lrt) {
      auto it = other_peers.find({node});
      NATTER_CHECK(it != other_peers.end(), "Do not known node in our LRT");
      receiver.emplace_back(*it);
    }
  } else {
    assert(lower_boundary != last_sender_lrt.end());
    uint32_t lower_boundary_number = std::get<1>(*lower_boundary);
    for (auto &node : our_lrt) {
      // Forward to all nodes in our LRT which have a greater number than lower_boundary
      if (std::get<1>(node) > lower_boundary_number) {
        auto it = other_peers.find({node});
        NATTER_CHECK(it != other_peers.end(), "Do not know node in our LRT");
        receiver.emplace_back(*it);
      }
    }
  }
}

void NatterMinhcast::Impl::addRightForwardNodes(LevelNumber own_node, LevelNumber last_node,
                                                const std::set<NodeInfo> &other_peers,
                                                std::vector<NodeInfo> &receiver) {
  std::set<LevelNumber> our_rrt = calculateRRT(own_node);
  if (our_rrt.empty()) return;

  // Calculate upper boundary
  std::set<LevelNumber> last_sender_rrt;
  auto upper_boundary = last_sender_rrt.end();
  if (std::get<0>(last_node) == std::get<0>(own_node)) {
    last_sender_rrt = calculateRRT(last_node);
    auto it = last_sender_rrt.find(own_node);
    NATTER_CHECK(it != last_sender_rrt.end(), "Cannot find ourselves in RRT from last node");
    upper_boundary = ++it;  // upper boundary is the next right node
  }

  // First node on level or no other peers right of us that already received the message
  // -> Send to all nodes in RRT
  if (upper_boundary == last_sender_rrt.end()) {
    for (auto &node : our_rrt) {
      auto it = other_peers.find({node});
      // Can only send nodes in RRT which exists and are known to us
      if (it != other_peers.end()) {
        receiver.emplace_back(*it);
      }
    }
  } else {
    assert(upper_boundary != last_sender_rrt.end());
    uint32_t upper_boundary_number = std::get<1>(*upper_boundary);
    for (auto &node : our_rrt) {
      // Forward to all nodes in our RRT which have a smaller number
      // than lower_boundary and are existing
      if (std::get<1>(node) < upper_boundary_number) {
        auto it = other_peers.find({node});
        // Can only send nodes in RRT which exists and are known to us
        if (it != other_peers.end()) {
          receiver.emplace_back(*it);
        }
      }
    }
  }
}

void NatterMinhcast::Impl::addInnerForwardNodes(const BroadcastInfo &bc,
                                                const std::set<NodeInfo> &other_peers,
                                                std::vector<NodeInfo> &receiver) {
  const auto [own_level, own_number, fanout] = bc.getOwnNodePos();
  const auto [last_level, last_number, last_fanout] = bc.getLastNodePos();

  // Previous node should be our parent
  assert(last_level == own_level - 1 &&
         last_number == std::floor(static_cast<float>(own_number) / fanout));

  const uint32_t right_child_number = last_number * fanout + (fanout - 1);

  const bool left_child = (own_number == last_number * fanout);
  const bool right_child = (own_number == right_child_number);

  NATTER_CHECK(left_child != right_child,
               "addInnerForwardNodes() called without being outer child");

  const uint32_t nodes_between = fanout - 2;

  if (nodes_between == 0) {
    // no nodes in between to forward to. Can only happen with m=2
    assert(fanout == 2);
    return;
  }

  if (left_child && !contains(other_peers, {{own_level, right_child_number, fanout}})) {
    // We are leftmost child and rightmost child does not exist
    // Forward to all our siblings (entire RRT)
    addRRT(bc.getOwnNodePos(), other_peers, receiver);
  } else if (left_child) {
    // We are the leftmost child and the rightmost child exists
    // Forward to adjacent half of our siblings (right of us)
    const auto upper_number = static_cast<uint32_t>(std::ceil(nodes_between / 2.0)) + own_number;
    addFromRoutingTable(calculateRRT(bc.getOwnNodePos()), other_peers, receiver, upper_number,
                        std::less_equal<>());
  } else if (right_child) {
    // We are the rightmost child and therefore the leftmost child must exist
    // Forward to adjacent half of our siblings (left of us)
    const auto lower_number = own_number - static_cast<uint32_t>(std::floor(nodes_between / 2.0));
    addFromRoutingTable(calculateLRT(bc.getOwnNodePos()), other_peers, receiver, lower_number,
                        std::greater_equal<>());
  }
}

#ifndef NDEBUG
void NatterMinhcast::Impl::logMinhcastBroadcastInfo(const BroadcastInfo &bc) {
  logger_.logMinhcastBroadcast(bc.msg_id, bc.ownLevel(), bc.ownNumber(), bc.forwarding_limit.up(),
                               bc.forwarding_limit.down());
  NATTER_CHECK(!contains(received_messages_, bc.msg_id), "received message more than once");
  received_messages_.insert(bc.msg_id);
}
#endif

bool NatterMinhcast::Impl::centeredNodeAddChildren(const BroadcastInfo &bc,
                                                   const std::set<NodeInfo> &other_peers,
                                                   std::vector<NodeInfo> &receiver) {
  bool other_forwarding_down = false;

  // Get other node who received the message
  bool left_child = bc.ownNumber() == bc.lastNumber() * bc.ownFanout();
  bool right_child = bc.ownNumber() == bc.lastNumber() * bc.ownFanout() + (bc.ownFanout() - 1);
  uint32_t others_number = left_child ? bc.lastNumber() * bc.ownFanout() + (bc.ownFanout() - 1)
                                      : bc.lastNumber() * bc.ownFanout();
  NATTER_CHECK(left_child != right_child,
               "Received message directly from above but we are not an outer child");

  if (left_child && !contains(other_peers, {{bc.ownLevel(), others_number, bc.ownFanout()}})) {
    // We are the only child received the message
    addLeftAndRightmostChildren(bc.getOwnNodePos(), other_peers, receiver);
  } else {
    // Calculate center nodes
    auto nodes_on_level = static_cast<uint32_t>(std::pow(bc.ownFanout(), bc.ownLevel()));

    // Thanks to https://stackoverflow.com/a/27833306
    float center_number = (nodes_on_level - 1) / 2.0F;
    float own_dist = std::max(static_cast<float>(bc.ownNumber()), center_number) -
                     std::min(static_cast<float>(bc.ownNumber()), center_number);
    float other_dist = std::max(static_cast<float>(others_number), center_number) -
                       std::min(static_cast<float>(others_number), center_number);

    if (other_dist < own_dist) {
      // Our sibling is nearer to center
      LevelNumber sibling{bc.ownLevel(), others_number, bc.ownFanout()};
      if (!hasChildren(sibling, other_peers)) {
        // Our sibling has no children but we might have
        addLeftAndRightmostChildren(bc.getOwnNodePos(), other_peers, receiver);
      } else {
        // Our sibling is forwarding down, nothing to do for us
        other_forwarding_down = true;
      }
    } else if (other_dist == own_dist) {
      // Both nodes have same distance, tie situation
      // In this case the left child is forwarding to children
      if (left_child) {
        addLeftAndRightmostChildren(bc.getOwnNodePos(), other_peers, receiver);
      }
    } else {
      // We are nearest to center: Send to children (if they exist)
      addLeftAndRightmostChildren(bc.getOwnNodePos(), other_peers, receiver);
    }
  }

  return other_forwarding_down;
}

std::vector<LevelNumber> NatterMinhcast::Impl::calculateRRTIntersection(const BroadcastInfo &bc) {
  std::vector<LevelNumber> intersection;
  auto last_lrt = calculateLRT(bc.getLastNodePos());
  auto our_rrt = calculateRRT(bc.getOwnNodePos());
  std::set_intersection(last_lrt.begin(), last_lrt.end(), our_rrt.begin(), our_rrt.end(),
                        std::back_inserter(intersection));
  assert(std::is_sorted(intersection.begin(), intersection.end()));
  return intersection;
}

bool NatterMinhcast::Impl::imperfectTreeAddChildren(const BroadcastInfo &bc,
                                                    const std::set<NodeInfo> &other_peers,
                                                    std::vector<NodeInfo> &receiver) {
  bool other_forwarding_down = false;

  // Special case with imperfect tree: Received message inlevel from right and should forward
  // to children because the last node has no children
  assert(!hasChildren(bc.getLastNodePos(),
                      other_peers));  // Info available through routing table neighbors

  if (hasChildren(bc.getOwnNodePos(), other_peers)) {
    // We have children. Check if we are the rightmost neighbor of last sender. If so send to
    // children.
    std::vector<LevelNumber> intersection = calculateRRTIntersection(bc);

    // If no nodes are in common or the leftmost common node (which due to construction of
    // neighbor links must be the leftmost neighbor of last_node before own_node) has no children
    // (known with RT neighbor links) we are the leftmost node that received the message on this
    // level already and have children
    if (intersection.empty() || !hasChildren(intersection[0], other_peers)) {
      addLeftAndRightmostChildren(bc.getOwnNodePos(), other_peers, receiver);
    } else {
      other_forwarding_down = true;
    }
  } else {
    // We should eventually forward down but do not have any children.
    // Iff we are the leftmost neighbor of last_sender, delegate the down_forward request to our
    // left neighbors
    auto last_lrt = calculateLRT(bc.getLastNodePos());
    assert(!last_lrt.empty());
    auto leftmost = *last_lrt.begin();

    // If we are not the leftmost neighbor of last_sender, therefore our LRT intervals
    // neighbors aren't responsible for down_forward. One of our left neighbors will do it
    // otherwise we must delegate down forwarding to our LRT intervals neighbors
    other_forwarding_down = (leftmost != bc.getOwnNodePos());
  }
  return other_forwarding_down;
}

std::tuple<bool, bool> NatterMinhcast::Impl::sendToChildren(const BroadcastInfo &bc,
                                                            const Level child_down_border) {
  const auto &other_peers = other_peers_[bc.topic];
  std::vector<NodeInfo> receiver;

  bool other_forwarding_down = false;
  bool received_from_right = bc.ownLevel() == bc.lastLevel() && bc.ownNumber() < bc.lastNumber();

  // Send to children
  if (bc.isInitialSender()) {
    addLeftAndRightmostChildren(bc.getOwnNodePos(), other_peers, receiver);
  } else if (bc.receivedFromLowerAdjacent() && bc.allowedDownForward()) {
    // First node on level received form
    addLeftAndRightmostChildren(bc.getOwnNodePos(), other_peers, receiver);
  } else if (bc.receivedDirectlyFromAbove() && bc.allowedDownForward()) {
    // Directly received from parent: The most centered and existing of the two children
    // should forward the message
    other_forwarding_down = centeredNodeAddChildren(bc, other_peers, receiver);
  } else if (!bc.receivedDirectlyFromAbove() && bc.receivedFromAbove() && bc.allowedDownForward()) {
    // Received from adjacent above, therefore we are this first node on this level
    // and should send to our children
    addLeftAndRightmostChildren(bc.getOwnNodePos(), other_peers, receiver);
  } else if (bc.allowedDownForward() && received_from_right) {
    other_forwarding_down = imperfectTreeAddChildren(bc, other_peers, receiver);
  }

  // Send message
  for (const auto &peer : receiver) {
    sendMessage(bc, peer, bc.ownLevel() + 1, child_down_border);
  }

  return {other_forwarding_down, !receiver.empty()};
}

void NatterMinhcast::Impl::sendMessage(const BroadcastInfo &bc, const NodeInfo &peer,
                                       uint32_t up_limit, uint32_t down_limit, bool inner_forward) {
  MinhcastMessage minhcast(bc.topic, bc.msg_id, bc.initial_node, bc.own_node, bc.content,
                           bc.current_round, {up_limit, down_limit}, inner_forward);

  logger_.logSendFullMsg(bc.msg_id, peer.uuid, getUUID());
  network_.send(peer.network_info, minhcast);
}

std::optional<NatterMinhcast::NodeInfo> NatterMinhcast::Impl::getDeeperAdjacent(
    const BroadcastInfo &bc) const {
  if (!bc.allowedDownForward()) return std::nullopt;

  const auto &other_peers = other_peers_.at(bc.topic);
  std::optional<NodeInfo> receiver_lower_adjacent;
  uint32_t last_adjacent_level = bc.ownLevel();
  std::for_each(other_peers.begin(), other_peers.end(),
                [&bc = std::as_const(bc), &last_adjacent_level,
                 &receiver_lower_adjacent](const NodeInfo &info) {
                  const uint32_t node_level = std::get<0>(info.position);

                  bool more_than_one_level_below = (node_level > bc.ownLevel() + 1);
                  if (!more_than_one_level_below) return;

                  bool in_forward_down_limit = (node_level <= bc.forwarding_limit.down());
                  bool lower_than_current_adjacent = (node_level > last_adjacent_level);
                  if (in_forward_down_limit && lower_than_current_adjacent) {
                    receiver_lower_adjacent = info;
                    last_adjacent_level = node_level;
                  }
                });
  return receiver_lower_adjacent;
}

void NatterMinhcast::Impl::sendToParent(const BroadcastInfo &bc, const Level parent_up_border) {
  if (bc.ownLevel() == 0) return;  // Parent does not exist

  if (bc.ownLevel() - 1 >= bc.forwarding_limit.up()) {
    const auto &other_peers = other_peers_[bc.topic];
    const uint32_t parent_level = bc.ownLevel() - 1;
    const uint32_t parent_number = std::floor(bc.ownNumber() / bc.ownFanout());
    const NodeInfo parent_info = {{parent_level, parent_number, bc.ownFanout()}};

    auto it = other_peers.find({parent_info});
    NATTER_CHECK(it != other_peers.end(), "Parent must exist but is not known");
    const NodeInfo &parent = *it;

    sendMessage(bc, parent, parent_up_border, bc.ownLevel() - 1);
  }
}

std::tuple<std::vector<NatterMinhcast::NodeInfo>, std::vector<NatterMinhcast::NodeInfo>>
NatterMinhcast::Impl::getInlevel(const BroadcastInfo &bc) {
  std::vector<NodeInfo> receiver_inlevel;
  std::vector<NodeInfo> receiver_inner;

  const auto &other_peers = other_peers_[bc.topic];

  bool single_first_on_level = bc.isInitialSender() || bc.ownLevel() < bc.lastLevel() ||
                               (bc.receivedFromAbove() && !bc.receivedDirectlyFromAbove());

  if (single_first_on_level) {
    addRoutingTableToReceiver(bc.getOwnNodePos(), other_peers, receiver_inlevel);
  } else if (bc.receivedDirectlyFromAbove()) {
    // Max two nodes (including us) received the message
    // Determine if we are left or right child and forward accordingly
    bool left_child = (bc.ownNumber() == bc.lastNumber() * bc.ownFanout());
    bool right_child = (bc.ownNumber() == bc.lastNumber() * bc.ownFanout() + (bc.ownFanout() - 1));
    NATTER_CHECK(left_child != right_child, "Cannot be left and right child simultaneous");
    if (left_child) {
      addLeftForwardNodes(bc.getOwnNodePos(), bc.getLastNodePos(), other_peers, receiver_inlevel);
    } else {
      addRightForwardNodes(bc.getOwnNodePos(), bc.getLastNodePos(), other_peers, receiver_inlevel);
    }
    addInnerForwardNodes(bc, other_peers, receiver_inner);
  } else {
    if (bc.ownNumber() < bc.lastNumber()) {
      // Left forward direction
      addLeftForwardNodes(bc.getOwnNodePos(), bc.getLastNodePos(), other_peers, receiver_inlevel);
    } else {
      // Right forward direction
      addRightForwardNodes(bc.getOwnNodePos(), bc.getLastNodePos(), other_peers, receiver_inlevel);
    }
  }

  return {receiver_inlevel, receiver_inner};
}

void NatterMinhcast::Impl::sendToInlevel(const BroadcastInfo &bc, bool other_forwarding_down,
                                         bool send_to_children) {
  const auto [receiver_inlevel, receiver_inner] = getInlevel(bc);

  // Calculate down border for
  auto get_down_border = [&bc = std::as_const(bc), other_forwarding_down,
                          send_to_children]() -> uint32_t {
    uint32_t down_border = std::min(bc.forwarding_limit.down(), maxLevelNumber());
    if (other_forwarding_down || send_to_children) {
      // Other node is forwarding down to its children, or we are forwarding to our children.
      // Therefore, our neighbors should not
      down_border = bc.ownLevel();
    }
    return down_border;
  };

  const uint32_t down_border = get_down_border();

  // Send inlevel messages
  for (auto &peer : receiver_inlevel) {
    assert(std::get<0>(peer.position) == bc.ownLevel());

    // Only allow our left neighbors to forward down
    const bool peer_left_of_us = (std::get<1>(peer.position) < bc.ownNumber());
    const uint32_t down_border_specific = peer_left_of_us ? down_border : bc.ownLevel();

    sendMessage(bc, peer, bc.ownLevel(), down_border_specific);
  }

  // Send inner messages
  for (const auto &peer : receiver_inner) {
    sendMessage(bc, peer, bc.ownLevel(), bc.ownLevel(), true);
  }
}

std::tuple<uint32_t, uint32_t> NatterMinhcast::Impl::sendToAdjacents(const BroadcastInfo &bc) {
  auto highest_adjacent = getHigherAdjacent(bc);
  auto deepest_adjacent = getDeeperAdjacent(bc);

  // For debugging
  NATTER_CHECK(!(highest_adjacent.has_value() && deepest_adjacent.has_value()),
               "SENDING TO 2 ADJACENTS!");
  if (deepest_adjacent.has_value()) {
    uint32_t deepest_adjacent_level = std::get<0>(deepest_adjacent->position);
    NATTER_CHECK(deepest_adjacent_level > bc.ownLevel() + 1,
                 "Deepest adjacent not at least one level below own level");
  }
  if (highest_adjacent.has_value()) {
    uint32_t highest_adjacent_level = std::get<0>(highest_adjacent->position);
    NATTER_CHECK(highest_adjacent_level < bc.ownLevel() - 1,
                 "Highest adjacent not at least one level above own level");
  }

  uint32_t child_down_border = bc.forwarding_limit.down();
  uint32_t parent_up_border = bc.forwarding_limit.up();

  // If number of level difference is odd, the node on higher levels (more up in tree) should
  // forward to more than the half of levels

  if (highest_adjacent.has_value()) {
    NATTER_CHECK(bc.ownLevel() != 0, "Cannot send to higher adjacent if we are root");
    const uint32_t peer_level = std::get<0>(highest_adjacent->position);
    const uint32_t parent_level = bc.ownLevel() - 1;
    const uint32_t level_diff_adj_parent = parent_level - peer_level - 1;
    const auto level_reached_from_adj =
        static_cast<uint32_t>(std::ceil(level_diff_adj_parent / 2.0));
    const uint32_t down_border = level_reached_from_adj + peer_level;
    const auto level_reached_from_parent =
        static_cast<uint32_t>(std::floor(level_diff_adj_parent / 2.0));
    parent_up_border = parent_level - level_reached_from_parent;
    NATTER_CHECK(parent_up_border - 1 == down_border, "failure in middle calculation");
    sendMessage(bc, highest_adjacent.value(), 0, down_border);
  }

  if (deepest_adjacent.has_value()) {
    const uint32_t peer_level = std::get<0>(deepest_adjacent->position);
    const uint32_t child_level = bc.ownLevel() + 1;
    const uint32_t level_diff_adj_child = peer_level - child_level - 1;

    const auto level_reached_from_adj =
        static_cast<uint32_t>(std::floor(level_diff_adj_child / 2.0));
    const uint32_t up_border = peer_level - level_reached_from_adj;
    const auto level_reached_from_child =
        static_cast<uint32_t>(std::ceil(level_diff_adj_child / 2.0));
    child_down_border = child_level + level_reached_from_child;
    NATTER_CHECK(child_down_border + 1 == up_border, "failure in middle calculation");
    sendMessage(bc, deepest_adjacent.value(), up_border, maxLevelNumber());
  }

  return {parent_up_border, child_down_border};
}

std::optional<NatterMinhcast::NodeInfo> NatterMinhcast::Impl::getHigherAdjacent(
    const BroadcastInfo &bc) const {
  if (!bc.allowedUpForward()) return std::nullopt;

  const auto &other_peers = other_peers_.at(bc.topic);
  std::optional<NodeInfo> receiver_higher_adjacent;
  uint32_t last_adjacent_level = bc.ownLevel();
  std::for_each(other_peers.begin(), other_peers.end(),
                [&bc = std::as_const(bc), &last_adjacent_level,
                 &receiver_higher_adjacent](const NodeInfo &info) {
                  const uint32_t node_level = std::get<0>(info.position);

                  const bool more_than_one_above = (node_level < bc.ownLevel() - 1);
                  if (!more_than_one_above) return;

                  const bool in_forward_up_limit = node_level >= bc.forwarding_limit.up();
                  const bool higher_than_current_adjacent = node_level < last_adjacent_level;
                  if (in_forward_up_limit && higher_than_current_adjacent) {
                    receiver_higher_adjacent = info;
                    last_adjacent_level = node_level;
                  }
                });
  return receiver_higher_adjacent;
}

void NatterMinhcast::Impl::broadcast(const BroadcastInfo &bc) {
#ifndef NDEBUG
  logMinhcastBroadcastInfo(bc);
#endif

  auto [parent_up_border, child_down_border] = sendToAdjacents(bc);
  sendToParent(bc, parent_up_border);
  const auto [other_sending_down, send_to_children] = sendToChildren(bc, child_down_border);
  sendToInlevel(bc, other_sending_down, send_to_children);
}
void NatterMinhcast::Impl::subscribeTopic(const std::string &topic, const NodeInfo &info) {
  NATTER_CHECK(std::get<2>(info.position) >= 2, "Fanout must be >= 2");
  other_peers_[topic] = {};
  own_node_info_[topic] = info;
  auto [level, number, fanout] = info.position;
  if (!info.network_info.ip.empty()) {
    // Only log when natter is used from outside
    logger_.logNewNetworkPeer(uuid_, info.network_info.ip, info.network_info.port, level, number);
  }
}

bool NatterMinhcast::Impl::isSubscribedToTopic(const std::string &topic) const {
  return contains(own_node_info_, topic);
}

void NatterMinhcast::Impl::unsubscribeTopic(const std::string &topic) {
  other_peers_.erase(topic);
  own_node_info_.erase(topic);
}

solanet::UUID NatterMinhcast::Impl::publish(const std::string &topic,
                                            const std::string &msg_content) {
  if (!contains(own_node_info_, topic)) {
    // Not subscribed to topic. Cannot publish
    throw std::runtime_error("not subscribed to topic");
  }

  Message msg(topic, uuid_, solanet::generateUUID(), msg_content, 1);

  logger_.logNewMessage(msg.topic, msg.content, msg.message_id);

  NodeInfo own_node = own_node_info_[topic];

  BroadcastInfo bc{
      {getUUID(), own_node.position},
      {getUUID(), own_node.position},
      {getUUID(), own_node.position},
      {},
      topic,
      msg.message_id,
      msg_content,
      1,
  };
  broadcast(bc);
  return msg.message_id;
}

bool NatterMinhcast::Impl::addPeer(const std::string &topic, const NodeInfo &info) {
  if (!contains(other_peers_, topic)) return false;         // not subscribed to topic
  if (contains(other_peers_[topic], {info})) return false;  // node already known
  NATTER_CHECK(std::get<2>(info.position) >= 2, "Fanout must be >= 2");
  other_peers_[topic].insert(info);
  return true;
}

bool NatterMinhcast::Impl::removePeer(const std::string &topic, const std::string &ip,
                                      uint16_t port) {
  NATTER_CHECK(contains(other_peers_, topic), "no such topic");

  NetworkInfoIPv4 network{ip, port};
  auto it = std::find_if(other_peers_.at(topic).begin(), other_peers_.at(topic).end(),
                         [&network](const NodeInfo &info) { return info.network_info == network; });
  if (it == other_peers_.at(topic).end()) {
    throw std::runtime_error("no such peer");
  }
  other_peers_[topic].erase(it);

  return true;
}

void NatterMinhcast::Impl::addLeftAndRightmostChildren(LevelNumber node_info,
                                                       const std::set<NodeInfo> &other_peers,
                                                       std::vector<NodeInfo> &receiver) {
  const auto [level, number, fanout] = node_info;
  const uint32_t child_level = level + 1;

  for (auto offset : {0U, fanout - 1}) {
    const uint32_t child_number = number * fanout + offset;
    auto peer = other_peers.find({{child_level, child_number, fanout}});
    if (peer == other_peers.end()) return;
    receiver.emplace_back(*peer);
  }
}

template <typename Compare>
void NatterMinhcast::Impl::addFromRoutingTable(const std::set<LevelNumber> &rt,
                                               const std::set<NodeInfo> &other_peers,
                                               std::vector<NodeInfo> &receiver, uint32_t boundary,
                                               Compare comp) {
  for (const auto &node : rt) {
    const auto [level, number, _] = node;
    auto it = other_peers.find({node});
    if (it != other_peers.end() && comp(number, boundary)) {
      receiver.emplace_back(*it);
    }
  }
}

}  // namespace natter::minhcast
