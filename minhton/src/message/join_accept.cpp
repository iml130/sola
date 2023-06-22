// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/join_accept.h"

#include <algorithm>
#include <stdexcept>

#include "minhton/core/routing_calculations.h"
namespace minhton {

MessageJoinAccept::MessageJoinAccept(const MinhtonMessageHeader &header, uint16_t fanout,
                                     NodeInfo adjacent_left, NodeInfo adjacent_right,
                                     std::vector<NodeInfo> routing_table_neighbors)
    : header_(header),
      fanout_(fanout),
      adjacent_left_(std::move(adjacent_left)),
      adjacent_right_(std::move(adjacent_right)),
      routing_table_neighbors_(std::move(routing_table_neighbors)) {
  header_.setMessageType(MessageType::kJoinAccept);

  MessageLoggingAdditionalInfo logging_info{getAdjacentLeft().getLogicalNodeInfo().getUuid(),
                                            getAdjacentRight().getLogicalNodeInfo().getUuid(), ""};
  logging_info.content = "m=" + std::to_string(getFanout());
  header_.setAdditionalLoggingInfo(logging_info);

  validate();
}

bool MessageJoinAccept::validateImpl() const {
  if (std::any_of(routing_table_neighbors_.begin(), routing_table_neighbors_.end(),
                  [](const NodeInfo &node) { return !node.isInitialized(); })) {
    return false;
  }

  return isFanoutValid(fanout_) &&
         (adjacent_left_.isInitialized() || adjacent_right_.isInitialized());
}

uint16_t MessageJoinAccept::getFanout() const { return fanout_; }

NodeInfo MessageJoinAccept::getAdjacentLeft() const { return adjacent_left_; }

NodeInfo MessageJoinAccept::getAdjacentRight() const { return adjacent_right_; }

std::vector<NodeInfo> MessageJoinAccept::getRoutingTableNeighbors() const {
  return routing_table_neighbors_;
}

}  // namespace minhton
