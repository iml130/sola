// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/search_exact.h"

namespace minhton {
MessageSearchExact::MessageSearchExact(MinhtonMessageHeader header, NodeInfo destination_node,
                                       std::shared_ptr<MessageSEVariant> query,
                                       bool notify_about_failure)
    : header_(std::move(header)),
      destination_node_(std::move(destination_node)),
      query_(std::move(query)),
      notify_about_failure_(notify_about_failure) {
  header_.setMessageType(MessageType::kSearchExact);

  MessageLoggingAdditionalInfo logging_info{};
  MessageType type = std::visit(
      [](const auto &msg) -> MessageType { return msg.getHeader().getMessageType(); }, *query_);
  logging_info.content = "query_destination=(" +
                         getDestinationNode().getLogicalNodeInfo().getString() +
                         "), query_msg_type=" + getMessageTypeString(type);
  header_.setAdditionalLoggingInfo(logging_info);

  validate();
}

bool MessageSearchExact::validateImpl() const {
  // dont call query->validate() here, because
  // the network information of the target are
  // always unknown when sending a search exact message

  return query_ != nullptr && getDestinationNode().isValidPeer();
}

NodeInfo MessageSearchExact::getDestinationNode() const { return destination_node_; }

std::shared_ptr<MessageSEVariant> MessageSearchExact::getQuery() const { return query_; }

bool MessageSearchExact::getNotifyAboutFailure() const { return notify_about_failure_; }

}  // namespace minhton
