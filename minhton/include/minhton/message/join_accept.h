// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_JOIN_ACCEPT_H_
#define MINHTON_MESSAGE_JOIN_ACCEPT_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// A fitting join position has been found by forwarding the join message appropriately. The target
/// of the last join message sends a Join Accept message to the entering node (new child). This
/// message contains all information the entering node needs about its new neighbors and the
/// network's fanout.
class MessageJoinAccept : public MinhtonMessage<MessageJoinAccept> {
public:
  MessageJoinAccept(const MinhtonMessageHeader &header, uint16_t fanout, NodeInfo adjacent_left,
                    NodeInfo adjacent_right, std::vector<NodeInfo> routing_table_neighbors);

  uint16_t getFanout() const;
  NodeInfo getAdjacentLeft() const;
  NodeInfo getAdjacentRight() const;
  std::vector<NodeInfo> getRoutingTableNeighbors() const;

  SERIALIZE(header_, fanout_, adjacent_left_, adjacent_right_, routing_table_neighbors_);

  MessageJoinAccept() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
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
