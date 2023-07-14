// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_EXCEPTION_H_
#define MINHTON_ALGORITHMS_EXCEPTION_H_

#include <cstdint>
#include <stdexcept>
#include <string>

namespace minhton {

enum AlgorithmType : uint8_t {
  kUndefinedAlgorithm = 0,
  kJoinAlgorithm = 1,
  kLeaveAlgorithm = 2,
  kUpdatingAlgorithm = 4,
};

class AlgorithmException : public std::exception {
public:
  explicit AlgorithmException(const std::string &message) noexcept;
  AlgorithmException(AlgorithmType type, const std::string &message) noexcept;
  ~AlgorithmException() override = default;
  const char *what() const noexcept override;
  static std::string getAlgorithmTypeString(AlgorithmType type);

private:
  AlgorithmType type_;
  std::string error_message_;
};

}  // namespace minhton
#endif
