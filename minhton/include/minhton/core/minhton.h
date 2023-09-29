// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_MINHTON_H_
#define MINHTON_CORE_MINHTON_H_

#include <functional>
#include <future>

#include "minhton/core/connection_info.h"
#include "minhton/core/node.h"

// High level
namespace minhton {

enum class State { kStarted, kConnected, kIdle, kError };

class Minhton {
public:
  Minhton(NeighborCallbackFct fct, const ConfigNode &config);
  ~Minhton() = default;

  using Entry = std::tuple<std::string, std::variant<int, float, bool, std::string>,
                           minhton::NodeData::ValueType>;

  void insert(const std::vector<Entry> &data);
  void update(const std::vector<Entry> &data);
  void remove(const std::vector<std::string> &keys);
  std::future<FindResult> find(const FindQuery &query);

  State getState();

  // DEBUG ONLY
  MinhtonNode &getNode() { return node_; }

  void stop();

private:
  MinhtonNode node_;
};
}  // namespace minhton

#endif
