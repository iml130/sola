// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/remove_neighbor.h"
namespace minhton {

MessageRemoveNeighbor::MessageRemoveNeighbor(MinhtonMessageHeader header,
                                             NodeInfo removed_position_node, bool acknowledge)
    : header_(std::move(header)),
      removed_position_node_(std::move(removed_position_node)),
      acknowledge_(acknowledge) {
  header_.setMessageType(MessageType::kRemoveNeighbor);
  MessageLoggingAdditionalInfo logging_info{getRemovedPositionNode().getLogicalNodeInfo().getUuid(),
                                            "", ""};
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageRemoveNeighbor::validateImpl() const {
  return getRemovedPositionNode().isInitialized();
}

NodeInfo MessageRemoveNeighbor::getRemovedPositionNode() const { return removed_position_node_; }

bool MessageRemoveNeighbor::getShouldAcknowledge() const { return acknowledge_; }

}  // namespace minhton
