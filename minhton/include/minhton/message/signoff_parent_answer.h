// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_SIGNOFF_PARENT_ANSWER_H_
#define MINHTON_MESSAGE_SIGNOFF_PARENT_ANSWER_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
class MessageSignoffParentAnswer : public MinhtonMessage<MessageSignoffParentAnswer> {
public:
  explicit MessageSignoffParentAnswer(MinhtonMessageHeader header, bool successful = false);

  bool wasSuccessful() const;

  SERIALIZE(header_, successful_);

  MessageSignoffParentAnswer() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  /// A vector of all neighbors the leaving node has information about
  bool successful_ = false;
};
}  // namespace minhton

#endif
