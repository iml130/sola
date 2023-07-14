// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef ALGORITHMS_ESEARCH_DISTRIBUTED_DATA_H_
#define ALGORITHMS_ESEARCH_DISTRIBUTED_DATA_H_

#include <cstdint>
#include <queue>
#include <unordered_map>
#include <vector>

#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/core/physical_node_info.h"

namespace minhton {

class DistributedData : public NodeData {
public:
  DistributedData() = default;
  explicit DistributedData(const PhysicalNodeInfo &p_node_info);

  void setPhysicalNodeInfo(const PhysicalNodeInfo &p_node_info);
  PhysicalNodeInfo getPhysicalNodeInfo() const;

  bool insert(Key key, NodeData::ValueTimestampAndType value_timestamp_and_type) override;
  bool update(Key key, NodeData::ValueTimestampAndType value_timestamp_and_type) override;
  void remove(Key key) override;

  void addSubscriptionOrderKey(NodeData::Key key);
  void removeSubscriptionOrderKey(NodeData::Key key);
  std::vector<NodeData::Key> getSubscriptionOrderKeys() const;

  std::queue<uint64_t> getUpdateTimestamps(const NodeData::Key &key);

  bool isKeySubscribed(NodeData::Key key);

  bool isValueUpToDate(const NodeData::Key &key, uint64_t validity_threshold_timestamp) override;

  uint8_t getTimestampStorageLimit() const;

  bool isLocal() const override;

private:
  PhysicalNodeInfo p_node_info_;

  // for which keys we have sent a subscription order
  std::vector<NodeData::Key> subscription_ordered_keys_;

  // to see how often we get updates
  std::unordered_map<NodeData::Key, std::queue<uint64_t>> update_timestamps_;

  // how many items we store in update timestamps
  uint8_t timestamp_storage_limit_ = 5;
};

}  // namespace minhton

#endif
