// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/utils/fsm.h"

namespace minhton {

std::string FiniteStateMachine::getStateString(FSMState state) {
  switch (state) {
    using minhton::FSMState;
    case kIdle:
      return "idle";
    case kWaitForBootstrapResponse:
      return "wait_for_bootstrap_response";
    case kWaitForJoinAccept:
      return "wait_for_join_accept";
    case kConnected:
      return "connected";
    case kConnectedAcceptingChild:
      return "connected_accepting_child";
    case kConnectedReplacing:
      return "connected_replacing";
    case kWaitForReplacementOffer:
      return "wait_for_replacement_offer";
    case kConnectedWaitingParentResponse:
      return "kConnectedWaitingParentResponse";
    case kConnectedWaitingParentResponseDirectLeaveWoReplacement:
      return "k_connected_waiting_parent_response_without_replacement";
    case kSignOffFromInlevelNeighborsDirectLeaveWoReplacement:
      return "kSignOffFromInlevelNeighborsDirectLeaveWoReplacement";
    case kErrorState:
      return "error";
    case kSignOffFromInlevelNeighbors:
      return "kSignOffFromInlevelNeighbors";
    case kJoinFailed:
      return "join_failed";
  }

  return "undefined";
}

bool FiniteStateMachine::joinNetworkSignalUsingBootstrap(const Signal &event) const {
  return event.signal_type == minhton::SignalType::kJoinNetwork && event.join_via_bootstrap;
}

bool FiniteStateMachine::joinNetworkSignalUsingAddress(const Signal &event) const {
  return event.signal_type == minhton::SignalType::kJoinNetwork && !event.join_via_bootstrap;
}

bool FiniteStateMachine::leaveNetworkSignalCanLeavePosition(const Signal &event) const {
  return event.signal_type == minhton::SignalType::kLeaveNetwork && event.can_leave_position;
}

bool FiniteStateMachine::leaveNetworkSignalCannotLeavePosition(const Signal &event) const {
  return event.signal_type == minhton::SignalType::kLeaveNetwork && !event.can_leave_position;
}

bool FiniteStateMachine::bootstrapResponseTimeoutInvalid(const Timeout &event) const {
  return event.timeout_type == minhton::TimeoutType::kBootstrapResponseTimeout &&
         !event.valid_bootstrap_response;
}

bool FiniteStateMachine::bootstrapResponseTimeoutValid(const Timeout &event) const {
  return event.timeout_type == minhton::TimeoutType::kBootstrapResponseTimeout &&
         event.valid_bootstrap_response;
}

bool FiniteStateMachine::joinResponseTimeout(const Timeout &event) const {
  return event.timeout_type == minhton::TimeoutType::kJoinAcceptResponseTimeout;
}

bool FiniteStateMachine::joinAcceptAckResponseTimeout(const Timeout &event) const {
  return event.timeout_type == minhton::TimeoutType::kJoinAcceptAckResponseTimeout;
}

bool FiniteStateMachine::replacementAckResponseTimeout(const Timeout &event) const {
  return event.timeout_type == minhton::TimeoutType::kReplacementAckResponseTimeout;
}

bool FiniteStateMachine::replacementOfferResponseTimeout(const Timeout &event) const {
  return event.timeout_type == minhton::TimeoutType::kReplacementOfferResponseTimeout;
}

bool FiniteStateMachine::timeoutNonCriticalMsgInConnectedState(const Timeout &event) const {
  // TODO unit tests
  return event.timeout_type == minhton::TimeoutType::kDsnAggregationTimeout ||
         event.timeout_type == minhton::TimeoutType::kInquiryAggregationTimeout;
}

bool FiniteStateMachine::recvJoinAcceptMessage(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kJoinAccept;
}

bool FiniteStateMachine::recvJoinAcceptAckMessage(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kJoinAcceptAck;
}

bool FiniteStateMachine::recvReplacementAckMessage(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kReplacementAck;
}

bool FiniteStateMachine::recvReplacementOfferMessage(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kReplacementOffer;
}

bool FiniteStateMachine::recvBootstrapDiscoverMessage(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kBootstrapDiscover;
}

bool FiniteStateMachine::recvBootstrapResponseMessage(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kBootstrapResponse;
}

bool FiniteStateMachine::sendJoinMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kJoin;
}

bool FiniteStateMachine::sendJoinAcceptMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kJoinAccept;
}

bool FiniteStateMachine::sendJoinAcceptAckMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kJoinAcceptAck;
}

bool FiniteStateMachine::sendReplacementOfferMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kReplacementOffer;
}

bool FiniteStateMachine::sendReplacementOfferNackMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kReplacementNack;
}

bool FiniteStateMachine::sendParentResponseMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kSignOffParentRequest;
}

bool FiniteStateMachine::sendReplacementAckMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kReplacementAck;
}

bool FiniteStateMachine::sendBootstrapDiscoverMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kBootstrapDiscover;
}

bool FiniteStateMachine::sendBootstrapResponseMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kBootstrapResponse;
}

bool FiniteStateMachine::recvFindReplacementAtNodeToReplace(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kFindReplacement &&
         event.does_not_need_replacement;
}

bool FiniteStateMachine::recvFindReplacement(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kFindReplacement;
}

bool FiniteStateMachine::recvParentResponseAck(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kSignOffParentAnswer;
}

bool FiniteStateMachine::recvFindReplacementNack(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kReplacementNack;
}

bool FiniteStateMachine::sendRemoveNeighborMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kRemoveNeighbor;
}

bool FiniteStateMachine::sendRemoveAndUpdateNeighbor(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kRemoveAndUpdateNeighbor;
}

bool FiniteStateMachine::sendUpdateNeighborsMessage(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kUpdateNeighbors;
}

bool FiniteStateMachine::recvNonCriticalMsgInConnectedState(const ReceiveMessage &event) const {
  return event.msg_type == minhton::MessageType::kJoin ||
         event.msg_type == minhton::MessageType::kUpdateNeighbors ||
         event.msg_type == minhton::MessageType::kRemoveNeighbor ||
         event.msg_type == minhton::MessageType::kRemoveAndUpdateNeighbor ||
         event.msg_type == minhton::MessageType::kGetNeighbors ||
         event.msg_type == minhton::MessageType::kInformAboutNeighbors ||
         event.msg_type == minhton::MessageType::kSearchExact ||
         event.msg_type == minhton::MessageType::kSearchExactFailure ||
         event.msg_type == minhton::MessageType::kEmpty ||
         event.msg_type == minhton::MessageType::kReplacementUpdate ||
         event.msg_type == minhton::MessageType::kFindReplacement ||
         event.msg_type == minhton::MessageType::kBootstrapDiscover ||
         event.msg_type == minhton::MessageType::kFindQueryAnswer ||
         event.msg_type == minhton::MessageType::kFindQueryRequest ||
         event.msg_type == minhton::MessageType::kAttributeInquiryAnswer ||
         event.msg_type == minhton::MessageType::kAttributeInquiryRequest ||
         event.msg_type == minhton::MessageType::kSubscriptionOrder ||
         event.msg_type == minhton::MessageType::kSubscriptionUpdate ||
         event.msg_type == minhton::MessageType::kLockNeighborRequest ||
         event.msg_type == minhton::MessageType::kLockNeighborResponse ||
         event.msg_type == minhton::MessageType::kRemoveNeighborAck ||
         event.msg_type == minhton::MessageType::kUnlockNeighbor ||
         event.msg_type == minhton::MessageType::kSignOffParentRequest;
}

bool FiniteStateMachine::sendNonCriticalMsgInConnectedState(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kJoin ||
         event.msg_type == minhton::MessageType::kUpdateNeighbors ||
         event.msg_type == minhton::MessageType::kRemoveNeighbor ||
         event.msg_type == minhton::MessageType::kGetNeighbors ||
         event.msg_type == minhton::MessageType::kInformAboutNeighbors ||
         event.msg_type == minhton::MessageType::kSearchExact ||
         event.msg_type == minhton::MessageType::kSearchExactFailure ||
         event.msg_type == minhton::MessageType::kEmpty ||
         event.msg_type == minhton::MessageType::kReplacementUpdate ||
         event.msg_type == minhton::MessageType::kFindReplacement ||
         event.msg_type == minhton::MessageType::kFindQueryAnswer ||
         event.msg_type == minhton::MessageType::kFindQueryRequest ||
         event.msg_type == minhton::MessageType::kAttributeInquiryAnswer ||
         event.msg_type == minhton::MessageType::kAttributeInquiryRequest ||
         event.msg_type == minhton::MessageType::kSubscriptionOrder ||
         event.msg_type == minhton::MessageType::kSubscriptionUpdate ||
         event.msg_type == minhton::MessageType::kSignOffParentRequest ||
         event.msg_type == minhton::MessageType::kSignOffParentAnswer ||
         event.msg_type == minhton::MessageType::kLockNeighborRequest ||
         event.msg_type == minhton::MessageType::kLockNeighborResponse ||
         event.msg_type == minhton::MessageType::kRemoveNeighborAck ||
         event.msg_type == minhton::MessageType::kUnlockNeighbor ||
         event.msg_type == minhton::MessageType::kReplacementNack;
}

bool FiniteStateMachine::sendLeaveRelatedMsgInIdleState(const SendMessage &event) const {
  return event.msg_type == minhton::MessageType::kUnlockNeighbor ||
         event.msg_type == minhton::MessageType::kSignOffParentRequest ||
         event.msg_type == minhton::MessageType::kReplacementAck;
}

}  // namespace minhton
