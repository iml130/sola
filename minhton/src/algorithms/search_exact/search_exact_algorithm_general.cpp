// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/search_exact/search_exact_algorithm_general.h"

#include <cmath>

#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"

namespace minhton {

void SearchExactAlgorithmGeneral::process(const MessageVariant &msg) {
  std::visit(Overload{[this](const MessageSearchExact &message) { processSearchExact(message); },
                      [](auto &message) {
                        throw AlgorithmException("Wrong Algorithm Interface process called");
                      }},
             msg);
}

void SearchExactAlgorithmGeneral::processSearchExact(const MessageSearchExact &msg) {
  NodeInfo destination_node = msg.getDestinationNode();
  this->performSearchExact(destination_node, msg.getQuery());
}

void SearchExactAlgorithmGeneral::performSearchExact(const NodeInfo &destination,
                                                     std::shared_ptr<MessageSEVariant> query) {
  if (!destination.isValidPeer()) {
    throw std::invalid_argument("Logical Address of Destination Node has to be initialized!");
  }

  if (!isFanoutValid(destination.getFanout())) {
    throw std::invalid_argument("The Fanout of the destination node has to be set!");
  }

  double self_value = getSelfNodeInfo().getPeerInfo().getHorizontalValue();
  double dest_value = destination.getPeerInfo().getHorizontalValue();
  MinhtonMessageHeader query_header =
      std::visit([](auto &&msg) -> MinhtonMessageHeader { return msg.getHeader(); }, *query);

  // we are the searched node
  if (self_value == dest_value) {
    if (query_header.getMessageType() == MessageType::kEmpty) {
      LOG_SEARCH_EXACT(SearchExactTestEntryTypes::kSuccess, query_header.getEventId(),
                       query_header.getSender(), destination,
                       getSelfNodeInfo());  // success
      return;
    }

    // never processing an message empty
    MessageVariant query_var = std::visit(
        [this](auto &&msg) -> MessageVariant {
          msg.setTarget(getSelfNodeInfo());
          return msg;
        },
        *query);
    this->access_->recv(query_var);
    return;
  }

  auto closest_node = calcClosestRedirect(dest_value);

  // Special Case We are the closest Node, where it would be redirected next.
  if (closest_node.getPeerInfo() == getSelfNodeInfo().getPeerInfo()) {
    if (query_header.getMessageType() == MessageType::kEmpty) {
      LOG_SEARCH_EXACT(SearchExactTestEntryTypes::kFailure, query_header.getEventId(),
                       query_header.getSender(), destination,
                       getSelfNodeInfo());  // fail
    }
    LOG_INFO("Searched Node " + destination.getString() + " does not exist");
    notifyAboutFailure(destination, query);
    return;
  }

  // forward the actual message to the next possible node
  LOG_INFO("Forwarding SRCH to " + closest_node.getString());
  if (query_header.getMessageType() == MessageType::kEmpty &&
      query_header.getSender().getPeerInfo() != getSelfNodeInfo().getPeerInfo()) {
    // hop and we are not the start

    LOG_SEARCH_EXACT(SearchExactTestEntryTypes::kHop, query_header.getEventId(),
                     query_header.getSender(), destination,
                     getSelfNodeInfo());  // hop (forwarding)
  }

  auto ref_event_id =
      query_header.getRefEventId() == 0 ? query_header.getEventId() : query_header.getRefEventId();
  MinhtonMessageHeader header(getSelfNodeInfo(), closest_node, ref_event_id);
  MessageSearchExact msg_srch(header, destination, query);
  this->send(msg_srch);
}

void SearchExactAlgorithmGeneral::notifyAboutFailure(const NodeInfo &destination,
                                                     std::shared_ptr<MessageSEVariant> query) {
  MinhtonMessageHeader query_header =
      std::visit([](auto &&msg) -> MinhtonMessageHeader { return msg.getHeader(); }, *query);
  auto ref_event_id =
      query_header.getRefEventId() == 0 ? query_header.getEventId() : query_header.getRefEventId();
  MinhtonMessageHeader header(getSelfNodeInfo(), query_header.getSender(), ref_event_id);
  MessageSearchExactFailure msg_failure(header, destination, query);
  send(msg_failure);
}

}  // namespace minhton
