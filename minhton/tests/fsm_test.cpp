// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "utils/fsm.h"

#include <catch2/catch_test_macros.hpp>

using namespace minhton;

using fsm = FiniteStateMachine;

TEST_CASE("FSM Constructor", "[FiniteStateMachine][Init]") {
  fsm m1;
  REQUIRE(m1.current_state() == FSMState::kIdle);

  // initialization of root
  fsm m2(FSMState::kConnected);
  REQUIRE(m2.current_state() == FSMState::kConnected);
}

TEST_CASE("FSM Idle to Connected Paths", "[FiniteStateMachine][IdleToConnected]") {
  SECTION("Join via Address, both sides") {
    fsm entering_node;
    REQUIRE(entering_node.current_state() == FSMState::kIdle);

    fsm network_node(FSMState::kConnected);
    REQUIRE(network_node.current_state() == FSMState::kConnected);

    // entering node gets join network signal, via address
    entering_node.process_event(Signal{SignalType::kJoinNetwork, false, false, ""});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForJoinAccept);
    REQUIRE(entering_node.isActionValid());

    // entering node sends join to network node
    // network node receives join from entering node
    entering_node.process_event(SendMessage{MessageType::kJoin});
    network_node.process_event(ReceiveMessage{MessageType::kJoin});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForJoinAccept);
    REQUIRE(network_node.current_state() == FSMState::kConnected);
    REQUIRE(entering_node.isActionValid());
    REQUIRE(network_node.isActionValid());

    // network node sends join accept to entering node
    // entering node receives join accept
    entering_node.process_event(ReceiveMessage{MessageType::kJoinAccept});
    network_node.process_event(SendMessage{MessageType::kJoinAccept});
    REQUIRE(entering_node.current_state() == FSMState::kConnected);
    REQUIRE(network_node.current_state() == FSMState::kConnectedAcceptingChild);
    REQUIRE(entering_node.isActionValid());
    REQUIRE(network_node.isActionValid());

    // directly after receiving join accept, the entering node sends a join accept ack to the
    // network node network node receives join accept ack
    entering_node.process_event(SendMessage{MessageType::kJoinAcceptAck});
    network_node.process_event(ReceiveMessage{MessageType::kJoinAcceptAck});
    REQUIRE(entering_node.current_state() == FSMState::kConnected);
    REQUIRE(network_node.current_state() == FSMState::kConnected);
    REQUIRE(entering_node.isActionValid());
    REQUIRE(network_node.isActionValid());
  }

  SECTION("Join via Bootstrap, both sides, valid response") {
    fsm entering_node;
    REQUIRE(entering_node.current_state() == FSMState::kIdle);

    fsm network_node1(FSMState::kConnected);
    REQUIRE(network_node1.current_state() == FSMState::kConnected);

    fsm network_node2(FSMState::kConnectedAcceptingChild);
    REQUIRE(network_node2.current_state() == FSMState::kConnectedAcceptingChild);

    fsm network_node3(FSMState::kConnectedReplacing);
    REQUIRE(network_node3.current_state() == FSMState::kConnectedReplacing);

    // entering node gets join network signal, via address
    entering_node.process_event(Signal{SignalType::kJoinNetwork, false, true, ""});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForBootstrapResponse);
    REQUIRE(entering_node.isActionValid());

    // entering node sends bootstrap discover
    // and in the application, the entering node needs to set a bootstrap response Timeout

    // network node receives bootstrap discover
    // both stay in their current state
    entering_node.process_event(SendMessage{MessageType::kBootstrapDiscover});
    network_node1.process_event(ReceiveMessage{MessageType::kBootstrapDiscover});
    network_node2.process_event(ReceiveMessage{MessageType::kBootstrapDiscover});
    network_node3.process_event(ReceiveMessage{MessageType::kBootstrapDiscover});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForBootstrapResponse);
    REQUIRE(network_node1.current_state() == FSMState::kConnected);
    REQUIRE(network_node2.current_state() == FSMState::kConnectedAcceptingChild);
    REQUIRE(network_node3.current_state() == FSMState::kConnectedReplacing);
    REQUIRE(entering_node.isActionValid());
    REQUIRE(network_node1.isActionValid());
    REQUIRE(network_node2.isActionValid());
    REQUIRE(network_node3.isActionValid());

    // and the other way around
    entering_node.process_event(ReceiveMessage{MessageType::kBootstrapResponse});
    network_node1.process_event(SendMessage{MessageType::kBootstrapResponse});
    network_node2.process_event(SendMessage{MessageType::kBootstrapResponse});
    network_node3.process_event(SendMessage{MessageType::kBootstrapResponse});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForBootstrapResponse);
    REQUIRE(network_node1.current_state() == FSMState::kConnected);
    REQUIRE(network_node2.current_state() == FSMState::kConnectedAcceptingChild);
    REQUIRE(network_node3.current_state() == FSMState::kConnectedReplacing);
    REQUIRE(entering_node.isActionValid());
    REQUIRE(network_node1.isActionValid());
    REQUIRE_FALSE(network_node2.isActionValid());
    REQUIRE_FALSE(network_node3.isActionValid());

    // entering node gets the bootstrap response Timeout triggered, with valid response
    entering_node.process_event(Timeout{TimeoutType::kBootstrapResponseTimeout, true});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForJoinAccept);
    REQUIRE(entering_node.isActionValid());

    // after that everything should be able to work the same way
    // as in the join via address case

    // entering node sends join to network node
    // network node receives join from entering node
    entering_node.process_event(SendMessage{MessageType::kJoin});
    network_node1.process_event(ReceiveMessage{MessageType::kJoin});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForJoinAccept);
    REQUIRE(network_node1.current_state() == FSMState::kConnected);
    REQUIRE(entering_node.isActionValid());
    REQUIRE(network_node1.isActionValid());

    // network node sends join accept to entering node
    // entering node receives join accept
    entering_node.process_event(ReceiveMessage{MessageType::kJoinAccept});
    network_node1.process_event(SendMessage{MessageType::kJoinAccept});
    REQUIRE(entering_node.current_state() == FSMState::kConnected);
    REQUIRE(network_node1.current_state() == FSMState::kConnectedAcceptingChild);
    REQUIRE(entering_node.isActionValid());
    REQUIRE(network_node1.isActionValid());

    // directly after receiving join accept, the entering node sends a join accept ack to the
    // network node network node receives join accept ack
    entering_node.process_event(SendMessage{MessageType::kJoinAcceptAck});
    network_node1.process_event(ReceiveMessage{MessageType::kJoinAcceptAck});
    REQUIRE(entering_node.current_state() == FSMState::kConnected);
    REQUIRE(network_node1.current_state() == FSMState::kConnected);
    REQUIRE(entering_node.isActionValid());
    REQUIRE(network_node1.isActionValid());
  }

  SECTION("Join via Address, JoinResponseTimeout triggered") {
    fsm entering_node;
    REQUIRE(entering_node.current_state() == FSMState::kIdle);

    entering_node.process_event(Signal{SignalType::kJoinNetwork, false, false, ""});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForJoinAccept);
    REQUIRE(entering_node.isActionValid());

    entering_node.process_event(SendMessage{MessageType::kJoin});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForJoinAccept);
    REQUIRE(entering_node.isActionValid());

    // entering node sets a join response Timeout, which then gets expired

    entering_node.process_event(Timeout{TimeoutType::kJoinAcceptResponseTimeout});
    REQUIRE(entering_node.current_state() == FSMState::kJoinFailed);
    REQUIRE(entering_node.isActionValid());
  }

  SECTION("Join via Bootstrap, invalid response") {
    fsm entering_node;
    REQUIRE(entering_node.current_state() == FSMState::kIdle);

    // entering node gets join network signal, via address
    entering_node.process_event(Signal{SignalType::kJoinNetwork, false, true, ""});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForBootstrapResponse);
    REQUIRE(entering_node.isActionValid());

    entering_node.process_event(SendMessage{MessageType::kBootstrapDiscover});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForBootstrapResponse);
    REQUIRE(entering_node.isActionValid());

    // and the other way around
    entering_node.process_event(ReceiveMessage{MessageType::kBootstrapResponse});
    REQUIRE(entering_node.current_state() == FSMState::kWaitForBootstrapResponse);
    REQUIRE(entering_node.isActionValid());

    // entering node gets the bootstrap response Timeout triggered, with valid response
    entering_node.process_event(Timeout{TimeoutType::kBootstrapResponseTimeout, false});
    REQUIRE(entering_node.current_state() == FSMState::kErrorState);
    REQUIRE(entering_node.isActionValid());
  }
}

TEST_CASE("FSM Connected Accepting", "[FiniteStateMachine][Accepting]") {
  SECTION("JoinAcceptAck Response Timeout") {
    fsm network_node(FSMState::kConnected);
    REQUIRE(network_node.current_state() == FSMState::kConnected);

    // receiving join
    network_node.process_event(ReceiveMessage{MessageType::kJoin});
    REQUIRE(network_node.current_state() == FSMState::kConnected);
    REQUIRE(network_node.isActionValid());

    // and then sending join accept back
    network_node.process_event(SendMessage{MessageType::kJoinAccept});
    REQUIRE(network_node.current_state() == FSMState::kConnectedAcceptingChild);
    REQUIRE(network_node.isActionValid());

    // in application setting a join accept ack response Timeout
    // which then might expire
    // and get back into connected state
    network_node.process_event(Timeout{TimeoutType::kJoinAcceptAckResponseTimeout});
    REQUIRE(network_node.current_state() == FSMState::kConnected);
    REQUIRE(network_node.isActionValid());
  }
}

TEST_CASE("FSM Connected Replacing", "[FiniteStateMachine][Replacement]") {
  SECTION("One sided, can leave position") {
    fsm network_node(FSMState::kConnected);
    REQUIRE(network_node.current_state() == FSMState::kConnected);

    // processing leave signal, with can leave position flag
    network_node.process_event(Signal{SignalType::kLeaveNetwork, true, false, ""});
    REQUIRE(network_node.current_state() ==
            FSMState::kConnectedWaitingParentResponseDirectLeaveWoReplacement);
    REQUIRE(network_node.isActionValid());

    // after this transition we still must be able to send all necessary update packages
    // like update neighbor, remove neighbor, search exact
    network_node.process_event(SendMessage{MessageType::kUpdateNeighbors});
    REQUIRE(network_node.current_state() ==
            FSMState::kConnectedWaitingParentResponseDirectLeaveWoReplacement);
    REQUIRE(network_node.isActionValid());
    network_node.process_event(SendMessage{MessageType::kRemoveNeighbor});
    REQUIRE(network_node.current_state() ==
            FSMState::kConnectedWaitingParentResponseDirectLeaveWoReplacement);
    REQUIRE(network_node.isActionValid());
    network_node.process_event(SendMessage{MessageType::kSearchExact});
    REQUIRE(network_node.current_state() ==
            FSMState::kConnectedWaitingParentResponseDirectLeaveWoReplacement);
    REQUIRE(network_node.isActionValid());

    // TODO Extend with parent response
  }

  SECTION("Both sided, replacing node, and node which cannot leave position") {
    fsm leaving_node(FSMState::kConnected);
    fsm replacing_node(FSMState::kConnected);

    REQUIRE(leaving_node.current_state() == FSMState::kConnected);
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);

    // processing leave signal, with cannot leave position flag
    leaving_node.process_event(Signal{SignalType::kLeaveNetwork, false, false, ""});
    REQUIRE(leaving_node.current_state() == FSMState::kWaitForReplacementOffer);
    REQUIRE(leaving_node.isActionValid());

    // leaving node then sends find replacement message
    leaving_node.process_event(SendMessage{MessageType::kFindReplacement});
    REQUIRE(leaving_node.current_state() == FSMState::kWaitForReplacementOffer);
    REQUIRE(leaving_node.isActionValid());

    // which the replacing node then receives
    replacing_node.process_event(ReceiveMessage{MessageType::kFindReplacement});
    replacing_node.process_event(SendMessage{MessageType::kSignOffParentRequest});
    REQUIRE(replacing_node.current_state() == FSMState::kConnectedWaitingParentResponse);
    REQUIRE(replacing_node.isActionValid());

    replacing_node.process_event(ReceiveMessage{MessageType::kSignOffParentAnswer});
    REQUIRE(replacing_node.current_state() == FSMState::kSignOffFromInlevelNeighbors);
    REQUIRE(replacing_node.isActionValid());

    // replacing node sends replacement offer
    replacing_node.process_event(SendMessage{MessageType::kReplacementOffer});
    REQUIRE(replacing_node.current_state() == FSMState::kConnectedReplacing);
    REQUIRE(replacing_node.isActionValid());

    // which the leaving node receives
    leaving_node.process_event(ReceiveMessage{MessageType::kReplacementOffer});
    REQUIRE(leaving_node.current_state() == FSMState::kIdle);
    REQUIRE(leaving_node.isActionValid());

    // then in the idle state, the leaving node still must be able to send the replacement ack back
    leaving_node.process_event(SendMessage{MessageType::kReplacementAck});
    REQUIRE(leaving_node.current_state() == FSMState::kIdle);
    REQUIRE(leaving_node.isActionValid());

    // replacing node receives replacement ack
    replacing_node.process_event(ReceiveMessage{MessageType::kReplacementAck});
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);
    REQUIRE(replacing_node.isActionValid());

    // and afterwards sends a bunch of update messages in the network
    replacing_node.process_event(SendMessage{MessageType::kUpdateNeighbors});
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);
    REQUIRE(replacing_node.isActionValid());
    replacing_node.process_event(SendMessage{MessageType::kRemoveNeighbor});
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);
    REQUIRE(replacing_node.isActionValid());
    replacing_node.process_event(SendMessage{MessageType::kReplacementUpdate});
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);
    REQUIRE(replacing_node.isActionValid());
    replacing_node.process_event(SendMessage{MessageType::kSearchExact});
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);
    REQUIRE(replacing_node.isActionValid());
  }

  SECTION("One sided, replacing node replacement ack response Timeout expires") {
    fsm replacing_node(FSMState::kConnected);
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);

    // replacing node receives find replacement
    replacing_node.process_event(ReceiveMessage{MessageType::kFindReplacement});
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);
    REQUIRE(replacing_node.isActionValid());

    // replacing node sends replacement offer
    replacing_node.process_event(SendMessage{MessageType::kSignOffParentRequest});
    replacing_node.process_event(ReceiveMessage{MessageType::kSignOffParentAnswer});
    replacing_node.process_event(SendMessage{MessageType::kReplacementOffer});
    REQUIRE(replacing_node.current_state() == FSMState::kConnectedReplacing);
    REQUIRE(replacing_node.isActionValid());

    // and application sets replacement ack response Timeout
    // which expires
    replacing_node.process_event(Timeout{TimeoutType::kReplacementAckResponseTimeout});
    REQUIRE(replacing_node.current_state() == FSMState::kConnected);
    REQUIRE(replacing_node.isActionValid());
  }

  SECTION("One sided, leaving node replacement offer response Timeout expires") {
    fsm leaving_node(FSMState::kConnected);
    REQUIRE(leaving_node.current_state() == FSMState::kConnected);

    // processing leave signal, with cannot leave position flag
    leaving_node.process_event(Signal{SignalType::kLeaveNetwork, false, false, ""});
    REQUIRE(leaving_node.current_state() == FSMState::kWaitForReplacementOffer);
    REQUIRE(leaving_node.isActionValid());

    // leaving node then sends find replacement message
    leaving_node.process_event(SendMessage{MessageType::kFindReplacement});
    REQUIRE(leaving_node.current_state() == FSMState::kWaitForReplacementOffer);
    REQUIRE(leaving_node.isActionValid());

    // leaving node sets Timeout to receive a replacement offer
    // which then expires
    leaving_node.process_event(Timeout{TimeoutType::kReplacementOfferResponseTimeout});
    REQUIRE(leaving_node.current_state() == FSMState::kConnected);
    REQUIRE(leaving_node.isActionValid());
  }
}

TEST_CASE("FSM Strictly Forbidden Transitions", "[FiniteStateMachine][StrictlyForbidden]") {
  SECTION("Sending Bootstrap Response if not in Connected State") {
    // and not changing state

    std::vector<FSMState> all_non_conn_states = {FSMState::kIdle,
                                                 FSMState::kWaitForBootstrapResponse,
                                                 FSMState::kWaitForJoinAccept,
                                                 FSMState::kWaitForReplacementOffer,
                                                 FSMState::kConnectedReplacing,
                                                 FSMState::kConnectedAcceptingChild,
                                                 FSMState::kErrorState};

    for (auto const &state : all_non_conn_states) {
      fsm current(state);
      REQUIRE(current.current_state() == state);

      current.process_event(SendMessage{MessageType::kBootstrapResponse});
      REQUIRE(current.current_state() == state);
      REQUIRE_FALSE(current.isActionValid());
    }
  }

  SECTION("Timeouts in not-related FSMState") {
    std::vector<std::tuple<FSMState, TimeoutType>> wrong_combinations = {
        std::make_tuple(FSMState::kIdle, TimeoutType::kBootstrapResponseTimeout),
        std::make_tuple(FSMState::kIdle, TimeoutType::kJoinAcceptResponseTimeout),
        std::make_tuple(FSMState::kIdle, TimeoutType::kJoinAcceptAckResponseTimeout),
        std::make_tuple(FSMState::kIdle, TimeoutType::kReplacementAckResponseTimeout),
        std::make_tuple(FSMState::kIdle, TimeoutType::kReplacementOfferResponseTimeout),

        std::make_tuple(FSMState::kWaitForBootstrapResponse,
                        TimeoutType::kJoinAcceptResponseTimeout),
        std::make_tuple(FSMState::kWaitForBootstrapResponse,
                        TimeoutType::kJoinAcceptAckResponseTimeout),
        std::make_tuple(FSMState::kWaitForBootstrapResponse,
                        TimeoutType::kReplacementAckResponseTimeout),
        std::make_tuple(FSMState::kWaitForBootstrapResponse,
                        TimeoutType::kReplacementOfferResponseTimeout),

        std::make_tuple(FSMState::kWaitForJoinAccept, TimeoutType::kBootstrapResponseTimeout),
        std::make_tuple(FSMState::kWaitForJoinAccept, TimeoutType::kJoinAcceptAckResponseTimeout),
        std::make_tuple(FSMState::kWaitForJoinAccept, TimeoutType::kReplacementAckResponseTimeout),
        std::make_tuple(FSMState::kWaitForJoinAccept,
                        TimeoutType::kReplacementOfferResponseTimeout),

        std::make_tuple(FSMState::kConnected, TimeoutType::kBootstrapResponseTimeout),
        std::make_tuple(FSMState::kConnected, TimeoutType::kJoinAcceptResponseTimeout),
        std::make_tuple(FSMState::kConnected, TimeoutType::kJoinAcceptAckResponseTimeout),
        std::make_tuple(FSMState::kConnected, TimeoutType::kReplacementAckResponseTimeout),
        std::make_tuple(FSMState::kConnected, TimeoutType::kReplacementOfferResponseTimeout),

        std::make_tuple(FSMState::kConnectedAcceptingChild, TimeoutType::kBootstrapResponseTimeout),
        std::make_tuple(FSMState::kConnectedAcceptingChild,
                        TimeoutType::kJoinAcceptResponseTimeout),
        std::make_tuple(FSMState::kConnectedAcceptingChild,
                        TimeoutType::kReplacementAckResponseTimeout),
        std::make_tuple(FSMState::kConnectedAcceptingChild,
                        TimeoutType::kReplacementOfferResponseTimeout),

        std::make_tuple(FSMState::kConnectedReplacing, TimeoutType::kBootstrapResponseTimeout),
        std::make_tuple(FSMState::kConnectedReplacing, TimeoutType::kJoinAcceptResponseTimeout),
        std::make_tuple(FSMState::kConnectedReplacing, TimeoutType::kJoinAcceptAckResponseTimeout),
        std::make_tuple(FSMState::kConnectedReplacing,
                        TimeoutType::kReplacementOfferResponseTimeout),

        std::make_tuple(FSMState::kWaitForReplacementOffer, TimeoutType::kBootstrapResponseTimeout),
        std::make_tuple(FSMState::kWaitForReplacementOffer,
                        TimeoutType::kJoinAcceptResponseTimeout),
        std::make_tuple(FSMState::kWaitForReplacementOffer,
                        TimeoutType::kJoinAcceptAckResponseTimeout),
        std::make_tuple(FSMState::kWaitForReplacementOffer,
                        TimeoutType::kReplacementAckResponseTimeout)};

    for (auto const &combi : wrong_combinations) {
      FSMState state = std::get<0>(combi);
      TimeoutType timeout_type = std::get<1>(combi);

      fsm current(state);
      REQUIRE(current.current_state() == state);

      current.process_event(Timeout{timeout_type});
      REQUIRE(current.current_state() == state);
      REQUIRE_FALSE(current.isActionValid());
    }
  }

  SECTION("Signals in not-related FSMState") {
    std::vector<std::tuple<FSMState, SignalType>> wrong_combinations = {
        std::make_tuple(FSMState::kIdle, SignalType::kLeaveNetwork),

        std::make_tuple(FSMState::kWaitForBootstrapResponse, SignalType::kJoinNetwork),
        std::make_tuple(FSMState::kWaitForBootstrapResponse, SignalType::kLeaveNetwork),

        std::make_tuple(FSMState::kWaitForJoinAccept, SignalType::kJoinNetwork),
        std::make_tuple(FSMState::kWaitForJoinAccept, SignalType::kLeaveNetwork),

        std::make_tuple(FSMState::kErrorState, SignalType::kJoinNetwork),
        std::make_tuple(FSMState::kErrorState, SignalType::kLeaveNetwork),

        std::make_tuple(FSMState::kWaitForReplacementOffer, SignalType::kJoinNetwork),
        std::make_tuple(FSMState::kWaitForReplacementOffer, SignalType::kLeaveNetwork),

        std::make_tuple(FSMState::kConnected, SignalType::kJoinNetwork),

        std::make_tuple(FSMState::kConnectedAcceptingChild, SignalType::kJoinNetwork),
        std::make_tuple(FSMState::kConnectedAcceptingChild, SignalType::kLeaveNetwork),

        std::make_tuple(FSMState::kConnectedReplacing, SignalType::kJoinNetwork),
        std::make_tuple(FSMState::kConnectedReplacing, SignalType::kLeaveNetwork),
    };

    for (auto const &combi : wrong_combinations) {
      FSMState state = std::get<0>(combi);
      SignalType signal_type = std::get<1>(combi);

      fsm current(state);
      REQUIRE(current.current_state() == state);

      current.process_event(Signal{signal_type, false, false, ""});
      REQUIRE(current.current_state() == state);
      REQUIRE_FALSE(current.isActionValid());
    }
  }

  SECTION("Messages of Connected States in non-connected States") {
    std::vector<MessageType> connected_state_messages = {
        MessageType::kRemoveNeighbor,       MessageType::kUpdateNeighbors,
        MessageType::kReplacementUpdate,    MessageType::kFindReplacement,
        MessageType::kInformAboutNeighbors, MessageType::kGetNeighbors,
        MessageType::kSearchExact,          MessageType::kEmpty,
    };

    fsm node1(FSMState::kWaitForBootstrapResponse);
    REQUIRE(node1.current_state() == FSMState::kWaitForBootstrapResponse);
    fsm node2(FSMState::kWaitForJoinAccept);
    REQUIRE(node2.current_state() == FSMState::kWaitForJoinAccept);
    fsm node3(FSMState::kIdle);
    REQUIRE(node3.current_state() == FSMState::kIdle);

    for (auto const &msg_type : connected_state_messages) {
      // send
      node1.process_event(SendMessage{msg_type});
      REQUIRE(node1.current_state() == FSMState::kWaitForBootstrapResponse);
      REQUIRE_FALSE(node1.isActionValid());

      node2.process_event(SendMessage{msg_type});
      REQUIRE(node2.current_state() == FSMState::kWaitForJoinAccept);
      REQUIRE_FALSE(node2.isActionValid());

      // recv
      node1.process_event(ReceiveMessage{msg_type});
      REQUIRE(node1.current_state() == FSMState::kWaitForBootstrapResponse);
      REQUIRE_FALSE(node1.isActionValid());

      node2.process_event(ReceiveMessage{msg_type});
      REQUIRE(node2.current_state() == FSMState::kWaitForJoinAccept);
      REQUIRE_FALSE(node2.isActionValid());
    }

    node1.process_event(SendMessage{MessageType::kJoin});
    REQUIRE(node1.current_state() == FSMState::kWaitForBootstrapResponse);
    REQUIRE_FALSE(node1.isActionValid());
    node1.process_event(ReceiveMessage{MessageType::kJoin});
    REQUIRE(node1.current_state() == FSMState::kWaitForBootstrapResponse);
    REQUIRE_FALSE(node1.isActionValid());

    node3.process_event(SendMessage{MessageType::kJoin});
    REQUIRE(node3.current_state() == FSMState::kIdle);
    REQUIRE_FALSE(node3.isActionValid());
    node3.process_event(ReceiveMessage{MessageType::kJoin});
    REQUIRE(node3.current_state() == FSMState::kIdle);
    REQUIRE_FALSE(node3.isActionValid());

    node3.process_event(SendMessage{MessageType::kReplacementUpdate});
    REQUIRE(node3.current_state() == FSMState::kIdle);
    REQUIRE_FALSE(node3.isActionValid());
    node3.process_event(ReceiveMessage{MessageType::kReplacementUpdate});
    REQUIRE(node3.current_state() == FSMState::kIdle);
    REQUIRE_FALSE(node3.isActionValid());

    node3.process_event(SendMessage{MessageType::kInformAboutNeighbors});
    REQUIRE(node3.current_state() == FSMState::kIdle);
    REQUIRE_FALSE(node3.isActionValid());
    node3.process_event(ReceiveMessage{MessageType::kInformAboutNeighbors});
    REQUIRE(node3.current_state() == FSMState::kIdle);
    REQUIRE_FALSE(node3.isActionValid());

    node3.process_event(SendMessage{MessageType::kGetNeighbors});
    REQUIRE(node3.current_state() == FSMState::kIdle);
    REQUIRE_FALSE(node3.isActionValid());
    node3.process_event(ReceiveMessage{MessageType::kGetNeighbors});
    REQUIRE(node3.current_state() == FSMState::kIdle);
    REQUIRE_FALSE(node3.isActionValid());
  }
}

TEST_CASE("FSM Definitely Allowed Transitions", "[FiniteStateMachine][DefinitelyAllowed]") {
  SECTION("Receiving Bootstrap Discover if any State") {
    // and not changing state

    std::vector<FSMState> all_states = {FSMState::kIdle,
                                        FSMState::kWaitForBootstrapResponse,
                                        FSMState::kWaitForJoinAccept,
                                        FSMState::kWaitForReplacementOffer,
                                        FSMState::kConnected,
                                        FSMState::kConnectedReplacing,
                                        FSMState::kConnectedAcceptingChild,
                                        FSMState::kErrorState};

    for (auto const &state : all_states) {
      fsm current(state);
      REQUIRE(current.current_state() == state);

      current.process_event(ReceiveMessage{MessageType::kBootstrapDiscover});
      REQUIRE(current.current_state() == state);
      REQUIRE(current.isActionValid());
    }
  }

  SECTION("Non Critical Messages in Connected States") {
    std::vector<MessageType> non_critical_messages = {
        MessageType::kJoin,
        MessageType::kRemoveNeighbor,
        MessageType::kUpdateNeighbors,
        MessageType::kReplacementUpdate,
        MessageType::kFindReplacement,
        MessageType::kInformAboutNeighbors,
        MessageType::kGetNeighbors,
        MessageType::kSearchExact,
        MessageType::kEmpty,
        MessageType::kFindQueryAnswer,
        MessageType::kFindQueryRequest,
        MessageType::kAttributeInquiryAnswer,
        MessageType::kAttributeInquiryRequest,
        MessageType::kSubscriptionOrder,
        MessageType::kSubscriptionUpdate,
    };

    fsm node1(FSMState::kConnected);
    REQUIRE(node1.current_state() == FSMState::kConnected);
    fsm node2(FSMState::kConnectedAcceptingChild);
    REQUIRE(node2.current_state() == FSMState::kConnectedAcceptingChild);
    fsm node3(FSMState::kConnectedReplacing);
    REQUIRE(node3.current_state() == FSMState::kConnectedReplacing);
    fsm node4(FSMState::kWaitForReplacementOffer);
    REQUIRE(node4.current_state() == FSMState::kWaitForReplacementOffer);

    // in all connected FSMState
    // = connected, connected accepting child, connected replacing, and waiting for repl offer
    // all non critical messages must be able to be processed (received and sent)
    // not not cause a state transition
    for (auto const &msg_type : non_critical_messages) {
      // sending
      node1.process_event(SendMessage{msg_type});
      REQUIRE(node1.current_state() == FSMState::kConnected);
      REQUIRE(node1.isActionValid());

      node2.process_event(SendMessage{msg_type});
      REQUIRE(node2.current_state() == FSMState::kConnectedAcceptingChild);
      REQUIRE(node2.isActionValid());

      node3.process_event(SendMessage{msg_type});
      REQUIRE(node3.current_state() == FSMState::kConnectedReplacing);
      REQUIRE(node3.isActionValid());

      node4.process_event(SendMessage{msg_type});
      REQUIRE(node4.current_state() == FSMState::kWaitForReplacementOffer);
      REQUIRE(node4.isActionValid());

      // receiving
      node1.process_event(ReceiveMessage{msg_type});
      REQUIRE(node1.current_state() == FSMState::kConnected);
      REQUIRE(node1.isActionValid());

      node2.process_event(ReceiveMessage{msg_type});
      REQUIRE(node2.current_state() == FSMState::kConnectedAcceptingChild);
      REQUIRE(node2.isActionValid());

      node3.process_event(ReceiveMessage{msg_type});
      REQUIRE(node3.current_state() == FSMState::kConnectedReplacing);
      REQUIRE(node3.isActionValid());

      node4.process_event(ReceiveMessage{msg_type});
      REQUIRE(node4.current_state() == FSMState::kWaitForReplacementOffer);
      REQUIRE(node4.isActionValid());
    }
  }
}
