// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_BOOTSTRAP_RESPONSE_H_
#define MINHTON_MESSAGE_BOOTSTRAP_RESPONSE_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// After receiving a Boostrap Discover message, the node receiving the message might answer with a
/// Bootstrap Response message, in case the node is in a connected state and a fitting join parent.
class MessageBootstrapResponse : public MinhtonMessage<MessageBootstrapResponse> {
public:
  MessageBootstrapResponse(const MinhtonMessageHeader &header, NodeInfo node_to_join);

  minhton::NodeInfo getNodeToJoin() const;

  SERIALIZE(header_, node_to_join_);

  MessageBootstrapResponse() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  // TODO maybe change to an array?!
  /// The node information the sender of the Bootstrap Discover message may join to.
  NodeInfo node_to_join_;
};
}  // namespace minhton

#endif
