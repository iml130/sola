// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <algorithm>

#include "minhton/algorithms/esearch/minhton_entity_search_algorithm.h"
#include "minhton/core/routing_calculations.h"
#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"
#include "minhton/message/attribute_inquiry_request.h"
#include "minhton/message/search_exact.h"

namespace minhton {

void MinhtonEntitySearchAlgorithm::processTimeout(const TimeoutType &type) {
  if (type == minhton::TimeoutType::kDsnAggregationTimeout) {
    this->processDSNAggregationTimeout();
  } else if (type == minhton::TimeoutType::kInquiryAggregationTimeout) {
    this->processInquiryAggregationTimeout();
  }
}

void MinhtonEntitySearchAlgorithm::localInsert(std::vector<Entry> entries) {
  for (auto &[key, value, type] : entries) {
    auto timestamp_now = this->access_->get_timestamp();
    // TODO static
    if (!local_data_.insert(key, {value, timestamp_now, type})) {
      throw AlgorithmException("already inserted");
    }

    updateSubscribers(key);
    logKeyValueContentUpdateInsert(key, value, type);
  }

  if (publish_attributes_after_insert_and_removal_) {
    bool we_are_dsn = getRoutingInfo()->areWeDSN();

    // if we are a dsn, we dont need to push our insert entries to ourselves
    // if we are a temp dsn, we might be not a dsn later on, so we should still publish to our dsn

    if (!we_are_dsn) {
      NodeInfo our_dsn = getRoutingInfo()->getCoveringDSNOrTempDSN();
      MinhtonMessageHeader header(this->getSelfNodeInfo(), our_dsn);

      std::unordered_map<std::string, NodeData::ValueAndType> attribute_values_and_types = {};
      for (auto &key : local_data_.getAllCurrentKeys()) {
        NodeData::ValueTimestampAndType val = local_data_.getValueTimestampAndType(key);
        attribute_values_and_types.insert({key, {std::get<0>(val), std::get<2>(val)}});
      }

      MessageAttributeInquiryAnswer msg(header, this->getSelfNodeInfo(),
                                        attribute_values_and_types);
      this->access_->perform_search_exact(our_dsn, std::make_shared<MessageSEVariant>(msg));
    }
  }
}

void MinhtonEntitySearchAlgorithm::localUpdate(std::vector<Entry> entries) {
  for (auto &[key, value, type] : entries) {
    // TODO static
    if (local_data_.hasKey(key)) {
      if (local_data_.getValue(key) == value) {
        // not updating if the update value is the same as the current one
        return;
      }
    }

    auto timestamp_now = this->access_->get_timestamp();
    if (!local_data_.update(key, {value, timestamp_now, type})) {
      throw AlgorithmException("could not update");
    }

    updateSubscribers(key);
    logKeyValueContentUpdateInsert(key, value, type);
  }
}

void MinhtonEntitySearchAlgorithm::localRemove(std::vector<std::string> keys) {
  for (const auto &key : keys) {
    local_data_.remove(key);
    logKeyValueContentRemove(key);
    updateSubscribers(key);
  }

  if (publish_attributes_after_insert_and_removal_) {
    throw std::logic_error("not implemented yet");
  }
}

// called on DSN
void MinhtonEntitySearchAlgorithm::optimizeSubscriptions() {
  auto timestamp_now = this->access_->get_timestamp();
  auto subscribe_map = dsn_handler_.getNodesAndKeysToSubscribe(timestamp_now);
  auto unsubscribe_map = dsn_handler_.getNodesAndKeysToUnsubscribe(timestamp_now);

  for (auto &[node, sub_keys] : subscribe_map) {
    MinhtonMessageHeader header(getSelfNodeInfo(), node);
    MessageSubscriptionOrder msg_sub_order(header, sub_keys, true);
    this->send(msg_sub_order);
  }

  for (auto &[node, unsub_keys] : unsubscribe_map) {
    MinhtonMessageHeader header(getSelfNodeInfo(), node);
    MessageSubscriptionOrder msg_unsub_order(header, unsub_keys, false);
    this->send(msg_unsub_order);
  }

  dsn_handler_.setPlacedSubscriptionOrders(subscribe_map);
  dsn_handler_.setPlacedUnsubscriptionOrders(unsubscribe_map);
}

// called on non-DSN
void MinhtonEntitySearchAlgorithm::processSubscriptionOrder(const MessageSubscriptionOrder &msg) {
  auto orderer = msg.getSender();

  auto keys = msg.getKeys();
  bool subscribed = msg.getSubscribe();

  for (const auto &key : keys) {
    if (subscribed) {
      local_data_.addKeySubscriber(key, orderer);
    } else {
      local_data_.removeKeySubscriber(key, orderer);
    }
  }
}

void MinhtonEntitySearchAlgorithm::updateSubscribers(NodeData::Key key) {
  auto key_subscribers = local_data_.getSubscribers(key);
  NodeData::Value value = local_data_.getValue(key);

  for (auto const &sub : key_subscribers) {
    MinhtonMessageHeader header(getSelfNodeInfo(), sub);
    MessageSubscriptionUpdate msg_update(header, key, value);
    this->send(msg_update);
  }
}

// called on DSN
void MinhtonEntitySearchAlgorithm::processSubscriptionUpdate(const MessageSubscriptionUpdate &msg) {
  NodeInfo updated_node = msg.getSender();

  NodeData::Key key = msg.getKey();
  NodeData::Value value = msg.getValue();

  uint64_t timestamp_now = this->access_->get_timestamp();
  dsn_handler_.updateInquiredOrSubscribedAttributeValues(
      updated_node, {{key, {value, NodeData::ValueType::kValueDynamic}}}, timestamp_now);

  // by definition, only dynamic attributes get subscribed
}

void MinhtonEntitySearchAlgorithm::logKeyValueContentUpdateInsert(NodeData::Key key,
                                                                  NodeData::Value value,
                                                                  NodeData::ValueType type) {
  NodeInfo node = this->getSelfNodeInfo();

  try {
    bool bool_value = std::get<bool>(value);
    LOG_CONTENT(node, ContentStatus::kContentInsertUpdate, key, type, std::to_string(bool_value));
  } catch (std::bad_variant_access const &ex) {
  };

  try {
    int int_value = std::get<int>(value);
    LOG_CONTENT(node, ContentStatus::kContentInsertUpdate, key, type, std::to_string(int_value));
    return;
  } catch (std::bad_variant_access const &ex) {
  };

  try {
    float float_value = std::get<float>(value);
    LOG_CONTENT(node, ContentStatus::kContentInsertUpdate, key, type, std::to_string(float_value));
    return;
  } catch (std::bad_variant_access const &ex) {
  };

  try {
    std::string string_value = std::get<std::string>(value);
    LOG_CONTENT(node, ContentStatus::kContentInsertUpdate, key, type, string_value);
    return;
  } catch (std::bad_variant_access const &ex) {
  };
}

void MinhtonEntitySearchAlgorithm::logKeyValueContentRemove(NodeData::Key key) {
  LOG_CONTENT(getSelfNodeInfo(), ContentStatus::kContentRemove, key,
              NodeData::ValueType::kValueDynamic, "");
}

void MinhtonEntitySearchAlgorithm::requestAttributeInformation(const NodeInfo &node) {
  MinhtonMessageHeader header(getSelfNodeInfo(), node);
  MessageAttributeInquiryRequest msg(header, true);

  if (node.isInitialized()) {
    send(msg);
  } else {
    access_->perform_search_exact(node, std::make_shared<MessageSEVariant>(msg));
  }
}

void MinhtonEntitySearchAlgorithm::processSearchExactFailure(const MessageSearchExactFailure &msg) {
  auto query = msg.getQuery();
  uint16_t fanout = getRoutingInfo()->getFanout();
  MinhtonMessageHeader query_header =
      std::visit([](auto &&msg) -> MinhtonMessageHeader { return msg.getHeader(); }, *query);

  if (query_header.getMessageType() == MessageType::kFindQueryRequest ||
      query_header.getMessageType() == MessageType::kAttributeInquiryAnswer) {
    auto non_existing_target = query_header.getTarget();
    auto const &[parent_level, parent_number] =
        calcParent(non_existing_target.getLevel(), non_existing_target.getNumber(), fanout);

    // dsn does not exist -> sending to its parent instead

    NodeInfo new_dsn_target(parent_level, parent_number, fanout);
    std::visit([new_dsn_target](auto &&query_msg) { query_msg.setTarget(new_dsn_target); }, *query);

    access_->perform_search_exact(new_dsn_target, query);

  } else {
    throw std::logic_error("not implemented yet");
  }
}

NodeData::NodesWithAttributes MinhtonEntitySearchAlgorithm::filterAggregationResultsAfterInquiries(
    const NodeData::NodesWithAttributes &full_results, const FindQuery::FindQueryScope &scope) {
  NodeData::NodesWithAttributes cleaned_results = filterDuplicateAttributes(full_results);

  if (scope == FindQuery::FindQueryScope::kAll) {
    return cleaned_results;
  }
  if (scope == FindQuery::FindQueryScope::kSome) {
    std::vector<std::pair<NodeInfo, NodeData::Attributes>> temp(cleaned_results.begin(),
                                                                cleaned_results.end());
    temp.resize(kFindQuerySomeScopeThreshold);
    auto filtered_results = NodeData::NodesWithAttributes(temp.begin(), temp.end());
    return filtered_results;
  }
  throw std::logic_error("not implemented scope option");
}

NodeData::NodesWithAttributes MinhtonEntitySearchAlgorithm::filterAggregationResultsAfterDSNs(
    const NodeData::NodesWithAttributes &full_results,
    [[maybe_unused]] const FindQuery::FindQueryScope &scope) {
  return filterDuplicateAttributes(full_results);
}

NodeData::NodesWithAttributes MinhtonEntitySearchAlgorithm::filterDuplicateAttributes(
    NodeData::NodesWithAttributes full_results) {
  auto key_comparison = [](const std::tuple<NodeData::Key, NodeData::Value> &t1,
                           const std::tuple<NodeData::Key, NodeData::Value> &t2) {
    return std::get<0>(t1) < std::get<0>(t2);
  };

  auto same_key = [](const std::tuple<NodeData::Key, NodeData::Value> &t1,
                     const std::tuple<NodeData::Key, NodeData::Value> &t2) {
    return std::get<0>(t1) == std::get<0>(t2);
  };

  for (auto &[node, attributes] : full_results) {
    // sort attributes by key
    std::sort(attributes.begin(), attributes.end(), key_comparison);
    attributes.erase(std::unique(attributes.begin(), attributes.end(), same_key), attributes.end());
  }

  return full_results;
}

}  // namespace minhton
