// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_REPLACEMENT_OFFER_H_
#define MINHTON_MESSAGE_REPLACEMENT_OFFER_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// A Find Replacement Message reached a node that can replace the leaving node. The node sends a
/// Replacement Offer to the leaving node to initiate the replacement process.
class MessageReplacementOffer : public MinhtonMessage<MessageReplacementOffer> {
public:
  explicit MessageReplacementOffer(const MinhtonMessageHeader &header);

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
