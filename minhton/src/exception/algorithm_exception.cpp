// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/exception/algorithm_exception.h"

namespace minhton {

AlgorithmException::AlgorithmException(const std::string &message) noexcept
    : type_(AlgorithmType::kUndefinedAlgorithm),
      error_message_(AlgorithmException::getAlgorithmTypeString(this->type_) + message) {}

AlgorithmException::AlgorithmException(AlgorithmType type, const std::string &message) noexcept
    : type_(type), error_message_(AlgorithmException::getAlgorithmTypeString(type) + message) {}

const char *AlgorithmException::what() const noexcept { return this->error_message_.c_str(); }

std::string AlgorithmException::getAlgorithmTypeString(AlgorithmType type) {
  std::string type_string;

  if (type == AlgorithmType::kUndefinedAlgorithm) {
    type_string = "AlgorithmException: ";
  } else if (type == AlgorithmType::kJoinAlgorithm) {
    type_string = "JoinAlgorithmException: ";
  } else if (type == AlgorithmType::kLeaveAlgorithm) {
    type_string = "LeaveAlgorithmException: ";
  }
  return type_string;
}

};  // namespace minhton
