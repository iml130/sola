// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_JOIN_ACCEPT_ACK_H_
#define MINHTON_MESSAGE_JOIN_ACCEPT_ACK_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** After receiving a MessageJoinAccept, the new child must send a
/// MessageJoinAcceptAck back to the new parent. It is needed to make sure the entering node still
/// wants to join. Only after receiving this message, the parent node will send update messages
/// through the network to inform the other neighbors about the joined node.
/// * **Algorithm Association:** Join.
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
