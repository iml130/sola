// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_REPLACEMENT_OFFER_H_
#define MINHTON_MESSAGE_REPLACEMENT_OFFER_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** A MessageFindReplacement reached a node that can replace the leaving node.
/// The node sends a MessageReplacementOffer to the leaving node to initiate the replacement
/// process.
/// * **Algorithm Association:** Leave.
class MessageReplacementOffer : public MinhtonMessage<MessageReplacementOffer> {
public:
  explicit MessageReplacementOffer(MinhtonMessageHeader header);

  SERIALIZE(header_);

  MessageReplacementOffer() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;
};
}  // namespace minhton

#endif
