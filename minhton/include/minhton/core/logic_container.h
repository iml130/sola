// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_LOGIC_CONTAINER_H_
#define MINHTON_CORE_LOGIC_CONTAINER_H_

#include <future>
#include <memory>

#include "minhton/algorithms/algorithm_interface.h"
#include "minhton/algorithms/esearch/interface_entity_search_algorithm.h"
#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/algorithms/join/interface_join_algorithm.h"
#include "minhton/algorithms/leave/interface_leave_algorithm.h"
#include "minhton/algorithms/misc/interface_bootstrap_algorithm.h"
#include "minhton/algorithms/misc/interface_response_algorithm.h"
#include "minhton/algorithms/search_exact/interface_search_exact_algorithm.h"
#include "minhton/core/access_container.h"
#include "minhton/core/constants.h"
#include "minhton/core/definitions.h"
#include "minhton/message/message.h"
#include "minhton/utils/algorithm_types_container.h"

namespace minhton {

class LogicContainer {
public:
  LogicContainer() = default;
  explicit LogicContainer(std::shared_ptr<AccessContainer> access,
                          const AlgorithmTypesContainer &types_container);

  void process(const MessageVariant &msg);
  void processSignal(Signal &signal);
  void processTimeout(const Timeout &timeout_event);

  bool isBootstrapResponseValid() const;
  bool canLeaveWithoutReplacement() const;

  void performSearchExactTest(const minhton::NodeInfo &destination,
                              std::shared_ptr<minhton::MessageSEVariant> query);

  std::future<FindResult> find(const FindQuery &query);

  void localInsert(const std::vector<Entry> &entries);
  void localUpdate(const std::vector<Entry> &entries);
  void localRemove(const std::vector<std::string> &keys);

private:
  std::shared_ptr<JoinAlgorithmInterface> join_algo_;
  std::shared_ptr<LeaveAlgorithmInterface> leave_algo_;
  std::shared_ptr<SearchExactAlgorithmInterface> search_exact_algo_;
  std::shared_ptr<ResponseAlgorithmInterface> response_algo_;
  std::shared_ptr<BootstrapAlgorithmInterface> bootstrap_algo_;
  std::shared_ptr<EntitySearchAlgorithmInterface> entity_search_algo_;

  std::unordered_map<minhton::MessageType, std::shared_ptr<AlgorithmInterface>>
      message_type_to_algorithm_map_;

  void associateMessageTypesWithAlgorithm(std::vector<minhton::MessageType> types,
                                          std::shared_ptr<AlgorithmInterface> algorithm);
};

}  // namespace minhton

#endif
