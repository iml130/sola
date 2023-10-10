// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_EMPTY_H_
#define MINHTON_MESSAGE_EMPTY_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** A helper message type for the Search Exact Test. It has no functionality by
/// itself. Not to be used outside of Search Exact Tests.
/// * **Algorithm Association:** Search Exact.
class MessageEmpty : public MinhtonMessage<MessageEmpty> {
public:
  explicit MessageEmpty(MinhtonMessageHeader header);

  SERIALIZE(header_)

  MessageEmpty() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;
};
}  // namespace minhton

#endif
