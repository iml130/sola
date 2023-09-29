// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ESEARCH_NODE_DATA_H_
#define MINHTON_ESEARCH_NODE_DATA_H_

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "minhton/core/node_info.h"
#include "solanet/serializer/serialize.h"

namespace minhton {

class NodeData {
public:
  enum ValueType : uint8_t {
    kValueDynamic = 0,
    kValueStatic = 1,
  };

  using Key = std::string;
  using Value = std::variant<int, float, bool, std::string>;
  using ValueAndTimestamp = std::tuple<Value, uint64_t>;
  using ValueTimestampAndType = std::tuple<Value, uint64_t, ValueType>;
  using ValueAndType = std::tuple<Value, ValueType>;

  using Attributes = std::vector<std::tuple<NodeData::Key, NodeData::Value>>;
  using NodesWithAttributes = std::unordered_map<NodeInfo, Attributes, NodeInfoHasher>;

  using AttributesAndTypes =
      std::vector<std::tuple<NodeData::Key, NodeData::Value, NodeData::ValueType>>;
  using NodesWithAttributesAndTypes =
      std::unordered_map<NodeInfo, AttributesAndTypes, NodeInfoHasher>;

  NodeData();

  virtual bool insert(Key key, ValueTimestampAndType value_timestamp_and_type);
  virtual bool update(Key key, ValueTimestampAndType value_timestamp_and_type);
  virtual void remove(Key key);

  bool hasKey(const Key &key);
  Value getValue(Key key);
  ValueAndTimestamp getValueAndTimestamp(Key key);
  ValueTimestampAndType getValueTimestampAndType(Key key);

  std::unordered_map<Key, ValueTimestampAndType> getData();

  virtual bool isValueUpToDate(const NodeData::Key &key, uint64_t validity_threshold_timestamp);

  virtual bool isLocal() const = 0;

  std::vector<Key> getAllCurrentKeys();

  SERIALIZE(data_);

private:
  std::unordered_map<Key, ValueTimestampAndType> data_;
};

}  // namespace minhton

#endif
