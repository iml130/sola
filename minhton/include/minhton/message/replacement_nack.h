// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_REPLACEMENT_NACK_H_
#define MINHTON_MESSAGE_REPLACEMENT_NACK_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** A MessageReplacementNack is sent back to the node to replace in case of a
/// failure during the leave process. A failure can occur when the chosen successor is already
/// replacing enother node or a MessageSignOffParentAnswer informed the current node about an
/// unsucessful sign off from the parent.
/// * **Algorithm Association:** Leave.
class MessageReplacementNack : public MinhtonMessage<MessageReplacementNack> {
public:
  explicit MessageReplacementNack(MinhtonMessageHeader header);

  SERIALIZE(header_);

  MessageReplacementNack() = default;

private:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;
};
}  // namespace minhton

#endif
