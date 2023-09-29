// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_MISC_BOOTSTRAP_ALGORITHM_INTERFACE_H_
#define MINHTON_ALGORITHMS_MISC_BOOTSTRAP_ALGORITHM_INTERFACE_H_

#include <memory>

#include "minhton/algorithms/algorithm_interface.h"
#include "minhton/message/message.h"

namespace minhton {

class BootstrapAlgorithmInterface : public AlgorithmInterface {
public:
  explicit BootstrapAlgorithmInterface(std::shared_ptr<AccessContainer> access)
      : AlgorithmInterface(access){};

  ~BootstrapAlgorithmInterface() override = default;

  void process(const MessageVariant &msg) override = 0;
  virtual void initiateJoin(const PhysicalNodeInfo &p_node_info) = 0;

  virtual void processBootstrapResponseTimeout() = 0;
  virtual bool isBootstrapResponseValid() const = 0;

  static std::vector<minhton::MessageType> getSupportedMessageTypes() {
    return {MessageType::kBootstrapDiscover, MessageType::kBootstrapResponse};
  }
};

}  // namespace minhton

#endif
