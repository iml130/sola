// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/esearch/distributed_data.h"

#include <algorithm>
#include <stdexcept>

namespace minhton {

DistributedData::DistributedData(const PhysicalNodeInfo &p_node_info) {
  setPhysicalNodeInfo(p_node_info);
  this->timestamp_storage_limit_ = 5;
}

void DistributedData::setPhysicalNodeInfo(const PhysicalNodeInfo &p_node_info) {
  p_node_info_ = p_node_info;
}

PhysicalNodeInfo DistributedData::getPhysicalNodeInfo() const { return p_node_info_; }

bool DistributedData::insert(NodeData::Key key,
                             NodeData::ValueTimestampAndType value_timestamp_and_type) {
  bool data_inserted = NodeData::insert(key, value_timestamp_and_type);
  auto update_timestamp = std::get<1>(value_timestamp_and_type);

  std::queue<uint64_t> update_queue;
  update_queue.push(update_timestamp);

  bool update_inserted = update_timestamps_.insert({key, update_queue}).second;

  return data_inserted && update_inserted;
}

bool DistributedData::update(NodeData::Key key,
                             NodeData::ValueTimestampAndType value_timestamp_and_type) {
  bool data_updated = NodeData::update(key, value_timestamp_and_type);
  if (!data_updated) {
    return false;
  }

  auto update_timestamp = std::get<1>(value_timestamp_and_type);

  bool timestamps_updated = false;
  auto it = update_timestamps_.find(key);
  if (it != update_timestamps_.end()) {
    uint64_t latest_timestamp = it->second.back();
    if (latest_timestamp > update_timestamp) {
      throw std::invalid_argument("The new update timestamp cannot be from earlier");
    }

    it->second.push(update_timestamp);
    timestamps_updated = true;

    if (it->second.size() > this->timestamp_storage_limit_) {
      it->second.pop();
    }
  }

  return timestamps_updated;
}

void DistributedData::remove(NodeData::Key key) {
  NodeData::remove(key);
  update_timestamps_.erase(key);
}

void DistributedData::addSubscriptionOrderKey(NodeData::Key key) {
  auto it = std::find(subscription_ordered_keys_.begin(), subscription_ordered_keys_.end(), key);
  if (it == subscription_ordered_keys_.end()) {
    subscription_ordered_keys_.push_back(key);
  }
}

void DistributedData::removeSubscriptionOrderKey(NodeData::Key key) {
  auto it = std::find(subscription_ordered_keys_.begin(), subscription_ordered_keys_.end(), key);
  if (it != subscription_ordered_keys_.end()) {
    subscription_ordered_keys_.erase(it);
  }
}

std::vector<NodeData::Key> DistributedData::getSubscriptionOrderKeys() const {
  return subscription_ordered_keys_;
}

std::queue<uint64_t> DistributedData::getUpdateTimestamps(const NodeData::Key &key) {
  auto it = update_timestamps_.find(key);
  if (it != update_timestamps_.end()) {
    return it->second;
  }
  return {};
}

bool DistributedData::isKeySubscribed(NodeData::Key key) {
  return std::find(subscription_ordered_keys_.begin(), subscription_ordered_keys_.end(), key) !=
         subscription_ordered_keys_.end();
}

bool DistributedData::isValueUpToDate(NodeData::Key key,
                                      const uint64_t &validity_threshold_timestamp) {
  if (!hasKey(key)) {
    throw std::logic_error("This method should not be called if the key is unknown");
  }

  if (isKeySubscribed(key)) {
    return true;
  }

  auto value_timestamp_and_type = getValueTimestampAndType(key);
  auto timestamp = std::get<1>(value_timestamp_and_type);
  auto type = std::get<2>(value_timestamp_and_type);

  if (type == ValueType::kValueStatic) {
    return true;
  }

  // false if timestamp is older than allowed
  return timestamp >= validity_threshold_timestamp;
}

uint8_t DistributedData::getTimestampStorageLimit() const { return this->timestamp_storage_limit_; }

bool DistributedData::isLocal() const { return false; }

}  // namespace minhton
