// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/core/logic_container.h"

#include "minhton/algorithms/esearch/minhton_entity_search_algorithm.h"
#include "minhton/algorithms/join/minhton_join_algorithm.h"
#include "minhton/algorithms/leave/minhton_leave_algorithm.h"
#include "minhton/algorithms/misc/bootstrap_algorithm_general.h"
#include "minhton/algorithms/misc/response_algorithm_general.h"
#include "minhton/algorithms/search_exact/minhton_search_exact_algorithm.h"
#include "minhton/exception/algorithm_exception.h"

namespace minhton {

LogicContainer::LogicContainer(std::shared_ptr<AccessContainer> access,
                               const AlgorithmTypesContainer &types_container) {
  if (types_container.join == JoinAlgorithms::kJoinMinhton) {
    join_algo_ = std::make_unique<MinhtonJoinAlgorithm>(access);
  } else {
    throw std::invalid_argument("Invalid Join Algorithm Type");
  }

  if (types_container.leave == LeaveAlgorithms::kLeaveMinhton) {
    leave_algo_ = std::make_unique<MinhtonLeaveAlgorithm>(access);
  } else {
    throw std::invalid_argument("Invalid Leave Algorithm Type");
  }

  if (types_container.search_exact == SearchExactAlgorithms::kSearchExactMinhton) {
    search_exact_algo_ = std::make_unique<MinhtonSearchExactAlgorithm>(access);
  } else {
    throw std::invalid_argument("Invalid Search Exact Algorithm Type");
  }

  if (types_container.response == ResponseAlgorithms::kResponseGeneral) {
    response_algo_ = std::make_unique<ResponseAlgorithmGeneral>(access);
  } else {
    throw std::invalid_argument("Invalid Response Algorithm Type");
  }

  if (types_container.bootstrap == BootstrapAlgorithms::kBootstrapGeneral) {
    bootstrap_algo_ = std::make_unique<BootstrapAlgorithmGeneral>(access);
  } else {
    throw std::invalid_argument("Invalid Bootstrap Algorithm Type");
  }

  entity_search_algo_ = std::make_unique<MinhtonEntitySearchAlgorithm>(access);

  access->replace_myself = [this](NodeInfo node_to_replace,
                                  std::vector<NodeInfo> neighbors_of_node_to_replace) -> void {
    leave_algo_->replaceMyself(node_to_replace, neighbors_of_node_to_replace);
  };

  access->continue_accept_child_procedure =
      [this](const minhton::MessageInformAboutNeighbors &message) -> void {
    join_algo_->continueAcceptChildProcedure(message);
  };

  access->perform_search_exact = [this](const minhton::NodeInfo &destination,
                                        std::shared_ptr<MessageSEVariant> query) -> void {
    search_exact_algo_->performSearchExact(destination, query);
  };

  access->wait_for_acks = [this](uint32_t number, std::function<void()> cb) -> void {
    response_algo_->waitForAcks(number, cb);
  };

  associateMessageTypesWithAlgorithm(JoinAlgorithmInterface::getSupportedMessageTypes(),
                                     join_algo_);
  associateMessageTypesWithAlgorithm(LeaveAlgorithmInterface::getSupportedMessageTypes(),
                                     leave_algo_);
  associateMessageTypesWithAlgorithm(SearchExactAlgorithmInterface::getSupportedMessageTypes(),
                                     search_exact_algo_);
  associateMessageTypesWithAlgorithm(ResponseAlgorithmInterface::getSupportedMessageTypes(),
                                     response_algo_);
  associateMessageTypesWithAlgorithm(BootstrapAlgorithmInterface::getSupportedMessageTypes(),
                                     bootstrap_algo_);
  associateMessageTypesWithAlgorithm(EntitySearchAlgorithmInterface::getSupportedMessageTypes(),
                                     entity_search_algo_);
}

void LogicContainer::associateMessageTypesWithAlgorithm(
    std::vector<minhton::MessageType> types, std::shared_ptr<AlgorithmInterface> algorithm) {
  for (MessageType type : types) {
    message_type_to_algorithm_map_[type] = algorithm;
  }
}

void LogicContainer::process(const MessageVariant &msg) {
  // Process the received message with the algorithm responsible for the message type
  MessageType type = std::visit(
      [this](auto &&message) -> MessageType { return message.getHeader().getMessageType(); }, msg);
  auto algorithm_to_use = message_type_to_algorithm_map_.at(type);
  algorithm_to_use->process(msg);
}

void LogicContainer::processSignal(Signal &signal) {
  if (signal.signal_type == SignalType::kLeaveNetwork) {
    this->leave_algo_->initiateSelfDeparture();
  } else if (signal.signal_type == SignalType::kJoinNetwork) {
    if (signal.join_via_bootstrap) {
      this->bootstrap_algo_->initiateJoin(PhysicalNodeInfo(signal.join_address, signal.join_port));
    } else {
      if (signal.join_nodeinfo.isInitialized()) {
        this->join_algo_->initiateJoin(signal.join_nodeinfo);
      } else {
        this->join_algo_->initiateJoin(PhysicalNodeInfo(signal.join_address, signal.join_port));
      }
    }
  }
}

void LogicContainer::processTimeout(const Timeout &timeout_event) {
  switch (timeout_event.timeout_type) {
    case TimeoutType::kBootstrapResponseTimeout: {
      if (timeout_event.valid_bootstrap_response) {
        this->bootstrap_algo_->processBootstrapResponseTimeout();
      }
    } break;

    case TimeoutType::kDsnAggregationTimeout: {
      this->entity_search_algo_->processTimeout(TimeoutType::kDsnAggregationTimeout);
    } break;

    case TimeoutType::kInquiryAggregationTimeout: {
      this->entity_search_algo_->processTimeout(TimeoutType::kInquiryAggregationTimeout);
    } break;

    default: {
    }
  }
}

bool LogicContainer::isBootstrapResponseValid() const {
  return this->bootstrap_algo_->isBootstrapResponseValid();
}

bool LogicContainer::canLeaveWithoutReplacement() {
  return this->leave_algo_->canLeaveWithoutReplacement();
}

void LogicContainer::performSearchExactTest(const NodeInfo &destination,
                                            std::shared_ptr<MessageSEVariant> query) {
  search_exact_algo_->performSearchExact(destination, query);
}

void LogicContainer::localInsert(const std::vector<Entry> &entries) {
  entity_search_algo_->localInsert(entries);
}

void LogicContainer::localUpdate(const std::vector<Entry> &entries) {
  entity_search_algo_->localUpdate(entries);
}

void LogicContainer::localRemove(const std::vector<std::string> &keys) {
  entity_search_algo_->localRemove(keys);
}

std::future<FindResult> LogicContainer::find(const FindQuery &query) {
  return entity_search_algo_->find(query);
}

}  // namespace minhton
