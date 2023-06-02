// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/algorithms/esearch/node_data.h"

#include <algorithm>
#include <stdexcept>

namespace minhton {

NodeData::NodeData() { this->data_ = std::unordered_map<Key, ValueTimestampAndType>(); }

bool NodeData::insert(Key key, NodeData::ValueTimestampAndType value_timestamp_and_type) {
  if (hasKey(key)) {
    return false;
  }
  bool inserted = data_.insert({key, value_timestamp_and_type}).second;
  return inserted;
}

bool NodeData::update(Key key, NodeData::ValueTimestampAndType value_timestamp_and_type) {
  auto it = data_.find(key);
  if (it != data_.end()) {
    if (std::get<2>(it->second) != std::get<2>(value_timestamp_and_type)) {
      throw std::invalid_argument("value types need to stay the same");
    }

    if (std::get<2>(it->second) == ValueType::kValueStatic) {
      if (std::get<0>(it->second) == std::get<0>(value_timestamp_and_type)) {
        // values didnt change, so its okay
        return true;
      }
      throw std::invalid_argument("static values cannot get updated");
    }

    it->second = value_timestamp_and_type;
    return true;
  }
  return false;
}

void NodeData::remove(Key key) { data_.erase(key); }

bool NodeData::hasKey(const Key &key) { return data_.find(key) != data_.end(); }

NodeData::ValueTimestampAndType NodeData::getValueTimestampAndType(Key key) { return data_[key]; }

NodeData::ValueAndTimestamp NodeData::getValueAndTimestamp(Key key) {
  auto value_timestamp_and_type = data_[key];
  return {std::get<0>(value_timestamp_and_type), std::get<1>(value_timestamp_and_type)};
}

NodeData::Value NodeData::getValue(Key key) { return std::get<0>(data_.at(key)); }

std::unordered_map<NodeData::Key, NodeData::ValueTimestampAndType> NodeData::getData() {
  return data_;
}

bool NodeData::isValueUpToDate(NodeData::Key key,
                               [[maybe_unused]] const uint64_t &validity_threshold_timestamp) {
  return hasKey(key);
}

std::vector<NodeData::Key> NodeData::getAllCurrentKeys() {
  std::vector<NodeData::Key> keys;

  std::transform(data_.begin(), data_.end(), std::back_inserter(keys),
                 [](const std::tuple<NodeData::Key, NodeData::ValueTimestampAndType> &tupl) {
                   return std::get<0>(tupl);
                 });

  return keys;
}

}  // namespace minhton
