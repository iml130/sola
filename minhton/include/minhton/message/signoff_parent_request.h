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
/// @brief * **Usage:** The last node of the network, the chosen successor, sends a
/// MessageSignoffParentRequest to its parent. The parent later answers with a
/// MessageSignoffParentAnswer to notify the successor if the leave is allowed to continue.
/// * **Algorithm Association:** Leave.
class MessageSignoffParentRequest : public MinhtonMessage<MessageSignoffParentRequest> {
public:
  explicit MessageSignoffParentRequest(MinhtonMessageHeader header);

  SERIALIZE(header_)

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
