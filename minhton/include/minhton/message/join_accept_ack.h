// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_JOIN_ACCEPT_ACK_H_
#define MINHTON_MESSAGE_JOIN_ACCEPT_ACK_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// After receiving a Join Accept message, the new child must send a Join Accept Ack back to the
/// new parent. It is needed to make sure the entering node still wants to join. Only after
/// receiving this message, the parent node will send update messages through the network to inform
/// the other neighbors about the joined node.
class MessageJoinAcceptAck : public MinhtonMessage<MessageJoinAcceptAck> {
public:
  explicit MessageJoinAcceptAck(MinhtonMessageHeader header);

  SERIALIZE(header_);

  MessageJoinAcceptAck() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;
};
}  // namespace minhton

#endif
