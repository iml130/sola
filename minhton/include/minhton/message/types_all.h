// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_TYPES_ALL_H_
#define MINHTON_MESSAGE_TYPES_ALL_H_

#include <variant>

#include "minhton/message/attribute_inquiry_answer.h"
#include "minhton/message/attribute_inquiry_request.h"
#include "minhton/message/bootstrap_discover.h"
#include "minhton/message/bootstrap_response.h"
#include "minhton/message/empty.h"
#include "minhton/message/find_query_answer.h"
#include "minhton/message/find_query_request.h"
#include "minhton/message/find_replacement.h"
#include "minhton/message/get_neighbors.h"
#include "minhton/message/inform_about_neighbors.h"
#include "minhton/message/join.h"
#include "minhton/message/join_accept.h"
#include "minhton/message/join_accept_ack.h"
#include "minhton/message/lock_neighbor_request.h"
#include "minhton/message/lock_neighbor_response.h"
#include "minhton/message/remove_and_update_neighbor.h"
#include "minhton/message/remove_neighbor.h"
#include "minhton/message/remove_neighbor_ack.h"
#include "minhton/message/replacement_ack.h"
#include "minhton/message/replacement_nack.h"
#include "minhton/message/replacement_offer.h"
#include "minhton/message/replacement_update.h"
#include "minhton/message/search_exact.h"
#include "minhton/message/search_exact_failure.h"
#include "minhton/message/signoff_parent_answer.h"
#include "minhton/message/signoff_parent_request.h"
#include "minhton/message/subscription_order.h"
#include "minhton/message/subscription_update.h"
#include "minhton/message/unlock_neighbor.h"
#include "minhton/message/update_neighbors.h"

namespace minhton {
using MessageVariant =
    std::variant<MessageAttributeInquiryAnswer, MessageAttributeInquiryRequest,
                 MessageBootstrapDiscover, MessageBootstrapResponse, MessageEmpty,
                 MessageFindQueryAnswer, MessageFindQueryRequest, MessageFindReplacement,
                 MessageGetNeighbors, MessageInformAboutNeighbors, MessageJoin, MessageJoinAccept,
                 MessageJoinAcceptAck, MessageLockNeighborRequest, MessageLockNeighborResponse,
                 MessageRemoveAndUpdateNeighbors, MessageRemoveNeighbor, MessageRemoveNeighborAck,
                 MessageReplacementAck, MessageReplacementNack, MessageReplacementOffer,
                 MessageReplacementUpdate, MessageSearchExact, MessageSearchExactFailure,
                 MessageSignoffParentAnswer, MessageSignoffParentRequest, MessageSubscriptionOrder,
                 MessageSubscriptionUpdate, MessageUnlockNeighbor, MessageUpdateNeighbors>;

/// Definition of a helper struct used for visiting variant types
template <class... Ts> struct Overload : Ts... {
#if 0
#endif
  using Ts::operator()...;
};

// TODO: Remove when using C++20
/// Explicit deduction guide for the Overload template
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;
}  // namespace minhton

#endif
