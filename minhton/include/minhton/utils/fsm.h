// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_UTILS_FSM_H_
#define MINHTON_UTILS_FSM_H_

#include "fsmlite/fsm.h"
#include "minhton/core/constants.h"
#include "minhton/core/node_info.h"
#include "minhton/message/types.h"

namespace minhton {

struct ReceiveMessage {
  minhton::MessageType msg_type = minhton::kInit;
  bool does_not_need_replacement = false;
};

struct SendMessage {
  minhton::MessageType msg_type = minhton::kInit;
};

struct Signal {
  minhton::SignalType signal_type;
  bool can_leave_position = false;
  bool join_via_bootstrap = false;
  std::string join_address;
  uint16_t join_port = 0;
  NodeInfo join_nodeinfo = NodeInfo();

  static Signal joinNetworkViaNodeInfo(const NodeInfo &ni) {
    return Signal{SignalType::kJoinNetwork, false, false, ni.getAddress(), ni.getPort(), ni};
  }
  static Signal joinNetworkViaAddress(std::string addr, uint16_t port) {
    return Signal{SignalType::kJoinNetwork, false, false, addr, port};
  }
  static Signal joinNetworkViaBootstrap(std::string addr, uint16_t port) {
    return Signal{SignalType::kJoinNetwork, false, true, addr, port};
  }
  static Signal joinNetworkViaBootstrap() {
    return Signal{SignalType::kJoinNetwork, false, true, ""};
  }
  static Signal leaveNetwork() { return Signal{SignalType::kLeaveNetwork, false, false, ""}; }
};

struct Timeout {
  minhton::TimeoutType timeout_type;
  bool valid_bootstrap_response = false;
};

class FiniteStateMachine : public fsmlite::fsm<FiniteStateMachine> {
  friend class fsm;  // base class needs access to transition_table

public:
  using event = int;

  explicit FiniteStateMachine(state_type init_state = kIdle) : fsm(init_state) {}

  bool isActionValid() {
    if (this->valid_action_) {
      this->valid_action_ = false;
      return true;
    }
    return false;
  }

  static std::string getStateString(FSMState state);

private:
  // actions
  void validAction([[maybe_unused]] const Signal &event) { this->valid_action_ = true; }
  void validAction([[maybe_unused]] const Timeout &event) { this->valid_action_ = true; }
  void validAction([[maybe_unused]] const ReceiveMessage &event) { this->valid_action_ = true; }
  void validAction([[maybe_unused]] const SendMessage &event) { this->valid_action_ = true; }

  // guards
  bool joinNetworkSignalUsingBootstrap(const Signal &event) const;
  bool joinNetworkSignalUsingAddress(const Signal &event) const;
  bool leaveNetworkSignalCanLeavePosition(const Signal &event) const;
  bool leaveNetworkSignalCannotLeavePosition(const Signal &event) const;

  bool bootstrapResponseTimeoutInvalid(const Timeout &event) const;
  bool bootstrapResponseTimeoutValid(const Timeout &event) const;
  bool joinResponseTimeout(const Timeout &event) const;
  bool joinAcceptAckResponseTimeout(const Timeout &event) const;
  bool replacementAckResponseTimeout(const Timeout &event) const;
  bool replacementOfferResponseTimeout(const Timeout &event) const;
  bool timeoutNonCriticalMsgInConnectedState(const Timeout &event) const;

  bool recvJoinAcceptMessage(const ReceiveMessage &event) const;
  bool recvJoinAcceptAckMessage(const ReceiveMessage &event) const;
  bool recvReplacementAckMessage(const ReceiveMessage &event) const;
  bool recvReplacementOfferMessage(const ReceiveMessage &event) const;
  bool recvBootstrapDiscoverMessage(const ReceiveMessage &event) const;
  bool recvBootstrapResponseMessage(const ReceiveMessage &event) const;
  bool recvNonCriticalMsgInConnectedState(const ReceiveMessage &event) const;
  bool recvFindReplacementAtNodeToReplace(const ReceiveMessage &event) const;
  bool recvFindReplacementNack(const ReceiveMessage &event) const;
  bool recvFindReplacement(const ReceiveMessage &event) const;
  bool recvParentResponseAck(const ReceiveMessage &event) const;

  bool sendJoinMessage(const SendMessage &event) const;
  bool sendJoinAcceptMessage(const SendMessage &event) const;
  bool sendJoinAcceptAckMessage(const SendMessage &event) const;
  bool sendReplacementOfferMessage(const SendMessage &event) const;
  bool sendReplacementOfferNackMessage(const SendMessage &event) const;
  bool sendParentResponseMessage(const SendMessage &event) const;
  bool sendReplacementAckMessage(const SendMessage &event) const;
  bool sendBootstrapDiscoverMessage(const SendMessage &event) const;
  bool sendBootstrapResponseMessage(const SendMessage &event) const;
  bool sendNonCriticalMsgInConnectedState(const SendMessage &event) const;
  bool sendLeaveRelatedMsgInIdleState(const SendMessage &event) const;
  bool sendRemoveNeighborMessage(const SendMessage &event) const;
  bool sendRemoveAndUpdateNeighbor(const SendMessage &event) const;
  bool sendUpdateNeighborsMessage(const SendMessage &event) const;

  bool valid_action_ = false;

  using m = FiniteStateMachine;

  using transition_table = table<
      //         Start-State - Event - Target-State - Action - Guard

      // workflow transitions
      mem_fn_row<kIdle, Signal, kWaitForBootstrapResponse, &m::validAction,
                 &m::joinNetworkSignalUsingBootstrap>,
      mem_fn_row<kIdle, Signal, kWaitForJoinAccept, &m::validAction,
                 &m::joinNetworkSignalUsingAddress>,
      mem_fn_row<kJoinFailed, Signal, kWaitForJoinAccept, &m::validAction,
                 &m::joinNetworkSignalUsingAddress>,
      mem_fn_row<kWaitForBootstrapResponse, Timeout, kErrorState, &m::validAction,
                 &m::bootstrapResponseTimeoutInvalid>,
      mem_fn_row<kWaitForBootstrapResponse, Timeout, kWaitForJoinAccept, &m::validAction,
                 &m::bootstrapResponseTimeoutValid>,
      mem_fn_row<kWaitForJoinAccept, Timeout, kJoinFailed, &m::validAction,
                 &m::joinResponseTimeout>,
      mem_fn_row<kWaitForJoinAccept, ReceiveMessage, kConnected, &m::validAction,
                 &m::recvJoinAcceptMessage>,
      mem_fn_row<kConnected, SendMessage, kConnectedAcceptingChild, &m::validAction,
                 &m::sendJoinAcceptMessage>,
      mem_fn_row<kConnectedAcceptingChild, ReceiveMessage, kConnected, &m::validAction,
                 &m::recvJoinAcceptAckMessage>,
      mem_fn_row<kConnectedAcceptingChild, Timeout, kConnected, &m::validAction,
                 &m::joinAcceptAckResponseTimeout>,
      mem_fn_row<kConnected, SendMessage, kConnectedWaitingParentResponse, &m::validAction,
                 &m::sendParentResponseMessage>,
      mem_fn_row<kConnectedWaitingParentResponse, SendMessage, kConnectedWaitingParentResponse,
                 &m::validAction, &m::sendReplacementOfferNackMessage>,
      mem_fn_row<kConnectedWaitingParentResponse, ReceiveMessage, kSignOffFromInlevelNeighbors,
                 &m::validAction, &m::recvParentResponseAck>,
      mem_fn_row<kConnectedWaitingParentResponseDirectLeaveWoReplacement, ReceiveMessage,
                 kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, &m::validAction,
                 &m::recvParentResponseAck>,
      mem_fn_row<kSignOffFromInlevelNeighbors, SendMessage, kConnectedReplacing, &m::validAction,
                 &m::sendReplacementOfferMessage>,
      mem_fn_row<kSignOffFromInlevelNeighbors, SendMessage, kSignOffFromInlevelNeighbors,
                 &m::validAction, &m::sendRemoveNeighborMessage>,
      mem_fn_row<kSignOffFromInlevelNeighbors, SendMessage, kSignOffFromInlevelNeighbors,
                 &m::validAction, &m::sendUpdateNeighborsMessage>,
      mem_fn_row<kSignOffFromInlevelNeighbors, SendMessage, kSignOffFromInlevelNeighbors,
                 &m::validAction, &m::sendRemoveAndUpdateNeighbor>,
      mem_fn_row<kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, SendMessage,
                 kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, &m::validAction,
                 &m::sendRemoveNeighborMessage>,
      mem_fn_row<kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, SendMessage,
                 kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, &m::validAction,
                 &m::sendUpdateNeighborsMessage>,
      mem_fn_row<kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, SendMessage,
                 kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, &m::validAction,
                 &m::sendRemoveAndUpdateNeighbor>,
      mem_fn_row<kConnectedReplacing, ReceiveMessage, kConnected, &m::validAction,
                 &m::recvReplacementAckMessage>,
      mem_fn_row<kConnectedReplacing, Timeout, kConnected, &m::validAction,
                 &m::replacementAckResponseTimeout>,
      mem_fn_row<kConnected, Signal, kConnectedWaitingParentResponseDirectLeaveWoReplacement,
                 &m::validAction, &m::leaveNetworkSignalCanLeavePosition>,
      mem_fn_row<kConnected, Signal, kWaitForReplacementOffer, &m::validAction,
                 &m::leaveNetworkSignalCannotLeavePosition>,
      mem_fn_row<kWaitForReplacementOffer, Timeout, kConnected, &m::validAction,
                 &m::replacementOfferResponseTimeout>,
      mem_fn_row<kWaitForReplacementOffer, ReceiveMessage, kIdle, &m::validAction,
                 &m::recvReplacementOfferMessage>,
      mem_fn_row<kConnected, SendMessage, kConnected, &m::validAction,
                 &m::sendJoinAcceptAckMessage>,

      // transitions needed for the workflow
      mem_fn_row<kWaitForJoinAccept, SendMessage, kWaitForJoinAccept, &m::validAction,
                 &m::sendJoinMessage>,
      mem_fn_row<kWaitForBootstrapResponse, SendMessage, kWaitForBootstrapResponse, &m::validAction,
                 &m::sendBootstrapDiscoverMessage>,
      mem_fn_row<kWaitForBootstrapResponse, ReceiveMessage, kWaitForBootstrapResponse,
                 &m::validAction, &m::recvBootstrapResponseMessage>,
      mem_fn_row<kIdle, SendMessage, kIdle, &m::validAction, &m::sendLeaveRelatedMsgInIdleState>,

      // non-critical connected recv transitions
      mem_fn_row<kConnected, ReceiveMessage, kConnected, &m::validAction,
                 &m::recvNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedAcceptingChild, ReceiveMessage, kConnectedAcceptingChild,
                 &m::validAction, &m::recvNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedReplacing, ReceiveMessage, kConnectedReplacing, &m::validAction,
                 &m::recvNonCriticalMsgInConnectedState>,
      mem_fn_row<kWaitForReplacementOffer, ReceiveMessage,
                 kConnectedWaitingParentResponseDirectLeaveWoReplacement, &m::validAction,
                 &m::recvFindReplacementAtNodeToReplace>,
      mem_fn_row<kWaitForReplacementOffer, ReceiveMessage, kWaitForReplacementOffer,
                 &m::validAction, &m::recvNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedWaitingParentResponse, ReceiveMessage, kConnectedWaitingParentResponse,
                 &m::validAction, &m::recvNonCriticalMsgInConnectedState>,
      mem_fn_row<kSignOffFromInlevelNeighbors, ReceiveMessage, kSignOffFromInlevelNeighbors,
                 &m::validAction, &m::recvNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedWaitingParentResponseDirectLeaveWoReplacement, ReceiveMessage,
                 kConnectedWaitingParentResponseDirectLeaveWoReplacement, &m::validAction,
                 &m::recvNonCriticalMsgInConnectedState>,
      mem_fn_row<kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, ReceiveMessage,
                 kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, &m::validAction,
                 &m::recvNonCriticalMsgInConnectedState>,
      mem_fn_row<kWaitForReplacementOffer, ReceiveMessage, kConnected, &m::validAction,
                 &m::recvFindReplacementNack>,

      // non-critical connected send transitions
      mem_fn_row<kConnected, SendMessage, kConnected, &m::validAction,
                 &m::sendNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedAcceptingChild, SendMessage, kConnectedAcceptingChild, &m::validAction,
                 &m::sendNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedReplacing, SendMessage, kConnectedReplacing, &m::validAction,
                 &m::sendNonCriticalMsgInConnectedState>,
      mem_fn_row<kWaitForReplacementOffer, SendMessage, kWaitForReplacementOffer, &m::validAction,
                 &m::sendNonCriticalMsgInConnectedState>,

      // TODO: ONLY FOR FORWARDING. NOT INITIATE-SELF-DEPARTURE
      mem_fn_row<kConnectedWaitingParentResponse, SendMessage, kConnectedWaitingParentResponse,
                 &m::validAction, &m::sendNonCriticalMsgInConnectedState>,
      mem_fn_row<kSignOffFromInlevelNeighbors, SendMessage, kSignOffFromInlevelNeighbors,
                 &m::validAction, &m::sendNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedWaitingParentResponseDirectLeaveWoReplacement, SendMessage,
                 kConnectedWaitingParentResponseDirectLeaveWoReplacement, &m::validAction,
                 &m::sendNonCriticalMsgInConnectedState>,
      mem_fn_row<kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, SendMessage,
                 kSignOffFromInlevelNeighborsDirectLeaveWoReplacement, &m::validAction,
                 &m::sendNonCriticalMsgInConnectedState>,

      // non-critical connected timeout transitions
      mem_fn_row<kConnected, Timeout, kConnected, &m::validAction,
                 &m::timeoutNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedAcceptingChild, Timeout, kConnectedAcceptingChild, &m::validAction,
                 &m::timeoutNonCriticalMsgInConnectedState>,
      mem_fn_row<kConnectedReplacing, Timeout, kConnectedReplacing, &m::validAction,
                 &m::timeoutNonCriticalMsgInConnectedState>,
      mem_fn_row<kWaitForReplacementOffer, Timeout, kWaitForReplacementOffer, &m::validAction,
                 &m::timeoutNonCriticalMsgInConnectedState>,

      // bootstrap discover must be allowed to be received in any state
      // but only answered in connected
      mem_fn_row<kConnected, SendMessage, kConnected, &m::validAction,
                 &m::sendBootstrapResponseMessage>,

      mem_fn_row<kIdle, ReceiveMessage, kIdle, &m::validAction, &m::recvBootstrapDiscoverMessage>,
      mem_fn_row<kIdle, ReceiveMessage, kIdle, &m::validAction, &m::recvFindReplacement>,
      mem_fn_row<kWaitForBootstrapResponse, ReceiveMessage, kWaitForBootstrapResponse,
                 &m::validAction, &m::recvBootstrapDiscoverMessage>,
      mem_fn_row<kWaitForJoinAccept, ReceiveMessage, kWaitForJoinAccept, &m::validAction,
                 &m::recvBootstrapDiscoverMessage>,
      mem_fn_row<kConnected, ReceiveMessage, kConnected, &m::validAction,
                 &m::recvBootstrapDiscoverMessage>,
      mem_fn_row<kConnectedAcceptingChild, ReceiveMessage, kConnectedAcceptingChild,
                 &m::validAction, &m::recvBootstrapDiscoverMessage>,
      mem_fn_row<kConnectedReplacing, ReceiveMessage, kConnectedReplacing, &m::validAction,
                 &m::recvBootstrapDiscoverMessage>,
      mem_fn_row<kWaitForReplacementOffer, ReceiveMessage, kWaitForReplacementOffer,
                 &m::validAction, &m::recvBootstrapDiscoverMessage>,
      mem_fn_row<kErrorState, ReceiveMessage, kErrorState, &m::validAction,
                 &m::recvBootstrapDiscoverMessage>,

      // multicast is unreliable, therefore we could also receive
      // unwanted bootstrap response messages in any state
      mem_fn_row<kIdle, ReceiveMessage, kIdle, &m::validAction, &m::recvBootstrapResponseMessage>,
      mem_fn_row<kWaitForBootstrapResponse, ReceiveMessage, kWaitForBootstrapResponse,
                 &m::validAction, &m::recvBootstrapResponseMessage>,
      mem_fn_row<kWaitForJoinAccept, ReceiveMessage, kWaitForJoinAccept, &m::validAction,
                 &m::recvBootstrapResponseMessage>,
      mem_fn_row<kConnected, ReceiveMessage, kConnected, &m::validAction,
                 &m::recvBootstrapResponseMessage>,
      mem_fn_row<kConnectedAcceptingChild, ReceiveMessage, kConnectedAcceptingChild,
                 &m::validAction, &m::recvBootstrapResponseMessage>,
      mem_fn_row<kConnectedReplacing, ReceiveMessage, kConnectedReplacing, &m::validAction,
                 &m::recvBootstrapResponseMessage>,
      mem_fn_row<kWaitForReplacementOffer, ReceiveMessage, kWaitForReplacementOffer,
                 &m::validAction, &m::recvBootstrapResponseMessage>,
      mem_fn_row<kErrorState, ReceiveMessage, kErrorState, &m::validAction,
                 &m::recvBootstrapResponseMessage>

      >;
};

}  // namespace minhton

#endif
