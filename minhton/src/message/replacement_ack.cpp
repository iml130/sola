// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/replacement_ack.h"

#include <algorithm>

namespace minhton {

MessageReplacementAck::MessageReplacementAck(MinhtonMessageHeader header,
                                             std::vector<NodeInfo> neighbors,
                                             LockedStates lockedStates)
    : header_(std::move(header)),
      neighbors_(std::move(neighbors)),
      locked_(lockedStates.locked),
      locked_right_(lockedStates.locked_right),
      locked_left_(lockedStates.locked_left) {
  header_.setMessageType(MessageType::kReplacementAck);

  MessageLoggingAdditionalInfo logging_info;
  std::string text = "neighbors={";
  for (auto const &neighbor : neighbors_) {
    std::string current_text = neighbor.getString() + ",";
    text += current_text;
  }
  text += "}";
  logging_info.content = text;
  header_.setAdditionalLoggingInfo(logging_info);

  validate();
}

bool MessageReplacementAck::validateImpl() const {
  // Check if all neighbors are initialized
  return std::all_of(neighbors_.begin(), neighbors_.end(),
                     [](const NodeInfo &node) { return node.isInitialized(); });
}

std::vector<NodeInfo> MessageReplacementAck::getNeighbors() const { return neighbors_; }

MessageReplacementAck::LockedStates MessageReplacementAck::getLockedStates() const {
  return LockedStates{locked_, locked_right_, locked_left_};
}

}  // namespace minhton
