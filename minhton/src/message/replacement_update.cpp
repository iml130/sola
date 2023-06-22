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
                                                   LogicalNodeInfo new_l_node_info,
                                                   bool should_acknowledge)
    : header_(header),
      removed_position_node_(std::move(removed_position_node)),
      replaced_position_node_(std::move(replaced_position_node)),
      new_l_node_info_(new_l_node_info),
      should_acknowledge_(should_acknowledge) {
  header_.setMessageType(MessageType::kReplacementUpdate);
  MessageLoggingAdditionalInfo logging_info{
      getRemovedPositionNode().getLogicalNodeInfo().getUuid(),
      getReplacedPositionNode().getLogicalNodeInfo().getUuid(), ""};
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageReplacementUpdate::validateImpl() const {
  return removed_position_node_.isInitialized() && replaced_position_node_.isInitialized() &&
         new_l_node_info_.isInitialized();
}

minhton::NodeInfo MessageReplacementUpdate::getRemovedPositionNode() const {
  return removed_position_node_;
}

minhton::NodeInfo MessageReplacementUpdate::getReplacedPositionNode() const {
  return replaced_position_node_;
}

LogicalNodeInfo MessageReplacementUpdate::getNewLogicalNodeInfo() const { return new_l_node_info_; }

bool MessageReplacementUpdate::getShouldAcknowledge() const { return should_acknowledge_; }

}  // namespace minhton
