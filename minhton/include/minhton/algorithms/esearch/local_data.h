// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef ALGORITHMS_ESEARCH_LOCAL_DATA_H_
#define ALGORITHMS_ESEARCH_LOCAL_DATA_H_

#include "minhton/algorithms/esearch/node_data.h"

namespace minhton {

class LocalData : public NodeData {
public:
  std::vector<NodeInfo> getSubscribers(const NodeData::Key &key);

  void addKeySubscriber(NodeData::Key key, NodeInfo &subscriber);
  void removeKeySubscriber(const NodeData::Key &key, NodeInfo &unsubscriber);

  bool isValueUpToDate(NodeData::Key key, const uint64_t &validity_threshold_timestamp) override;

  bool isLocal() const override;

private:
  std::unordered_map<NodeData::Key, std::vector<NodeInfo>> key_subscribers_;
};

}  // namespace minhton

#endif
