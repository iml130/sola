// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_INVALID_MESSAGE_EXCEPTION_H_
#define MINHTON_INVALID_MESSAGE_EXCEPTION_H_

#include <cstdint>
#include <stdexcept>
#include <string>

#include "minhton/message/message_header.h"

namespace minhton {

class InvalidMessageException : public std::exception {
public:
  explicit InvalidMessageException(const MinhtonMessageHeader &header) noexcept;
  ~InvalidMessageException() override = default;
  const char *what() const noexcept override;

private:
  MessageType message_type_;
  std::string error_message_;
  // MessageProcessingModes processing_mode_;  // sending or receiving
};

}  // namespace minhton
#endif
