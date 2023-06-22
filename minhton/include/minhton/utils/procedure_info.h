// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_PROCEDURES_INFO_H_
#define MINHTON_PROCEDURES_INFO_H_

#include <future>
#include <unordered_map>
#include <vector>

#include "minhton/algorithms/esearch/find_query.h"
#include "minhton/core/definitions.h"
#include "minhton/core/node_info.h"
#include "minhton/core/physical_node_info.h"

namespace minhton {

enum ProcedureKey : uint8_t {
  kBootstrapProcess,
  kJoinProcedure,
  kLeaveProcedure,
  kAcceptChildProcedure,
  kFindReplacementProcedure,
  kEntitySearchUndecidedNodeInquiries,
};

///
/// A class to store intermediate procedure states.
/// Similar to a key-value dictionary.
///
/// The key is a
///
/// Supports saving, loading, updating and removing of procedure states.
///
class ProcedureInfo {
public:
  ProcedureInfo();
  ~ProcedureInfo();

  bool hasKey(ProcedureKey key) const;

  void save(ProcedureKey key, const std::vector<minhton::NodeInfo> &value);
  std::vector<minhton::NodeInfo> load(ProcedureKey key);
  void update(ProcedureKey key, const std::vector<minhton::NodeInfo> &value);
  void remove(ProcedureKey key);

  void saveEventId(ProcedureKey key, uint64_t event_id);
  uint64_t loadEventId(ProcedureKey key);
  void removeEventId(ProcedureKey key);

  void saveFindQuery(uint64_t ref_event_id, const FindQuery &value);
  FindQuery loadFindQuery(uint64_t ref_event_id);
  void updateFindQuery(uint64_t ref_event_id, const FindQuery &value);
  void removeFindQuery(uint64_t ref_event_id);

  void saveFindQueryUndecidedNodes(uint64_t ref_event_id, const std::vector<NodeInfo> &value);
  std::vector<NodeInfo> loadFindQueryUndecidedNodes(uint64_t ref_event_id);
  void updateFindQueryUndecidedNodes(uint64_t ref_event_id, const std::vector<NodeInfo> &value);
  void removeFindQueryUndecidedNodes(uint64_t ref_event_id);

  void saveFindQueryPreliminaryResults(uint64_t ref_event_id,
                                       const NodeData::NodesWithAttributes &value);
  NodeData::NodesWithAttributes loadFindQueryPreliminaryResults(uint64_t ref_event_id);
  void addFindQueryPreliminaryResults(uint64_t ref_event_id, const NodeInfo &node,
                                      NodeData::Attributes value);
  void removeFindQueryPreliminaryResults(uint64_t ref_event_id);

  void saveDSNAggregationStartTimestamp(uint64_t ref_event_id, uint64_t value);
  uint64_t loadDSNAggregationStartTimestamp(uint64_t ref_event_id);
  void updateDSNAggregationStartTimestamp(uint64_t ref_event_id, uint64_t value);
  void removeDSNAggregationStartTimestamp(uint64_t ref_event_id);
  std::unordered_map<uint64_t, uint64_t> getDSNAggregationStartTimestampMap() const;

  void saveInquiryAggregationStartTimestamp(uint64_t ref_event_id, uint64_t value);
  uint64_t loadInquiryAggregationStartTimestamp(uint64_t ref_event_id);
  void updateInquiryAggregationStartTimestamp(uint64_t ref_event_id, uint64_t value);
  void removeInquiryAggregationStartTimestamp(uint64_t ref_event_id);
  std::unordered_map<uint64_t, uint64_t> getInquiryAggregationStartTimestampMap() const;

  void saveNumberOfAddressedDSNs(uint64_t ref_event_id, uint16_t value);
  uint16_t loadNumberOfAddressedDSNs(uint64_t ref_event_id);
  void updateNumberOfAddressedDSNs(uint64_t ref_event_id, uint16_t value);
  void removeNumberOfAddressedDSNs(uint64_t ref_event_id);

  void saveNumberOfAnsweredDSNs(uint64_t ref_event_id, uint16_t value);
  uint16_t loadNumberOfAnsweredDSNs(uint64_t ref_event_id);
  void updateNumberOfAnsweredDSNs(uint64_t ref_event_id, uint16_t value);
  void removeNumberOfAnsweredDSNs(uint64_t ref_event_id);

  void saveFindResultPromise(uint64_t ref_event_id, std::promise<FindResult> &&promise);
  std::promise<FindResult> &loadFindResultPromise(uint64_t ref_event_id);
  void removeFindResultPromise(uint64_t ref_event_id);

  bool hasEvent(ProcedureKey key) const;
  bool hasNodeInfo(ProcedureKey key) const;

  bool hasFindQueryEvent(uint64_t ref_event_id) const;
  bool hasFindQueryUndecidedNodes(uint64_t ref_event_id) const;
  bool hasFindQueryPreliminaryResults(uint64_t ref_event_id) const;
  bool hasDSNAggregationStartTimestamp(uint64_t ref_event_id) const;
  bool hasInquiryAggregationStartTimestamp(uint64_t ref_event_id) const;
  bool hasNumberOfAddressedDSNs(uint64_t ref_event_id) const;
  bool hasNumberOfAnsweredDSNs(uint64_t ref_event_id) const;
  bool hasFindResultFuture(uint64_t ref_event_id) const;

private:
  std::unordered_map<ProcedureKey, std::vector<minhton::NodeInfo>> map_;
  std::unordered_map<ProcedureKey, uint64_t> event_ids_;

  // RefEventId -> FindQuery
  std::unordered_map<uint64_t, FindQuery> find_query_requests_;

  // RefEventId -> vector of undecided nodes
  std::unordered_map<uint64_t, std::vector<NodeInfo>> find_query_undecided_nodes_;

  // RefEventId -> vector answered nodes and preliminary answers
  std::unordered_map<uint64_t, NodeData::NodesWithAttributes> find_query_preliminary_results_;

  // RefEventId -> Timestamp
  std::unordered_map<uint64_t, uint64_t> dsn_aggregation_start_timestamp_;

  // RefEventId -> Timestamp
  std::unordered_map<uint64_t, uint64_t> inquiry_aggregation_start_timestamp_;

  // RefEventId -> Number of addressed DSNs
  std::unordered_map<uint64_t, uint16_t> number_of_addressed_dsns_;

  // RefEventId -> Number of answered DSNs
  std::unordered_map<uint64_t, uint16_t> number_of_answered_dsns_;

  std::unordered_map<uint64_t, std::promise<FindResult>> find_result_promises_;
};

}  // namespace minhton

#endif
