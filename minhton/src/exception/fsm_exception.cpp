// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/exception/fsm_exception.h"

#include "minhton/message/message.h"

namespace minhton {

FSMException::FSMException(const std::string &message) noexcept {
  this->error_message_ = "FSMException: " + message;
}

FSMException::FSMException(FSMState previous_state, SendMessage event,
                           const std::string &message) noexcept {
  std::string state_text =
      "Previous FSMState: " + FiniteStateMachine::getStateString(previous_state);
  std::string event_text = " Event: SendMessage " + getMessageTypeString(event.msg_type);
  std::string message_text;
  if (!message.empty()) {
    message_text += "Message: " + message;
  }
  this->error_message_ = state_text + " - " + event_text + " - " + message_text;
}

FSMException::FSMException(FSMState previous_state, ReceiveMessage event,
                           const std::string &message) noexcept {
  std::string state_text =
      "Previous FSMState: " + FiniteStateMachine::getStateString(previous_state);
  std::string event_text = " Event: ReceiveMessage " + getMessageTypeString(event.msg_type);
  std::string message_text;
  if (!message.empty()) {
    message_text += "Message: " + message;
  }
  this->error_message_ = state_text + " - " + event_text + " - " + message_text;
}

FSMException::FSMException(FSMState previous_state, Signal event,
                           const std::string &message) noexcept {
  std::string state_text =
      "Previous FSMState: " + FiniteStateMachine::getStateString(previous_state);
  std::string message_text;
  if (!message.empty()) {
    message_text += "Message: " + message;
  }
  std::string event_text = " Event: Signal ";

  if (event.signal_type == SignalType::kJoinNetwork) {
    if (event.join_via_bootstrap) {
      event_text += "Join via Bootstrap";
    } else {
      event_text +=
          "Join via Address to " + event.join_address + ":" + std::to_string(event.join_port);
    }
  } else if (event.signal_type == SignalType::kLeaveNetwork) {
    if (event.can_leave_position) {
      event_text += "Leave - can leave Position without Replacement";
    } else {
      event_text += "Leave - needs to be replaced";
    }
  }

  this->error_message_ = state_text + " - " + event_text + " - " + message_text;
}

FSMException::FSMException(FSMState previous_state, Timeout event,
                           const std::string &message) noexcept {
  std::string state_text =
      "Previous FSMState: " + FiniteStateMachine::getStateString(previous_state);
  std::string message_text;
  if (!message.empty()) {
    message_text += "Message: " + message;
  }
  std::string event_text = " Event: Timeout ";

  if (event.timeout_type == TimeoutType::kBootstrapResponseTimeout) {
    event_text += "Bootstrap Response ";
    if (event.valid_bootstrap_response) {
      event_text += "valid";
    } else {
      event_text += "invalid";
    }
  } else if (event.timeout_type == TimeoutType::kJoinAcceptAckResponseTimeout) {
    event_text += "Join Accept Ack Response";
  } else if (event.timeout_type == TimeoutType::kJoinAcceptResponseTimeout) {
    event_text += "Join Accept Response";
  } else if (event.timeout_type == TimeoutType::kReplacementAckResponseTimeout) {
    event_text += "Replacement Ack Response";
  } else if (event.timeout_type == TimeoutType::kReplacementOfferResponseTimeout) {
    event_text += "Replacement Offer Response";
  }

  this->error_message_ = state_text + " - " + event_text + " - " + message_text;
}

const char *FSMException::what() const noexcept { return this->error_message_.c_str(); }

}  // namespace minhton
