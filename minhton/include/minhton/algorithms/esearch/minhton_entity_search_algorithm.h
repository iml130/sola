// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_ESEARCH_MINHTON_ENTITIY_SEARCH_ALGORITHM_H_
#define MINHTON_ALGORITHMS_ESEARCH_MINHTON_ENTITIY_SEARCH_ALGORITHM_H_

#include <memory>

#include "minhton/algorithms/esearch/dsn_handler.h"
#include "minhton/algorithms/esearch/interface_entity_search_algorithm.h"
#include "minhton/algorithms/esearch/local_data.h"
#include "minhton/core/definitions.h"
#include "minhton/message/attribute_inquiry_answer.h"
#include "minhton/message/attribute_inquiry_request.h"
#include "minhton/message/find_query_answer.h"
#include "minhton/message/find_query_request.h"
#include "minhton/message/message.h"
#include "minhton/message/search_exact_failure.h"
#include "minhton/message/subscription_order.h"
#include "minhton/message/subscription_update.h"

namespace minhton {

class MinhtonEntitySearchAlgorithm : public EntitySearchAlgorithmInterface {
public:
  explicit MinhtonEntitySearchAlgorithm(std::shared_ptr<AccessContainer> access);

  void process(const MessageVariant &msg) override;
  void processTimeout(const TimeoutType &type) override;

  std::future<FindResult> find(FindQuery query) override;

  void localInsert(std::vector<Entry> entries) override;
  void localUpdate(std::vector<Entry> entries) override;
  void localRemove(std::vector<std::string> keys) override;

private:
  void processFindQueryRequest(const MessageFindQueryRequest &msg);
  void processFindQueryAnswer(const MessageFindQueryAnswer &msg);
  void processAttributeInquiryAnswer(const MessageAttributeInquiryAnswer &msg);
  void processAttributeInquiryRequest(const MessageAttributeInquiryRequest &msg);
  void processSubscriptionUpdate(const MessageSubscriptionUpdate &msg);
  void processSubscriptionOrder(const MessageSubscriptionOrder &msg);
  void processSearchExactFailure(const MessageSearchExactFailure &msg);

  void performFindQueryForwarding(const MessageFindQueryRequest &msg);
  void performSendInquiryAggregations(uint64_t ref_event_id, FindQuery &query);
  void savePreliminaryResultsFromInquiryAggregation(uint64_t ref_event_id,
                                                    NodeData::NodesWithAttributes results);

  void processDSNAggregationTimeout();
  void processInquiryAggregationTimeout();

  void notifyAboutFindQueryResults(uint64_t ref_event_id, const FindQuery &query,
                                   NodeData::NodesWithAttributes results);

  void concludeAggregationOfInquiries(uint64_t ref_event_id);
  void concludeAggregationOfDSNs(uint64_t ref_event_id);

  void requestAttributeInformation(const NodeInfo &node);

  void optimizeSubscriptions();
  void updateSubscribers(NodeData::Key key);

  std::vector<NodeInfo> calcDSNsToSendInitialCDSForwardingTo() const;

  static NodeData::NodesWithAttributes filterAggregationResultsAfterInquiries(
      const NodeData::NodesWithAttributes &full_results, const FindQuery::FindQueryScope &scope);

  static NodeData::NodesWithAttributes filterAggregationResultsAfterDSNs(
      const NodeData::NodesWithAttributes &full_results, const FindQuery::FindQueryScope &scope);

  static NodeData::NodesWithAttributes filterDuplicateAttributes(
      NodeData::NodesWithAttributes full_results);

  NodeData::NodesWithAttributes getRelevantAttributesAndValues(
      const std::vector<NodeInfo> &true_nodes, const FindQuery &query);

  void logKeyValueContentUpdateInsert(NodeData::Key key, NodeData::Value value,
                                      NodeData::ValueType type);
  void logKeyValueContentRemove(NodeData::Key key);

  LocalData local_data_;
  DSNHandler dsn_handler_;

  static const uint8_t kFindQuerySomeScopeThreshold = 2;

  const uint16_t default_num_of_forwarding_hops_per_dsn_ = 5;

  uint64_t value_time_validity_threshold_at_find_query_request_;
  uint64_t value_time_validity_threshold_after_inquiry_aggregation_;

  bool publish_attributes_after_insert_and_removal_ = true;
};

}  // namespace minhton

#endif
