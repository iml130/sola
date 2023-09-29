// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_SEARCHEXACT_SEARCH_EXACT_ALGORITHM_INTERFACE_H_
#define MINHTON_ALGORITHMS_SEARCHEXACT_SEARCH_EXACT_ALGORITHM_INTERFACE_H_

#include <memory>

#include "minhton/algorithms/algorithm_interface.h"
#include "minhton/core/physical_node_info.h"
#include "minhton/message/message.h"
#include "minhton/message/se_types.h"

namespace minhton {

class SearchExactAlgorithmInterface : public AlgorithmInterface {
public:
  explicit SearchExactAlgorithmInterface(std::shared_ptr<AccessContainer> access)
      : AlgorithmInterface(access){};

  ~SearchExactAlgorithmInterface() override = default;

  void process(const MessageVariant &msg) override = 0;
  virtual void performSearchExact(const minhton::NodeInfo &destination,
                                  std::shared_ptr<MessageSEVariant> query) = 0;

  static std::vector<minhton::MessageType> getSupportedMessageTypes() {
    return {
        MessageType::kSearchExact,
        MessageType::kEmpty,
    };
  }
};

}  // namespace minhton

#endif
