// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_SEARCH_EXACT_FAILURE_H_
#define MINHTON_MESSAGE_SEARCH_EXACT_FAILURE_H_

#include "minhton/message/message.h"
#include "minhton/message/se_types.h"
#include "minhton/message/serialize.h"

namespace minhton {

/// A node wants to send a message to another node in the network, but does not know the physical
/// address, only the tree position. The Search Exact message gets forwarded into the right
/// direction until it reaches its destination.
class MessageSearchExactFailure : public MinhtonMessage<MessageSearchExactFailure> {
public:
  MessageSearchExactFailure(MinhtonMessageHeader header, NodeInfo destination_node,
                            std::shared_ptr<MessageSEVariant> query);

  minhton::NodeInfo getDestinationNode() const;
  std::shared_ptr<MessageSEVariant> getQuery() const;

  SERIALIZE(header_, destination_node_, query_);

  MessageSearchExactFailure() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  /// The destination node of the message. (This is not the target node. The target is only the node
  /// the message gets forwarded to.)
  minhton::NodeInfo destination_node_;

  /// Another internal message of any other message type which the inital sender wants to send to
  /// the destination node. Within the query message, the sender is the inital sender and the target
  /// the destination node.
  std::shared_ptr<MessageSEVariant> query_;
};
}  // namespace minhton

#endif
