// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/search_exact_failure.h"

namespace minhton {
MessageSearchExactFailure::MessageSearchExactFailure(MinhtonMessageHeader header,
                                                     NodeInfo destination_node,
                                                     std::shared_ptr<MessageSEVariant> query)
    : header_(std::move(header)),
      destination_node_(std::move(destination_node)),
      query_(std::move(query)) {
  header_.setMessageType(MessageType::kSearchExactFailure);

  MessageLoggingAdditionalInfo logging_info{getDestinationNode().getLogicalNodeInfo().getUuid(), "",
                                            ""};
  MessageType type = std::visit(
      [](const auto &msg) -> MessageType { return msg.getHeader().getMessageType(); }, *query_);
  logging_info.content = "failure_query_msg_type=" + getMessageTypeString(type);
  header_.setAdditionalLoggingInfo(logging_info);

  validate();
}

bool MessageSearchExactFailure::validateImpl() const {
  // dont call query->validate() here, because
  // the network information of the target are
  // always unknown when sending a search exact message

  return getDestinationNode().isValidPeer() && getQuery() != nullptr;
}

NodeInfo MessageSearchExactFailure::getDestinationNode() const { return destination_node_; }

std::shared_ptr<MessageSEVariant> MessageSearchExactFailure::getQuery() const { return query_; }

}  // namespace minhton
