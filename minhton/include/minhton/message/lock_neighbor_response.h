// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_LOCK_NEIGHBOR_RESPONSE_H_
#define MINHTON_MESSAGE_LOCK_NEIGHBOR_RESPONSE_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** The answer to a MessageLockNeighborRequest, received by the parent of a
/// successor node.
/// * **Algorithm Association:** Leave.
class MessageLockNeighborResponse : public MinhtonMessage<MessageLockNeighborResponse> {
public:
  /// @param successful Indicates whether a node could successfully lock itself or not.
  explicit MessageLockNeighborResponse(MinhtonMessageHeader header, bool successful = false);

  bool wasSuccessful() const;

  SERIALIZE(header_, successful_);

  MessageLockNeighborResponse() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  bool successful_ = false;
};
}  // namespace minhton

#endif
