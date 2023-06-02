// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/replacement_update.h"

namespace minhton {

MessageReplacementUpdate::MessageReplacementUpdate(const MinhtonMessageHeader &header,
                                                   NodeInfo removed_position_node,
                                                   NodeInfo replaced_position_node,
                                                   PeerInfo new_peer, bool should_acknowledge)
    : header_(header),
      removed_position_node_(std::move(removed_position_node)),
      replaced_position_node_(std::move(replaced_position_node)),
      new_peer_(new_peer),
      should_acknowledge_(should_acknowledge) {
  header_.setMessageType(MessageType::kReplacementUpdate);
  MessageLoggingAdditionalInfo logging_info{getRemovedPositionNode().getPeerInfo().getUuid(),
                                            getReplacedPositionNode().getPeerInfo().getUuid(), ""};
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageReplacementUpdate::validateImpl() const {
  return removed_position_node_.isInitialized() && replaced_position_node_.isInitialized() &&
         new_peer_.isInitialized();
}

minhton::NodeInfo MessageReplacementUpdate::getRemovedPositionNode() const {
  return removed_position_node_;
}

minhton::NodeInfo MessageReplacementUpdate::getReplacedPositionNode() const {
  return replaced_position_node_;
}

PeerInfo MessageReplacementUpdate::getNewPeer() const { return new_peer_; }

bool MessageReplacementUpdate::getShouldAcknowledge() const { return should_acknowledge_; }

}  // namespace minhton
