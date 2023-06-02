// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/utils/procedure_info.h"

#include <stdexcept>

#include "minhton/exception/algorithm_exception.h"
using namespace std;
namespace minhton {

ProcedureInfo::ProcedureInfo() {}

ProcedureInfo::~ProcedureInfo() {}

bool ProcedureInfo::hasEvent(ProcedureKey key) const {
  return this->event_ids_.find(key) != this->event_ids_.end();
};
bool ProcedureInfo::hasNodeInfo(ProcedureKey key) const {
  return this->map_.find(key) != this->map_.end();
};
bool ProcedureInfo::hasFindQueryEvent(uint64_t ref_event_id) const {
  return this->find_query_requests_.find(ref_event_id) != this->find_query_requests_.end();
};

bool ProcedureInfo::hasFindQueryUndecidedNodes(uint64_t ref_event_id) const {
  return this->find_query_undecided_nodes_.find(ref_event_id) !=
         this->find_query_undecided_nodes_.end();
}

bool ProcedureInfo::hasFindQueryPreliminaryResults(uint64_t ref_event_id) const {
  return this->find_query_preliminary_results_.find(ref_event_id) !=
         this->find_query_preliminary_results_.end();
}

bool ProcedureInfo::hasDSNAggregationStartTimestamp(uint64_t ref_event_id) const {
  return this->dsn_aggregation_start_timestamp_.find(ref_event_id) !=
         this->dsn_aggregation_start_timestamp_.end();
}

bool ProcedureInfo::hasInquiryAggregationStartTimestamp(uint64_t ref_event_id) const {
  return this->inquiry_aggregation_start_timestamp_.find(ref_event_id) !=
         this->inquiry_aggregation_start_timestamp_.end();
}

bool ProcedureInfo::hasNumberOfAddressedDSNs(uint64_t ref_event_id) const {
  return this->number_of_addressed_dsns_.find(ref_event_id) !=
         this->number_of_addressed_dsns_.end();
}

bool ProcedureInfo::hasNumberOfAnsweredDSNs(uint64_t ref_event_id) const {
  return this->number_of_answered_dsns_.find(ref_event_id) != this->number_of_answered_dsns_.end();
}

bool ProcedureInfo::hasFindResultFuture(uint64_t ref_event_id) const {
  return this->find_result_promises_.find(ref_event_id) != this->find_result_promises_.end();
}

bool ProcedureInfo::hasKey(ProcedureKey key) const { return hasEvent(key) || hasNodeInfo(key); }

void ProcedureInfo::save(ProcedureKey key,
                         const std::vector<minhton::NodeInfo> &value) noexcept(false) {
  if (this->hasKey(key)) {
    throw AlgorithmException("Key already exists. You must wait until it has been removed.");
  }

  this->map_[key] = value;
}

void ProcedureInfo::saveEventId(ProcedureKey key, uint64_t event_id) noexcept(false) {
  if (this->hasKey(key)) {
    throw AlgorithmException("Key already exists. You must wait until it has been removed.");
  }

  this->event_ids_[key] = event_id;
}

void ProcedureInfo::update(ProcedureKey key,
                           const std::vector<minhton::NodeInfo> &value) noexcept(false) {
  if (!this->hasKey(key)) {
    throw AlgorithmException("Key does not exist. You cannot update a non-existent entry.");
  }

  this->map_[key] = value;
}

std::vector<minhton::NodeInfo> ProcedureInfo::load(ProcedureKey key) noexcept(false) {
  if (!this->hasKey(key)) {
    throw AlgorithmException("Key does not exist. You cannot load a non-existent entry.");
  }

  return this->map_[key];
}

uint64_t ProcedureInfo::loadEventId(ProcedureKey key) noexcept(false) {
  if (!this->hasKey(key)) {
    throw AlgorithmException("Key does not exist. You cannot load a non-existent entry.");
  }

  return this->event_ids_[key];
}

void ProcedureInfo::remove(ProcedureKey key) noexcept(false) {
  if (!this->hasKey(key)) {
    throw AlgorithmException("Key does not exist. You cannot remove a non-existent entry.");
  }

  this->map_.erase(key);
}

void ProcedureInfo::removeEventId(ProcedureKey key) noexcept(false) {
  if (!this->hasKey(key)) {
    throw AlgorithmException("Key does not exist. You cannot remove a non-existent entry.");
  }

  this->event_ids_.erase(key);
}

void ProcedureInfo::saveFindQuery(uint64_t ref_event_id, const FindQuery &value) noexcept(false) {
  if (this->hasFindQueryEvent(ref_event_id)) {
    throw AlgorithmException("Event already exists. Extremely unlikely.");
  }

  this->find_query_requests_[ref_event_id] = value;
}

FindQuery ProcedureInfo::loadFindQuery(uint64_t ref_event_id) noexcept(false) {
  if (!this->hasFindQueryEvent(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot load a non-existent entry.");
  }

  return this->find_query_requests_[ref_event_id];
}

void ProcedureInfo::updateFindQuery(uint64_t ref_event_id, const FindQuery &value) noexcept(false) {
  if (!this->hasFindQueryEvent(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot update a non-existent entry.");
  }

  this->find_query_requests_[ref_event_id] = value;
}

void ProcedureInfo::removeFindQuery(uint64_t ref_event_id) noexcept(false) {
  if (!this->hasFindQueryEvent(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot remove a non-existent entry.");
  }

  this->find_query_requests_.erase(ref_event_id);
}

void ProcedureInfo::saveFindQueryUndecidedNodes(
    uint64_t ref_event_id, const std::vector<NodeInfo> &value) noexcept(false) {
  if (this->hasFindQueryUndecidedNodes(ref_event_id)) {
    throw AlgorithmException("Event already exists. Extremely unlikely.");
  }

  this->find_query_undecided_nodes_[ref_event_id] = value;
}

std::vector<NodeInfo> ProcedureInfo::loadFindQueryUndecidedNodes(uint64_t ref_event_id) noexcept(
    false) {
  if (!this->hasFindQueryUndecidedNodes(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot load a non-existent entry.");
  }

  return this->find_query_undecided_nodes_[ref_event_id];
}

void ProcedureInfo::updateFindQueryUndecidedNodes(
    uint64_t ref_event_id, const std::vector<NodeInfo> &value) noexcept(false) {
  if (!this->hasFindQueryUndecidedNodes(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot update a non-existent entry.");
  }

  this->find_query_undecided_nodes_[ref_event_id] = value;
}

void ProcedureInfo::removeFindQueryUndecidedNodes(uint64_t ref_event_id) noexcept(false) {
  if (!this->hasFindQueryUndecidedNodes(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot remove a non-existent entry.");
  }

  this->find_query_undecided_nodes_.erase(ref_event_id);
}

void ProcedureInfo::saveFindQueryPreliminaryResults(
    uint64_t ref_event_id, const NodeData::NodesWithAttributes &value) noexcept(false) {
  if (this->hasFindQueryPreliminaryResults(ref_event_id)) {
    throw AlgorithmException("Event already exists. Extremely unlikely.");
  }

  this->find_query_preliminary_results_[ref_event_id] = value;
}

NodeData::NodesWithAttributes ProcedureInfo::loadFindQueryPreliminaryResults(
    uint64_t ref_event_id) noexcept(false) {
  if (!this->hasFindQueryPreliminaryResults(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot load a non-existent entry.");
  }

  return this->find_query_preliminary_results_[ref_event_id];
}

void ProcedureInfo::addFindQueryPreliminaryResults(uint64_t ref_event_id, const NodeInfo &node,
                                                   NodeData::Attributes value) noexcept(false) {
  if (!this->hasFindQueryPreliminaryResults(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot update a non-existent entry.");
  }

  if (this->find_query_preliminary_results_[ref_event_id].find(node) ==
      this->find_query_preliminary_results_[ref_event_id].end()) {
    this->find_query_preliminary_results_[ref_event_id][node] = value;
  } else {
    this->find_query_preliminary_results_[ref_event_id][node].insert(
        this->find_query_preliminary_results_[ref_event_id][node].end(), value.begin(),
        value.end());
  }
}

void ProcedureInfo::removeFindQueryPreliminaryResults(uint64_t ref_event_id) noexcept(false) {
  if (!this->hasFindQueryPreliminaryResults(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot remove a non-existent entry.");
  }

  this->find_query_preliminary_results_.erase(ref_event_id);
}

void ProcedureInfo::saveDSNAggregationStartTimestamp(uint64_t ref_event_id, uint64_t value) {
  if (this->hasDSNAggregationStartTimestamp(ref_event_id)) {
    throw AlgorithmException("Event already exists. Extremely unlikely.");
  }

  this->dsn_aggregation_start_timestamp_[ref_event_id] = value;
}

uint64_t ProcedureInfo::loadDSNAggregationStartTimestamp(uint64_t ref_event_id) {
  if (!this->hasDSNAggregationStartTimestamp(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot load a non-existent entry.");
  }

  return this->dsn_aggregation_start_timestamp_[ref_event_id];
}

void ProcedureInfo::updateDSNAggregationStartTimestamp(uint64_t ref_event_id, uint64_t value) {
  if (!this->hasDSNAggregationStartTimestamp(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot update a non-existent entry.");
  }

  this->dsn_aggregation_start_timestamp_[ref_event_id] = value;
}

void ProcedureInfo::removeDSNAggregationStartTimestamp(uint64_t ref_event_id) {
  if (!this->hasDSNAggregationStartTimestamp(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot remove a non-existent entry.");
  }

  this->dsn_aggregation_start_timestamp_.erase(ref_event_id);
}

std::unordered_map<uint64_t, uint64_t> ProcedureInfo::getDSNAggregationStartTimestampMap() const {
  return dsn_aggregation_start_timestamp_;
}

void ProcedureInfo::saveInquiryAggregationStartTimestamp(uint64_t ref_event_id, uint64_t value) {
  if (this->hasInquiryAggregationStartTimestamp(ref_event_id)) {
    throw AlgorithmException("Event already exists. Extremely unlikely.");
  }

  this->inquiry_aggregation_start_timestamp_[ref_event_id] = value;
}

uint64_t ProcedureInfo::loadInquiryAggregationStartTimestamp(uint64_t ref_event_id) {
  if (!this->hasInquiryAggregationStartTimestamp(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot load a non-existent entry.");
  }

  return this->inquiry_aggregation_start_timestamp_[ref_event_id];
}

void ProcedureInfo::updateInquiryAggregationStartTimestamp(uint64_t ref_event_id, uint64_t value) {
  if (!this->hasInquiryAggregationStartTimestamp(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot update a non-existent entry.");
  }

  this->inquiry_aggregation_start_timestamp_[ref_event_id] = value;
}

void ProcedureInfo::removeInquiryAggregationStartTimestamp(uint64_t ref_event_id) {
  if (!this->hasInquiryAggregationStartTimestamp(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot remove a non-existent entry.");
  }

  this->inquiry_aggregation_start_timestamp_.erase(ref_event_id);
}

std::unordered_map<uint64_t, uint64_t> ProcedureInfo::getInquiryAggregationStartTimestampMap()
    const {
  return inquiry_aggregation_start_timestamp_;
}

void ProcedureInfo::saveNumberOfAddressedDSNs(uint64_t ref_event_id, uint16_t value) {
  if (this->hasNumberOfAddressedDSNs(ref_event_id)) {
    throw AlgorithmException("Event already exists. Extremely unlikely.");
  }

  this->number_of_addressed_dsns_[ref_event_id] = value;
}

uint16_t ProcedureInfo::loadNumberOfAddressedDSNs(uint64_t ref_event_id) {
  if (!this->hasNumberOfAddressedDSNs(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot load a non-existent entry.");
  }

  return this->number_of_addressed_dsns_[ref_event_id];
}

void ProcedureInfo::updateNumberOfAddressedDSNs(uint64_t ref_event_id, uint16_t value) {
  if (!this->hasNumberOfAddressedDSNs(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot update a non-existent entry.");
  }

  this->number_of_addressed_dsns_[ref_event_id] = value;
}

void ProcedureInfo::removeNumberOfAddressedDSNs(uint64_t ref_event_id) {
  if (!this->hasNumberOfAddressedDSNs(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot remove a non-existent entry.");
  }

  this->number_of_addressed_dsns_.erase(ref_event_id);
}

void ProcedureInfo::saveNumberOfAnsweredDSNs(uint64_t ref_event_id, uint16_t value) {
  if (this->hasNumberOfAnsweredDSNs(ref_event_id)) {
    throw AlgorithmException("Event already exists. Extremely unlikely.");
  }

  this->number_of_answered_dsns_[ref_event_id] = value;
}

uint16_t ProcedureInfo::loadNumberOfAnsweredDSNs(uint64_t ref_event_id) {
  if (!this->hasNumberOfAnsweredDSNs(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot load a non-existent entry.");
  }

  return this->number_of_answered_dsns_[ref_event_id];
}

void ProcedureInfo::updateNumberOfAnsweredDSNs(uint64_t ref_event_id, uint16_t value) {
  if (!this->hasNumberOfAnsweredDSNs(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot update a non-existent entry.");
  }

  this->number_of_answered_dsns_[ref_event_id] = value;
}

void ProcedureInfo::removeNumberOfAnsweredDSNs(uint64_t ref_event_id) {
  if (!this->hasNumberOfAnsweredDSNs(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot remove a non-existent entry.");
  }

  this->number_of_answered_dsns_.erase(ref_event_id);
}

void ProcedureInfo::saveFindResultPromise(uint64_t ref_event_id,
                                          std::promise<FindResult> &&promise) {
  if (this->hasFindResultFuture(ref_event_id)) {
    throw AlgorithmException("Event already exists. Extremely unlikely.");
  }

  this->find_result_promises_[ref_event_id] = std::move(promise);
}

std::promise<FindResult> &ProcedureInfo::loadFindResultPromise(uint64_t ref_event_id) {
  if (!this->hasFindResultFuture(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot load a non-existent entry.");
  }

  return this->find_result_promises_[ref_event_id];
}

void ProcedureInfo::removeFindResultPromise(uint64_t ref_event_id) {
  if (!this->hasFindResultFuture(ref_event_id)) {
    throw AlgorithmException("Event does not exist. You cannot remove a non-existent entry.");
  }

  this->find_result_promises_.erase(ref_event_id);
}

}  // namespace minhton
