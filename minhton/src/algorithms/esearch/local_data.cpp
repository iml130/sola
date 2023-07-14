// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/esearch/local_data.h"

#include <algorithm>

namespace minhton {

std::vector<NodeInfo> LocalData::getSubscribers(const NodeData::Key &key) {
  auto it = key_subscribers_.find(key);
  if (it != key_subscribers_.end()) {
    return it->second;
  }
  return {};
}

void LocalData::addKeySubscriber(NodeData::Key key, NodeInfo &subscriber) {
  auto it = key_subscribers_.find(key);
  if (it != key_subscribers_.end()) {
    if (std::find(it->second.begin(), it->second.end(), subscriber) == it->second.end()) {
      // not added yet
      it->second.push_back(subscriber);
    }
  } else {
    key_subscribers_[key] = {subscriber};
  }
}

void LocalData::removeKeySubscriber(const NodeData::Key &key, NodeInfo &unsubscriber) {
  auto it = key_subscribers_.find(key);
  if (it != key_subscribers_.end()) {
    it->second.erase(std::remove(it->second.begin(), it->second.end(), unsubscriber),
                     it->second.end());
  }
}

bool LocalData::isValueUpToDate(const NodeData::Key &key,
                                [[maybe_unused]] uint64_t validity_threshold_timestamp) {
  return hasKey(key);
}

bool LocalData::isLocal() const { return true; }

}  // namespace minhton
