// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_SIGNOFF_PARENT_REQUEST_H_
#define MINHTON_MESSAGE_SIGNOFF_PARENT_REQUEST_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
class MessageSignoffParentRequest : public MinhtonMessage<MessageSignoffParentRequest> {
public:
  explicit MessageSignoffParentRequest(const MinhtonMessageHeader &header);

  SERIALIZE(header_);

  MessageSignoffParentRequest() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;
};
}  // namespace minhton

#endif
