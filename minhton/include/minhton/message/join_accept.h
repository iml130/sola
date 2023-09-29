// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_JOIN_ACCEPT_H_
#define MINHTON_MESSAGE_JOIN_ACCEPT_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** A fitting join position has been found by forwarding the MessageJoin
/// appropriately. The target of the last MessageJoin sends a MessageJoinAccept to the entering node
/// (new child). This message contains all information the entering node needs about its new
/// neighbors and the network's fanout.
/// * **Algorithm Association:** Join.
class MessageJoinAccept : public MinhtonMessage<MessageJoinAccept> {
public:
  /// @param fanout Fanout of the entire network.
  /// @param adjacent_left Adjacent left neighbor of the new child. Might be left empty if the new
  /// child has no adjacent left.
  /// @param adjacent_right Adjacent right neighbor of the new child. Might be left empty if the new
  /// child has no adjacent right.
  /// @param routing_table_neighbors A vector of all existing routing table neighbors on the same
  /// level.
  MessageJoinAccept(MinhtonMessageHeader header, uint16_t fanout, NodeInfo adjacent_left,
                    NodeInfo adjacent_right, std::vector<NodeInfo> routing_table_neighbors);

  uint16_t getFanout() const;
  NodeInfo getAdjacentLeft() const;
  NodeInfo getAdjacentRight() const;
  std::vector<NodeInfo> getRoutingTableNeighbors() const;

  SERIALIZE(header_, fanout_, adjacent_left_, adjacent_right_, routing_table_neighbors_);

  MessageJoinAccept() = default;

private:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

  /// Fanout of the entire network
  uint16_t fanout_ = 0;

  /// Adjacent left neighbor of the new child. Might be left empty if the new child has no adjacent
  /// left.
  NodeInfo adjacent_left_;

  /// Adjacent right neighbor of the new child. Might be left empty if the new child has no
  /// adjacent right.
  NodeInfo adjacent_right_;

  /// A vector of all existing routing table neighbors on the same level.
  std::vector<NodeInfo> routing_table_neighbors_;
};
}  // namespace minhton

#endif
