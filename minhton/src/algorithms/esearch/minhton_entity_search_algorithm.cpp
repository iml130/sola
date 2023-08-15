// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/esearch/minhton_entity_search_algorithm.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

#include "minhton/exception/algorithm_exception.h"
#include "minhton/logging/logging.h"
#include "minhton/message/attribute_inquiry_request.h"
#include "minhton/message/se_types.h"
#include "solanet/uuid_generator.h"

namespace minhton {

MinhtonEntitySearchAlgorithm::MinhtonEntitySearchAlgorithm(std::shared_ptr<AccessContainer> access)
    : EntitySearchAlgorithmInterface(access),
      dsn_handler_(access->routing_info,
                   [&](const NodeInfo &node) { requestAttributeInformation(node); }) {
  value_time_validity_threshold_at_find_query_request_ = 5000;

  value_time_validity_threshold_after_inquiry_aggregation_ =
      value_time_validity_threshold_at_find_query_request_;
  value_time_validity_threshold_after_inquiry_aggregation_ +=
      access_->get_timeout_length(TimeoutType::kInquiryAggregationTimeout);
};

void MinhtonEntitySearchAlgorithm::process(const MessageVariant &msg) {
  std::visit(
      Overload{
          [this](const MessageFindQueryRequest &message) { processFindQueryRequest(message); },
          [this](const MessageFindQueryAnswer &message) { processFindQueryAnswer(message); },
          [this](const MessageAttributeInquiryAnswer &message) {
            processAttributeInquiryAnswer(message);
          },
          [this](const MessageAttributeInquiryRequest &message) {
            processAttributeInquiryRequest(message);
          },
          [this](const MessageSubscriptionUpdate &message) { processSubscriptionUpdate(message); },
          [this](const MessageSubscriptionOrder &message) { processSubscriptionOrder(message); },
          [this](const MessageSearchExactFailure &message) { processSearchExactFailure(message); },
          [](auto & /*message*/) {
            throw AlgorithmException("Wrong Algorithm Interface process called");
          }},
      msg);
}

// called on RequestingNode
std::future<FindResult> MinhtonEntitySearchAlgorithm::find(FindQuery query) {
  // this event id will be the identifier for the find query

  query.setRequestingNode(getSelfNodeInfo());
  std::cout << "\tFind Query at " << access_->get_timestamp() << ": "
            << query.serializeBooleanExpression() << std::endl;

  solanet::UUID id = solanet::generateUUID();
  uint64_t ref_event_id = 0;
  std::memcpy(&ref_event_id, id.data(), 8);
  access_->procedure_info->saveFindQuery(ref_event_id, query);
  access_->procedure_info->saveFindQueryPreliminaryResults(ref_event_id, {});

  std::promise<FindResult> promise;
  auto future = promise.get_future();
  access_->procedure_info->saveFindResultPromise(ref_event_id, std::move(promise));

  LOG_EVENT(EventType::kFindQueryEvent, ref_event_id);
  LOG_FIND_QUERY(ref_event_id, getSelfNodeInfo(), query);

  access_->set_timeout(minhton::TimeoutType::kDsnAggregationTimeout);
  uint64_t timestamp_now = access_->get_timestamp();
  access_->procedure_info->saveDSNAggregationStartTimestamp(ref_event_id, timestamp_now);

  auto dsns_to_send_request_to = calcDSNsToSendInitialCDSForwardingTo();
  auto fanout = getSelfNodeInfo().getFanout();

  for (auto const &dsn_target : dsns_to_send_request_to) {
    MinhtonMessageHeader header(getSelfNodeInfo(), dsn_target, ref_event_id);
    MessageFindQueryRequest msg_request(header, query, MessageFindQueryRequest::kDirectionNone,
                                        {0, pow(fanout, dsn_target.getLevel())});

    if (dsn_target.getLogicalNodeInfo() == getSelfNodeInfo().getLogicalNodeInfo()) {
      if (dsn_handler_.isActive()) {
        performFindQueryForwarding(msg_request);
        performSendInquiryAggregations(ref_event_id, query);
      }
    } else {
      access_->perform_search_exact(dsn_target, std::make_shared<MessageSEVariant>(msg_request));
    }
  }

  return future;
}

// called on DSN
void MinhtonEntitySearchAlgorithm::processFindQueryRequest(const MessageFindQueryRequest &msg) {
  if (!dsn_handler_.isActive()) {
    return;
  }

  performFindQueryForwarding(msg);

  uint64_t ref_event_id = msg.getHeader().getRefEventId();
  FindQuery query = msg.getFindQuery();
  auto requesting_node = query.getRequestingNode();

  if (requesting_node != getSelfNodeInfo()) {
    performSendInquiryAggregations(ref_event_id, query);
  }
}

// called on DSN in performFindQueryRequest
void MinhtonEntitySearchAlgorithm::performFindQueryForwarding(const MessageFindQueryRequest &msg) {
  uint16_t fanout = getSelfNodeInfo().getFanout();
  uint32_t level = getSelfNodeInfo().getLevel();
  uint32_t number = getSelfNodeInfo().getNumber();
  uint64_t ref_event_id = msg.getHeader().getRefEventId();

  auto const interval_start = std::get<0>(msg.getInterval());
  auto const interval_end = std::get<1>(msg.getInterval());
  auto direction = msg.getForwardingDirection();
  auto rt_neighbors = getRoutingInfo()->getAllInitializedRoutingTableNeighbors();
  auto dsn_numbers = getDSNSet(level, fanout);

  std::vector<NodeInfo> dsn_rt_neighbors_in_interval;
  auto is_dsn_and_in_interval = [&](const NodeInfo &node) {
    bool is_dsn = std::find_if(dsn_numbers.begin(), dsn_numbers.end(), [&](const uint32_t &num) {
                    return num == node.getNumber();
                  }) != dsn_numbers.end();
    if (is_dsn) {
      return interval_start < node.getNumber() && node.getNumber() < interval_end;
    }
    return false;
  };
  std::copy_if(rt_neighbors.begin(), rt_neighbors.end(),
               std::back_inserter(dsn_rt_neighbors_in_interval), is_dsn_and_in_interval);

  if (dsn_rt_neighbors_in_interval.empty()) {
    return;
  }

  std::vector<std::tuple<NodeInfo, MessageFindQueryRequest::ForwardingDirection,
                         std::pair<uint32_t, uint32_t>>>
      messages_info;

  if (direction == MessageFindQueryRequest::ForwardingDirection::kDirectionRight ||
      direction == MessageFindQueryRequest::ForwardingDirection::kDirectionNone) {
    std::vector<NodeInfo> right_interval;
    std::copy_if(dsn_rt_neighbors_in_interval.begin(), dsn_rt_neighbors_in_interval.end(),
                 std::back_inserter(right_interval),
                 [&number](const NodeInfo &node) { return node.getNumber() > number; });

    auto it = right_interval.begin();
    for (; it != right_interval.end(); it++) {
      auto it_next = std::next(it);
      uint32_t lower_interval_limit = it->getNumber();
      auto upper_interval_limit = (uint32_t)pow(fanout, level);

      if (it_next != right_interval.end()) {
        upper_interval_limit = it_next->getNumber();
      } else {
        upper_interval_limit = std::min(interval_end, upper_interval_limit);
      }

      messages_info.push_back({*it,
                               MessageFindQueryRequest::ForwardingDirection::kDirectionRight,
                               {lower_interval_limit, upper_interval_limit}});
    }
  }

  if (direction == MessageFindQueryRequest::ForwardingDirection::kDirectionLeft ||
      direction == MessageFindQueryRequest::ForwardingDirection::kDirectionNone) {
    std::vector<NodeInfo> left_interval;
    std::copy_if(dsn_rt_neighbors_in_interval.begin(), dsn_rt_neighbors_in_interval.end(),
                 std::back_inserter(left_interval),
                 [&number](const NodeInfo &node) { return node.getNumber() < number; });
    std::sort(
        left_interval.begin(), left_interval.end(),
        [](const NodeInfo &n1, const NodeInfo &n2) { return n2.getNumber() < n1.getNumber(); });

    auto it = left_interval.begin();
    for (; it != left_interval.end(); it++) {
      auto it_next = std::next(it);
      uint32_t lower_interval_limit = 0;
      uint32_t upper_interval_limit = it->getNumber();

      if (it_next != left_interval.end()) {
        lower_interval_limit = it_next->getNumber();
      } else {
        lower_interval_limit = std::max(lower_interval_limit, interval_start);
      }

      messages_info.push_back({*it,
                               MessageFindQueryRequest::ForwardingDirection::kDirectionLeft,
                               {lower_interval_limit, upper_interval_limit}});
    }
  }

  for (auto const &[node, dir, interval] : messages_info) {
    MinhtonMessageHeader header(getSelfNodeInfo(), node, ref_event_id);
    MessageFindQueryRequest msg_request(header, msg.getFindQuery(), dir, interval);
    send(msg_request);
  }
}

void MinhtonEntitySearchAlgorithm::performSendInquiryAggregations(const uint64_t ref_event_id,
                                                                  FindQuery &query) {
  auto timestamp_now = access_->get_timestamp();

  auto requesting_node = query.getRequestingNode();

  auto undecided_nodes_and_missing_keys =
      dsn_handler_.getUndecidedNodesAndMissingKeys(query, false, timestamp_now);
  auto true_nodes = dsn_handler_.getTrueNodes(query, timestamp_now);

  dsn_handler_.notifyAboutQueryRequest(query, timestamp_now);

  if (requesting_node != getSelfNodeInfo()) {
    access_->procedure_info->saveFindQuery(ref_event_id, query);
  }

  if (undecided_nodes_and_missing_keys.empty() ||
      (query.getScope() == FindQuery::FindQueryScope::kSome &&
       true_nodes.size() >= kFindQuerySomeScopeThreshold)) {
    // no undecided nodes -> we dont have to wait for answers
    concludeAggregationOfInquiries(ref_event_id);
    return;
  }

  auto key_selector = [](auto tuple) { return std::get<0>(tuple); };
  std::vector<NodeInfo> undecided_nodes(undecided_nodes_and_missing_keys.size());
  std::transform(undecided_nodes_and_missing_keys.begin(), undecided_nodes_and_missing_keys.end(),
                 undecided_nodes.begin(), key_selector);

  for (auto &[undecided_node, missing_keys] : undecided_nodes_and_missing_keys) {
    // remove duplicates from missing keys
    std::sort(missing_keys.begin(), missing_keys.end());
    missing_keys.erase(std::unique(missing_keys.begin(), missing_keys.end()), missing_keys.end());

    MinhtonMessageHeader header(getSelfNodeInfo(), undecided_node, ref_event_id);

    bool inquire_all = false;
    if (query.getSelection() == FindQuery::FindQuerySelection::kSelectAll) {
      inquire_all = true;
    }

    MessageAttributeInquiryRequest msg_inquiry(header, inquire_all, missing_keys);
    send(msg_inquiry);
  }

  access_->set_timeout(minhton::TimeoutType::kInquiryAggregationTimeout);
  access_->procedure_info->saveInquiryAggregationStartTimestamp(ref_event_id, timestamp_now);
}

// called on non-DSN
void MinhtonEntitySearchAlgorithm::processAttributeInquiryRequest(
    const MessageAttributeInquiryRequest &msg) {
  MinhtonMessageHeader header(getSelfNodeInfo(), msg.getSender(), msg.getHeader().getRefEventId());

  std::unordered_map<std::string, NodeData::ValueAndType> attribute_values_and_types = {};
  if (msg.getInquireAll()) {
    for (auto const &key : local_data_.getAllCurrentKeys()) {
      NodeData::ValueTimestampAndType val = local_data_.getValueTimestampAndType(key);
      attribute_values_and_types.insert({key, {std::get<0>(val), std::get<2>(val)}});
    }
  } else {
    auto missing_keys = msg.getMissingKeys();
    for (auto const &missing_key : missing_keys) {
      // looking in local data for keys
      // not setting it, if the key is not present
      if (local_data_.hasKey(missing_key)) {
        NodeData::ValueTimestampAndType val = local_data_.getValueTimestampAndType(missing_key);
        attribute_values_and_types.insert({missing_key, {std::get<0>(val), std::get<2>(val)}});
      }
    }
  }

  MessageAttributeInquiryAnswer msg_answer(header, getSelfNodeInfo(), attribute_values_and_types);
  send(msg_answer);
}

// called on DSN
void MinhtonEntitySearchAlgorithm::processAttributeInquiryAnswer(
    const MessageAttributeInquiryAnswer &msg) {
  NodeInfo inquired_node = msg.getInquiredNode();

  auto answered_attribute_values_and_types = msg.getAttributeValuesAndTypes();
  auto removed_attribute_keys = msg.getRemovedAttributeKeys();
  uint64_t timestamp_now = access_->get_timestamp();

  dsn_handler_.updateInquiredOrSubscribedAttributeValues(
      inquired_node, answered_attribute_values_and_types, timestamp_now);

  if (!removed_attribute_keys.empty()) {
    dsn_handler_.updateRemovedAttributes(inquired_node, removed_attribute_keys);
  }
}

// called on DSN
void MinhtonEntitySearchAlgorithm::concludeAggregationOfInquiries(uint64_t ref_event_id) {
  FindQuery query = access_->procedure_info->loadFindQuery(ref_event_id);
  NodeInfo requesting_node = query.getRequestingNode();

  auto timestamp_now = access_->get_timestamp();
  std::vector<NodeInfo> true_nodes = dsn_handler_.getTrueNodes(query, timestamp_now);

  uint64_t validity_threshold_timestamp =
      timestamp_now - value_time_validity_threshold_after_inquiry_aggregation_;
  if (query.evaluate(local_data_, true, validity_threshold_timestamp).isTrue()) {
    true_nodes.push_back(getSelfNodeInfo());
  }

  NodeData::NodesWithAttributes results = getRelevantAttributesAndValues(true_nodes, query);
  NodeData::NodesWithAttributes filtered_results =
      filterAggregationResultsAfterInquiries(results, query.getScope());

  if (requesting_node != getSelfNodeInfo()) {
    MinhtonMessageHeader header(getSelfNodeInfo(), requesting_node, ref_event_id);
    MessageFindQueryAnswer msg_answer(header, filtered_results);
    send(msg_answer);

    // only removing if we are not the requesting node
    // because the requesting node still needs
    // the find query in concludeAggregationOfDSNs
    access_->procedure_info->removeFindQuery(ref_event_id);
  } else {
    // we are requesting node and DSN

    savePreliminaryResultsFromInquiryAggregation(ref_event_id, filtered_results);
  }

  try {
    // might never have been saved if there were no undecided nodes to begin with
    // and no inquiry to start

    access_->procedure_info->removeInquiryAggregationStartTimestamp(ref_event_id);
  } catch (AlgorithmException const &ex) {
  };

  // temporarily deactivating subscriptions
  // optimizeSubscriptions();
}

// called on Requesting Node
void MinhtonEntitySearchAlgorithm::processFindQueryAnswer(const MessageFindQueryAnswer &msg) {
  uint64_t ref_event_id = msg.getHeader().getRefEventId();
  auto fulfilling_nodes_with_attributes = msg.getFulfillingNodesWithAttributes();

  savePreliminaryResultsFromInquiryAggregation(ref_event_id, fulfilling_nodes_with_attributes);
}

void MinhtonEntitySearchAlgorithm::savePreliminaryResultsFromInquiryAggregation(
    const uint64_t ref_event_id, NodeData::NodesWithAttributes results) {
  try {
    for (auto const &[node, attributes] : results) {
      access_->procedure_info->addFindQueryPreliminaryResults(ref_event_id, node, attributes);
    }
  } catch (const AlgorithmException &e) {
    std::cout << "too late " << ref_event_id << std::endl;
  }
}

// called on Requesting Node
void MinhtonEntitySearchAlgorithm::concludeAggregationOfDSNs(uint64_t ref_event_id) {
  FindQuery query = access_->procedure_info->loadFindQuery(ref_event_id);

  NodeData::NodesWithAttributes results =
      access_->procedure_info->loadFindQueryPreliminaryResults(ref_event_id);
  NodeData::NodesWithAttributes filtered_results =
      filterAggregationResultsAfterDSNs(results, query.getScope());

  notifyAboutFindQueryResults(ref_event_id, query, filtered_results);

  access_->procedure_info->removeFindQuery(ref_event_id);
  access_->procedure_info->removeFindQueryPreliminaryResults(ref_event_id);
  access_->procedure_info->removeDSNAggregationStartTimestamp(ref_event_id);
}

// called on Requesting Node - in the very end
void MinhtonEntitySearchAlgorithm::notifyAboutFindQueryResults(
    uint64_t ref_event_id, [[maybe_unused]] const FindQuery &query,
    NodeData::NodesWithAttributes results) {
  FindResult entries;
  for (auto const &[node, attributes] : results) {
    LOG_FIND_QUERY_RESULT(ref_event_id, node);
    std::vector<Entry> node_attributes;
    for (auto &attribute : attributes) {
      Entry entry{std::get<0>(attribute), std::get<1>(attribute), NodeData::kValueDynamic};
      node_attributes.push_back(entry);
    }
    entries.push_back(node_attributes);
    // TODO
  }
  if (results.empty()) {
    LOG_FIND_QUERY_RESULT(ref_event_id, NodeInfo());
  }

  access_->procedure_info->loadFindResultPromise(ref_event_id).set_value(entries);
}

NodeData::NodesWithAttributes MinhtonEntitySearchAlgorithm::getRelevantAttributesAndValues(
    const std::vector<NodeInfo> &true_nodes, const FindQuery &query) {
  NodeData::NodesWithAttributes data;

  std::vector<NodeData::Key> relevant_keys = query.getRelevantAttributes();
  std::vector<NodeData::Key> relevant_topic_keys = query.getRelevantTopicAttributes();

  FindQuery::FindQuerySelection selection = query.getSelection();
  std::vector<NodeData::Key> selected_keys = query.getSelectedAttributeKeys();
  if (selection == FindQuery::FindQuerySelection::kSelectSpecific) {
    relevant_keys = selected_keys;
  }

  auto cover_data = dsn_handler_.getCoverData();

  for (auto const &node : true_nodes) {
    std::vector<std::tuple<NodeData::Key, NodeData::Value>> node_data;

    if (node.getLogicalNodeInfo() == getSelfNodeInfo().getLogicalNodeInfo()) {
      // local

      if (selection == FindQuery::FindQuerySelection::kSelectAll || relevant_keys.empty()) {
        relevant_keys = local_data_.getAllCurrentKeys();
      }

      for (auto const &key : relevant_keys) {
        if (local_data_.hasKey(key)) {
          auto value = local_data_.getValue(key);
          node_data.push_back({key, value});
        } else if (std::find(relevant_topic_keys.begin(), relevant_topic_keys.end(), key) !=
                   relevant_topic_keys.end()) {
          node_data.push_back({key, false});
        }
      }

    } else {
      // distributed

      auto it = cover_data.find(node.getLogicalNodeInfo());
      if (it != cover_data.end()) {
        DistributedData &distr_data = it->second;
        if (selection == FindQuery::FindQuerySelection::kSelectAll || relevant_keys.empty()) {
          relevant_keys = distr_data.getAllCurrentKeys();
        }

        for (auto const &key : relevant_keys) {
          if (distr_data.hasKey(key)) {
            auto value = distr_data.getValue(key);
            node_data.push_back({key, value});
          } else if (std::find(relevant_topic_keys.begin(), relevant_topic_keys.end(), key) !=
                     relevant_topic_keys.end()) {
            node_data.push_back({key, false});
          }
        }
      }
    }

    if (!node_data.empty()) {
      data[node] = node_data;
    }
  }

  return data;
}

// called on Requesting Node
void MinhtonEntitySearchAlgorithm::processDSNAggregationTimeout() {
  uint64_t timestamp_now = access_->get_timestamp();
  uint16_t timeout_length =
      access_->get_timeout_length(minhton::TimeoutType::kDsnAggregationTimeout);
  uint64_t treshold = timestamp_now - timeout_length;

  auto dsn_aggregation_start_timestamps_map =
      access_->procedure_info->getDSNAggregationStartTimestampMap();

  for (auto const &[ref_event_id, start_timestamp] : dsn_aggregation_start_timestamps_map) {
    if (start_timestamp >= treshold) {
      concludeAggregationOfDSNs(ref_event_id);
    }
  }
}

// called on DSN
void MinhtonEntitySearchAlgorithm::processInquiryAggregationTimeout() {
  uint64_t timestamp_now = access_->get_timestamp();
  uint16_t timeout_length =
      access_->get_timeout_length(minhton::TimeoutType::kInquiryAggregationTimeout);
  uint64_t treshold = timestamp_now - timeout_length;

  auto inquiry_aggregation_start_timestamps_map =
      access_->procedure_info->getInquiryAggregationStartTimestampMap();

  for (auto const &[ref_event_id, start_timestamp] : inquiry_aggregation_start_timestamps_map) {
    if (start_timestamp <= treshold) {
      concludeAggregationOfInquiries(ref_event_id);
    }
  }
}

std::vector<NodeInfo> MinhtonEntitySearchAlgorithm::calcDSNsToSendInitialCDSForwardingTo() const {
  std::vector<NodeInfo> initial_dsns;

  auto self = getSelfNodeInfo();
  auto fanout = self.getFanout();
  auto adj_left = getRoutingInfo()->getAdjacentLeft();
  auto adj_right = getRoutingInfo()->getAdjacentRight();

  auto this_level_ds = getDSNSet(self.getLevel(), fanout);
  bool we_are_dsn = !this_level_ds.empty() && std::find(this_level_ds.begin(), this_level_ds.end(),
                                                        self.getNumber()) != this_level_ds.end();

  uint32_t known_max_level = 0;
  if (adj_left.getLevel() != adj_right.getLevel() || adj_left.getLevel() != self.getLevel() ||
      adj_right.getLevel() != self.getLevel()) {
    // exact approximation
    known_max_level = std::max(adj_left.getLevel(), adj_right.getLevel());
    known_max_level = std::max(known_max_level, self.getLevel());
  } else {
    known_max_level = adj_left.getLevel();
  }

  if (self.getLevel() != 0) {
    initial_dsns.push_back(NodeInfo(0, 0, fanout));
  } else {
    initial_dsns.push_back(self);
  }

  for (uint32_t level = 2; level < known_max_level; level += 2) {
    if (self.getLevel() == level && we_are_dsn) {
      initial_dsns.push_back(self);

    } else {
      auto level_ds = getDSNSet(level, fanout);
      if (level_ds.size() <= (size_t)fanout + 1) {
        // pick first
        initial_dsns.push_back(NodeInfo(level, fanout, fanout));

      } else {
        // pick dsn in middle of set
        int middle_index = level_ds.size() / 2;
        auto it = level_ds.begin();
        std::advance(it, middle_index);
        uint32_t middle_number = *it;

        initial_dsns.push_back(NodeInfo(level, middle_number, fanout));
      }
    }
  }

  if (known_max_level % 2 == 0) {
    if (self.getLevel() == known_max_level && we_are_dsn) {
      initial_dsns.push_back(self);
    } else {
      initial_dsns.push_back(NodeInfo(known_max_level, known_max_level == 0 ? 0 : fanout, fanout));
    }
  } else {
    initial_dsns.push_back(NodeInfo(known_max_level + 1, fanout, fanout));
  }

  return initial_dsns;
}

}  // namespace minhton
