// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_LOCK_NEIGHBOR_REQUEST_H_
#define MINHTON_MESSAGE_LOCK_NEIGHBOR_REQUEST_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** The parent of a successor node sends a Lock Neighbor Request to its right
/// and left neighbor. The receivers try to lock themselves for the leave procedure and respond with
/// a MessageLockNeighborResponse, indicating success or failure of locking themselves.
/// * **Algorithm Association:** Leave.
class MessageLockNeighborRequest : public MinhtonMessage<MessageLockNeighborRequest> {
public:
  explicit MessageLockNeighborRequest(MinhtonMessageHeader header);

  SERIALIZE(header_);

  MessageLockNeighborRequest() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;
};
}  // namespace minhton

#endif
