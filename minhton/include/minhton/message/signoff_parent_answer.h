// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_SIGNOFF_PARENT_ANSWER_H_
#define MINHTON_MESSAGE_SIGNOFF_PARENT_ANSWER_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** The successor node waits for the MessageSignoffParentAnswer from its parent
/// after sending a MessageSignoffParentRequest to it.
/// * **Algorithm Association:** Leave.
class MessageSignoffParentAnswer : public MinhtonMessage<MessageSignoffParentAnswer> {
public:
  /// @param successful Indicates whether the leave process is allowed to continue.
  explicit MessageSignoffParentAnswer(MinhtonMessageHeader header, bool successful = false);

  bool wasSuccessful() const;

  SERIALIZE(header_, successful_);

  MessageSignoffParentAnswer() = default;

private:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

  /// A vector of all neighbors the leaving node has information about
  bool successful_ = false;
};
}  // namespace minhton

#endif
