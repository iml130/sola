// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_FSM_EXCEPTION_H_
#define MINHTON_FSM_EXCEPTION_H_

#include <cstdint>
#include <stdexcept>
#include <string>

#include "minhton/utils/fsm.h"

namespace minhton {

class FSMException : public std::exception {
public:
  explicit FSMException(const std::string &message) noexcept;

  FSMException(FSMState previous_state, SendMessage event, const std::string &message) noexcept;
  FSMException(FSMState previous_state, ReceiveMessage event, const std::string &message) noexcept;
  FSMException(FSMState previous_state, Signal event, const std::string &message) noexcept;
  FSMException(FSMState previous_state, Timeout event, const std::string &message) noexcept;

  ~FSMException() override = default;
  const char *what() const noexcept override;

private:
  std::string error_message_;
};

}  // namespace minhton
#endif
