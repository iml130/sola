// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_UNLOCK_NEIGHBOR_H_
#define MINHTON_MESSAGE_UNLOCK_NEIGHBOR_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** Used for unlocking nodes after they were locked during a concurrent
/// operation like leave.
/// * **Algorithm Association:** Leave.
class MessageUnlockNeighbor : public MinhtonMessage<MessageUnlockNeighbor> {
public:
  explicit MessageUnlockNeighbor(MinhtonMessageHeader header);

  SERIALIZE(header_);

  MessageUnlockNeighbor() = default;

private:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;
};
}  // namespace minhton

#endif
