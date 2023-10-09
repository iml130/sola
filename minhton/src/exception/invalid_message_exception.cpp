// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/exception/invalid_message_exception.h"

namespace minhton {

InvalidMessageException::InvalidMessageException(const MinhtonMessageHeader &header) noexcept
    : message_type_(header.getMessageType()) {
  this->error_message_ = /*"InvalidMessageException: " + mode_text + " " +*/
      getMessageTypeString(header.getMessageType()) + " Sender: " + header.getSender().getString() +
      " - Target: " + header.getTarget().getString();
}

const char *InvalidMessageException::what() const noexcept { return this->error_message_.c_str(); }

}  // namespace minhton
