// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_MINHCAST_MINHCAST_IMPL_H_
#define NATTER_MINHCAST_MINHCAST_IMPL_H_

#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "broadcast_info.h"
#include "core/network_facade.h"
#include "logging/logger.h"
#include "natter/logger_interface.h"
#include "natter/minhcast_level_number.h"
#include "natter/natter_minhcast.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"

namespace natter::minhcast {

class MinhcastMessage;

class NatterMinhcast::Impl {
public:
  Impl(MsgReceiveFct recv, MsgMissingFct miss, std::vector<logging::LoggerPtr> logger = {},
       solanet::UUID uuid = solanet::generateUUID());

  // Publish message, returns message_id if everything went successfull
  solanet::UUID publish(const std::string &topic, const std::string &msg_content);

  bool addPeer(const std::string &topic, const NodeInfo &info);

  bool removePeer(const std::string &topic, const std::string &ip, uint16_t port);

  void subscribeTopic(const std::string &topic, const NodeInfo &info);

  bool isSubscribedToTopic(const std::string &topic) const;

  void unsubscribeTopic(const std::string &topic);

  NodeInfo getOwnNodeInfo(const std::string &topic) const { return own_node_info_.at(topic); }

  solanet::UUID getUUID() const { return uuid_; }

  NetworkInfoIPv4 getNetworkInfo() const { return network_.getNetworkInfo(); }

private:
  BroadcastInfo createBroadcastInfo(const MinhcastMessage &msg) const;

#ifndef NDEBUG
  // For debugging: Log forwarding limits and check that no message arrived multiple times
  void logMinhcastBroadcastInfo(const BroadcastInfo &bc);
#endif

  /**
   * Send to children if required
   * @return (true if other node needs to forward down instead of us), (true if we sent to our
   * children)
   */
  std::tuple<bool, bool> sendToChildren(const BroadcastInfo &bc, Level child_down_border);

  // Helper methods to send message to other nodes
  void sendToInlevel(const BroadcastInfo &bc, bool other_forwarding_down, bool send_to_children);
  void sendToParent(const BroadcastInfo &bc, Level parent_up_border);
  std::tuple<uint32_t, uint32_t> sendToAdjacents(const BroadcastInfo &bc);

  // Return nodes for inlevel forward for given broadcast info
  std::tuple<std::vector<NodeInfo>, std::vector<NodeInfo>> getInlevel(const BroadcastInfo &bc);

  /**
   * Add left/rightmost children to receiver if this node is the most centered one of
   * the previous two children received a message on the current level.
   * @return true if the other child node is forwarding down
   */
  static bool centeredNodeAddChildren(const BroadcastInfo &bc,
                                      const std::set<NodeInfo> &other_peers,
                                      std::vector<NodeInfo> &receiver);

  /**
   * Add left/rightmost children to receiver if this node is the most centered one of
   * the previous two children received a message on the current level.
   * @return true if the other child node is forwarding down
   */
  static bool imperfectTreeAddChildren(const BroadcastInfo &bc,
                                       const std::set<NodeInfo> &other_peers,
                                       std::vector<NodeInfo> &receiver);

  // Calculate intersection between lastNode LRT and ownNode RRT
  static std::vector<LevelNumber> calculateRRTIntersection(const BroadcastInfo &bc);

  // Get leftmost adjacent on deepest level and within BroadcastInfos forwarding limits
  std::optional<NatterMinhcast::NodeInfo> getDeeperAdjacent(const BroadcastInfo &bc) const;

  // Get rightmost adjacent on highest level and within BroadcastInfos forwarding limits
  std::optional<NatterMinhcast::NodeInfo> getHigherAdjacent(const BroadcastInfo &bc) const;

  // Helper method to send message to peer
  void sendMessage(const BroadcastInfo &bc, const NodeInfo &peer, uint32_t up_limit,
                   uint32_t down_limit, bool inner_forward = false);

  // Processing method for newly arrived messages
  void processMessage(const MinhcastMessage &msg);

  void broadcast(const BroadcastInfo &bc);

  // Check if node has children
  static bool hasChildren(LevelNumber node, const std::set<NodeInfo> &other_peers);

  // Add all nodes from LRT and RRT of own_node to receiver
  static void addRoutingTableToReceiver(LevelNumber own_node, const std::set<NodeInfo> &other_peers,
                                        std::vector<NodeInfo> &receiver);
  static void addLRT(LevelNumber own_node, const std::set<NodeInfo> &other_peers,
                     std::vector<NodeInfo> &receiver);
  static void addRRT(LevelNumber own_node, const std::set<NodeInfo> &other_peers,
                     std::vector<NodeInfo> &receiver);

  // Add all nodes in given direction to receiver which are in the remaining interval of nodes
  // next to us which haven't received the message yet and are known by us
  static void addLeftForwardNodes(LevelNumber own_node, LevelNumber last_node,
                                  const std::set<NodeInfo> &other_peers,
                                  std::vector<NodeInfo> &receiver);
  static void addRightForwardNodes(LevelNumber own_node, LevelNumber last_node,
                                   const std::set<NodeInfo> &other_peers,
                                   std::vector<NodeInfo> &receiver);

  // Add inner nodes (of two outer children with m > 2) to receiver
  static void addInnerForwardNodes(const BroadcastInfo &bc, const std::set<NodeInfo> &other_peers,
                                   std::vector<NodeInfo> &receiver);

  static void addLeftAndRightmostChildren(LevelNumber node_info,
                                          const std::set<NodeInfo> &other_peers,
                                          std::vector<NodeInfo> &receiver);

  // Helper function to add nodes in given interval by passing a comparison function object
  template <typename Compare>
  static void addFromRoutingTable(const std::set<LevelNumber> &rt,
                                  const std::set<NodeInfo> &other_peers,
                                  std::vector<NodeInfo> &receiver, uint32_t boundary, Compare comp);

  using Topic = std::string;
  std::unordered_map<Topic, NodeInfo> own_node_info_;
  std::unordered_map<Topic, std::set<NodeInfo>> other_peers_;
  solanet::UUID uuid_;
  natter::logging::Logger logger_;
  MsgReceiveFct msg_recv_callback_;
  core::NetworkFacade<MinhcastMessage> network_;

#ifndef NDEBUG
  // To keep track of possible received duplicate messages
  std::set<solanet::UUID> received_messages_;
#endif
};
}  // namespace natter::minhcast

#endif  // DAISI_NATTER_IMPL_H_
