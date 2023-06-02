// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_ESEARCH_ENTITY_SEARCH_ALGORITHM_INTERFACE_H_
#define MINHTON_ALGORITHMS_ESEARCH_ENTITY_SEARCH_ALGORITHM_INTERFACE_H_

#include <future>
#include <memory>

#include "minhton/algorithms/algorithm_interface.h"
#include "minhton/algorithms/esearch/find_query.h"
#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/core/definitions.h"
#include "minhton/message/message.h"

namespace minhton {

class EntitySearchAlgorithmInterface : public AlgorithmInterface {
public:
  EntitySearchAlgorithmInterface(std::shared_ptr<AccessContainer> access)
      : AlgorithmInterface(access){};
  virtual void process(const MessageVariant &msg) override = 0;

  virtual ~EntitySearchAlgorithmInterface(){};

  virtual std::future<FindResult> find(FindQuery query) = 0;

  virtual void localInsert(std::vector<Entry> entries) = 0;
  virtual void localUpdate(std::vector<Entry> entries) = 0;
  virtual void localRemove(std::vector<std::string> keys) = 0;

  virtual void processTimeout(const TimeoutType &type) = 0;

  static std::vector<minhton::MessageType> getSupportedMessageTypes() {
    return {
        MessageType::kAttributeInquiryAnswer, MessageType::kAttributeInquiryRequest,
        MessageType::kFindQueryAnswer,        MessageType::kFindQueryRequest,
        MessageType::kSubscriptionOrder,      MessageType::kSubscriptionUpdate,
        MessageType::kSearchExactFailure  // only for now
    };
  }
};

}  // namespace minhton

#endif
