// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_CONSTANTS_H_
#define MINHTON_CORE_CONSTANTS_H_

#include <cstdint>
#include <string>
#include <unordered_map>

///
/// Global definition of constants that are used in many different files
///

namespace minhton {

const uint32_t kTreeMapperRootValue = 100;

const uint16_t kFanoutMinimum = 2;
const uint16_t kFanoutMaximum = 255;
const uint16_t kFanoutDefault = 2;

/// Bootstrap UDP Multicast Settings
const int kBootstrapRepeats = 1;

/// UDP Multicast Bootstrap Port
const int kMulticastPort = 11999;

/// UDP Multicast Bootstrap Address
const std::string kMulticastAddress = "224.1.1.1";

const uint16_t kDefaultIpPort = 2000;

// timeouts in ms
const uint16_t kDefaultTimeoutLength = 2500;

// for logging
enum class SearchExactTestEntryTypes : uint8_t { kStart = 0, kHop = 1, kSuccess = 2, kFailure = 3 };

// for logging
enum class MessageProcessingModes : uint8_t { kReceiving = 0, kSending = 1 };

// for logging
enum class NeighborRelationship : uint8_t {
  kParent = 0,
  kChild = 1,
  kAdjacentLeft = 2,
  kAdjacentRight = 3,
  kRoutingTableNeighbor = 4,
  kRoutingTableNeighborChild = 5,
  kUnknownRelationship = 6,
};

// for logging
enum class EventType : uint8_t {
  kJoinEvent = 0,
  kLeaveEvent = 1,
  kFindQueryEvent = 3,
  kRequestCountdownStart = 4,
};

// for logging
enum class ContentStatus : uint8_t {
  kContentInsertUpdate = 0,
  kContentRemove = 1,
};

// for fsm
enum class SignalType : uint8_t {
  kJoinNetwork = 0,
  kLeaveNetwork = 1,
  kCleanupSignal = 2,
};

// for fsm
enum class TimeoutType : uint8_t {
  kBootstrapResponseTimeout = 0,
  kJoinAcceptResponseTimeout = 1,
  kJoinAcceptAckResponseTimeout = 2,
  kReplacementAckResponseTimeout = 3,
  kReplacementOfferResponseTimeout = 4,
  kDsnAggregationTimeout = 5,
  kInquiryAggregationTimeout = 6,
  kSelfDepartureRetry = 7,
  kJoinRetry = 8,
};

// for fsm
enum FSMState : int {
  kIdle,
  kWaitForBootstrapResponse,
  kWaitForJoinAccept,
  kConnected,
  kConnectedAcceptingChild,
  kConnectedReplacing,
  kWaitForReplacementOffer,
  kErrorState,
  kConnectedWaitingParentResponse,
  kSignOffFromInlevelNeighbors,
  kConnectedWaitingParentResponseDirectLeaveWoReplacement,
  kSignOffFromInlevelNeighborsDirectLeaveWoReplacement,
  kJoinFailed,
};

enum class JoinAlgorithms {
  kJoinMinhton,
};

enum class LeaveAlgorithms {
  kLeaveMinhton,
};

enum class SearchExactAlgorithms {
  kSearchExactMinhton,
};

enum class ResponseAlgorithms {
  kResponseGeneral,
};

enum class BootstrapAlgorithms {
  kBootstrapGeneral,
};
}  // namespace minhton
#endif
