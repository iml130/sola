// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_MISC_RESPONSE_ALGORITHM_INTERFACE_H_
#define MINHTON_ALGORITHMS_MISC_RESPONSE_ALGORITHM_INTERFACE_H_

#include <functional>
#include <memory>

#include "minhton/algorithms/algorithm_interface.h"
#include "minhton/message/message.h"

namespace minhton {

class ResponseAlgorithmInterface : public AlgorithmInterface {
public:
  ResponseAlgorithmInterface(std::shared_ptr<AccessContainer> access)
      : AlgorithmInterface(access){};
  virtual void process(const MessageVariant &msg) override = 0;
  virtual void waitForAcks(uint32_t number, std::function<void()> cb) = 0;

  virtual ~ResponseAlgorithmInterface(){};

  static std::vector<minhton::MessageType> getSupportedMessageTypes() {
    return {
        MessageType::kUpdateNeighbors,         MessageType::kGetNeighbors,
        MessageType::kRemoveNeighbor,          MessageType::kInformAboutNeighbors,
        MessageType::kRemoveAndUpdateNeighbor, MessageType::kRemoveNeighborAck,
    };
  }

protected:
  uint32_t number_ = 0;
  std::function<void()> cb_;
};

}  // namespace minhton

#endif
