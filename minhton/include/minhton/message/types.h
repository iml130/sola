// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_TYPES_H_
#define MINHTON_MESSAGE_TYPES_H_

#include <cstdint>

namespace minhton {
///
/// there are various types of the messages for MinhtonNode. For further details for about the
/// messages, check the corresponding message headers
///
enum class MessageType : uint32_t {
  // General
  kInit = 0,

  // Join Procedure
  kJoin = 10,
  kJoinAccept = 12,
  kJoinAcceptAck = 14,

  // Entity Search
  kFindQueryRequest = 20,
  kFindQueryAnswer = 22,
  kAttributeInquiryRequest = 24,
  kAttributeInquiryAnswer = 26,
  kSubscriptionOrder = 28,
  kSubscriptionUpdate = 30,

  // Search Exact
  kSearchExact = 40,
  kSearchExactFailure = 41,
  kEmpty = 42,

  // Bootstrap Algorithm
  kBootstrapDiscover = 50,
  kBootstrapResponse = 52,

  // Updates & Response Algorithm
  kRemoveNeighbor = 60,
  kRemoveNeighborAck = 62,
  kUpdateNeighbors = 64,
  kReplacementUpdate = 66,
  kGetNeighbors = 70,
  kInformAboutNeighbors = 72,

  // Leave Procedure
  kFindReplacement = 80,
  kReplacementNack = 81,
  kSignOffParentRequest = 82,
  kLockNeighborRequest = 84,
  kLockNeighborResponse = 86,
  kSignOffParentAnswer = 88,
  kRemoveAndUpdateNeighbor = 90,
  kReplacementOffer = 92,
  kReplacementAck = 94,
  kUnlockNeighbor = 96,
};

}  // namespace minhton
#endif
