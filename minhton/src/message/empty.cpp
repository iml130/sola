// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/empty.h"

namespace minhton {

MessageEmpty::MessageEmpty(const MinhtonMessageHeader &header) : header_(header) {
  header_.setMessageType(MessageType::kEmpty);
  validate();
}

bool MessageEmpty::validateImpl() const { return true; }
}  // namespace minhton
