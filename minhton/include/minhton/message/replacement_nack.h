// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_REPLACEMENT_NACK_H_
#define MINHTON_MESSAGE_REPLACEMENT_NACK_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
class MessageReplacementNack : public MinhtonMessage<MessageReplacementNack> {
public:
  explicit MessageReplacementNack(const MinhtonMessageHeader &header);

  SERIALIZE(header_);

  MessageReplacementNack() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;
};
}  // namespace minhton

#endif
