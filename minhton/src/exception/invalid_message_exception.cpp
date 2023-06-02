// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/exception/invalid_message_exception.h"

namespace minhton {

InvalidMessageException::InvalidMessageException(const MinhtonMessageHeader &header) noexcept {
  // this->processing_mode_ = mode;
  this->message_type_ = header.getMessageType();

  // std::string mode_text;
  // if (mode == MessageProcessingModes::kReceiving) {
  //   mode_text = "Receiving";
  // } else {
  //   mode_text = "Sending";
  // }

  this->error_message_ = /*"InvalidMessageException: " + mode_text + " " +*/
      getMessageTypeString(header.getMessageType()) + " Sender: " + header.getSender().getString() +
      " - Target: " + header.getTarget().getString();
}

const char *InvalidMessageException::what() const noexcept { return this->error_message_.c_str(); }

}  // namespace minhton
