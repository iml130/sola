// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/signoff_parent_answer.h"

#include <algorithm>
namespace minhton {

MessageSignoffParentAnswer::MessageSignoffParentAnswer(const MinhtonMessageHeader &header,
                                                       bool successful)
    : header_(header), successful_(successful) {
  header_.setMessageType(MessageType::kSignOffParentAnswer);
  validate();
}

bool MessageSignoffParentAnswer::validateImpl() const { return true; }

bool MessageSignoffParentAnswer::wasSuccessful() const { return successful_; }

}  // namespace minhton
