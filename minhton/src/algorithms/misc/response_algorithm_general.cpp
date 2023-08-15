// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/misc/response_algorithm_general.h"

#include <cassert>

#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"
#include "minhton/message/remove_and_update_neighbor.h"
#include "minhton/message/remove_neighbor_ack.h"

namespace minhton {

void ResponseAlgorithmGeneral::process(const MessageVariant &msg) {
  std::visit(
      Overload{[this](const MessageUpdateNeighbors &message) { processUpdateNeighbors(message); },
               [this](const MessageRemoveNeighbor &message) { processRemoveNeighbor(message); },
               [this](const MessageInformAboutNeighbors &message) {
                 processInformAboutNeighbors(message);
               },
               [this](const MessageGetNeighbors &message) { processGetNeighbors(message); },
               [this](const MessageRemoveAndUpdateNeighbors &message) {
                 processRemoveAndUpdateNeighbors(message);
               },
               [this](const MessageRemoveNeighborAck &) { processRemoveNeighborAck(); },
               [](auto & /*message*/) {
                 throw AlgorithmException("Wrong Algorithm Interface process called");
               }},
      msg);
}

void ResponseAlgorithmGeneral::processRemoveNeighborAck() {
  assert(cb_);
  assert(number_ > 0);
  number_--;

  if (number_ == 0) {
    cb_();
    cb_ = std::function<void()>{};
  }
}

void ResponseAlgorithmGeneral::processUpdateNeighbors(const MessageUpdateNeighbors &msg) {
  minhton::NodeInfo sender = msg.getSender();
  auto neighbors_and_relationships = msg.getNeighborsToUpdate();

  for (auto const &neighbor_and_relationship : neighbors_and_relationships) {
    auto neighbor = std::get<0>(neighbor_and_relationship);
    minhton::NeighborRelationship relationship = std::get<1>(neighbor_and_relationship);

    switch (relationship) {
      case minhton::NeighborRelationship::kAdjacentLeft: {
        if (neighbor.isInitialized()) {
          getRoutingInfo()->setAdjacentLeft(neighbor, msg.getHeader().getRefEventId());
        } else {
          getRoutingInfo()->resetAdjacentLeft(msg.getHeader().getRefEventId());
        }
        break;
      }

      case minhton::NeighborRelationship::kAdjacentRight: {
        if (neighbor.isInitialized()) {
          getRoutingInfo()->setAdjacentRight(neighbor, msg.getHeader().getRefEventId());
        } else {
          getRoutingInfo()->resetAdjacentRight(msg.getHeader().getRefEventId());
        }
        break;
      }

      case minhton::NeighborRelationship::kRoutingTableNeighbor: {
        getRoutingInfo()->updateRoutingTableNeighbor(neighbor, msg.getHeader().getRefEventId());
        break;
      }

      case minhton::NeighborRelationship::kRoutingTableNeighborChild: {
        getRoutingInfo()->updateRoutingTableNeighborChild(neighbor,
                                                          msg.getHeader().getRefEventId());
        break;
      }

      case minhton::NeighborRelationship::kUnknownRelationship: {
        getRoutingInfo()->updateNeighbor(neighbor, msg.getHeader().getRefEventId());
        break;
      }

      default: {
        throw AlgorithmException("Requested Relationship not supported yet");
      }
    }
  }

  if (msg.getShouldAcknowledge()) {
    MinhtonMessageHeader header(getSelfNodeInfo(), msg.getSender(),
                                msg.getHeader().getRefEventId());
    MessageRemoveNeighborAck ack(header);
    this->send(ack);
  }
}

void ResponseAlgorithmGeneral::waitForAcks(uint32_t number, std::function<void()> cb) {
  assert(!cb_);
  assert(number_ == 0);
  if (number == 0) {
    cb();
    return;
  }

  number_ = number;
  cb_ = cb;
}

void ResponseAlgorithmGeneral::processGetNeighbors(const MessageGetNeighbors &msg) {
  std::vector<NodeInfo> neighbors = {};

  for (auto const &rel : msg.getRelationships()) {
    switch (rel) {
      case NeighborRelationship::kAdjacentLeft: {
        if (getRoutingInfo()->getAdjacentLeft().isInitialized()) {
          neighbors.push_back(getRoutingInfo()->getAdjacentLeft());
        }
        break;
      }

      case NeighborRelationship::kAdjacentRight: {
        if (getRoutingInfo()->getAdjacentRight().isInitialized()) {
          neighbors.push_back(getRoutingInfo()->getAdjacentRight());
        }
        break;
      }

      case NeighborRelationship::kChild: {
        for (auto const &child : getRoutingInfo()->getInitializedChildren()) {
          neighbors.push_back(child);
        }
        break;
      }

      case NeighborRelationship::kParent: {
        if (getRoutingInfo()->getParent().isInitialized()) {
          neighbors.push_back(getRoutingInfo()->getParent());
        }
        break;
      }

      case NeighborRelationship::kRoutingTableNeighbor: {
        for (auto const &neighbor : getRoutingInfo()->getAllInitializedRoutingTableNeighbors()) {
          neighbors.push_back(neighbor);
        }
        break;
      }

      case NeighborRelationship::kRoutingTableNeighborChild: {
        for (auto const &neighbor_child :
             getRoutingInfo()->getAllInitializedRoutingTableNeighborChildren()) {
          neighbors.push_back(neighbor_child);
        }
        break;
      }

      default:
        throw AlgorithmException("Requested Relationship not supported yet");
    }
  }

  if (!neighbors.empty()) {
    MinhtonMessageHeader header(getSelfNodeInfo(), msg.getSendBackToNode(),
                                msg.getHeader().getRefEventId());
    MessageInformAboutNeighbors message_inform(header, neighbors);
    this->send(message_inform);
  }
}

void ResponseAlgorithmGeneral::processRemoveAndUpdateNeighbors(
    const MessageRemoveAndUpdateNeighbors &msg) {
  if (msg.getTarget() != getSelfNodeInfo()) {
    return;
  }  // edge case: Our old position was the leftmost child
  assert(!msg.getMessageRemoveNeighbor().getShouldAcknowledge());
  assert(!msg.getMessageUpdateNeighbors().getShouldAcknowledge());
  processRemoveNeighbor(msg.getMessageRemoveNeighbor());
  processUpdateNeighbors(msg.getMessageUpdateNeighbors());

  if (msg.getShouldAcknowledge()) {
    MinhtonMessageHeader header(getSelfNodeInfo(), msg.getSender(),
                                msg.getHeader().getRefEventId());
    MessageRemoveNeighborAck ack(header);
    this->send(ack);
  }
}

void ResponseAlgorithmGeneral::processRemoveNeighbor(const MessageRemoveNeighbor &msg) {
  bool should_acknowledge = msg.getShouldAcknowledge();

  getRoutingInfo()->removeNeighbor(msg.getRemovedPositionNode(), msg.getHeader().getRefEventId());

  if (msg.getSender().getLevel() - 1 == getRoutingInfo()->getSelfNodeInfo().getLevel()) {
    // Node to be removed is our child. Therefore, forward this MessageRemoveNeighbor to all our
    // neighbors as they also know our (old) child
    for (const auto &neighbor : getRoutingInfo()->getRoutingTableNeighbors()) {
      MinhtonMessageHeader header(getSelfNodeInfo(), neighbor, msg.getHeader().getRefEventId());
      MessageRemoveNeighbor remove_neighbor(header, msg.getRemovedPositionNode(),
                                            msg.getShouldAcknowledge());
      this->send(remove_neighbor);
    }

    should_acknowledge = false;  // Should not acknowledge ourself
  }

  if (should_acknowledge) {
    MinhtonMessageHeader header(getSelfNodeInfo(), msg.getSender(), msg.getHeader().getEventId());
    MessageRemoveNeighborAck ack(header);
    this->send(ack);
  }
}

void ResponseAlgorithmGeneral::processInformAboutNeighbors(const MessageInformAboutNeighbors &msg) {
  if (this->access_->procedure_info->hasKey(ProcedureKey::kAcceptChildProcedure)) {
    this->access_->continue_accept_child_procedure(msg);
  } else {
    // just updating our neighbors with the given information
    for (auto const &node : msg.getRequestedNeighbors()) {
      getRoutingInfo()->updateNeighbor(node, msg.getHeader().getRefEventId());
    }
  }
}

}  // namespace minhton
