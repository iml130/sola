// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/replacement_nack.h"

namespace minhton {

MessageReplacementNack::MessageReplacementNack(MinhtonMessageHeader header)
    : header_(std::move(header)) {
  header_.setMessageType(MessageType::kReplacementNack);
  validate();
}

bool MessageReplacementNack::validateImpl() const { return true; }

}  // namespace minhton
