// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_JOIN_JOIN_ALGORITHM_INTERFACE_H_
#define MINHTON_ALGORITHMS_JOIN_JOIN_ALGORITHM_INTERFACE_H_

#include <memory>

#include "minhton/algorithms/algorithm_interface.h"
#include "minhton/core/physical_node_info.h"
#include "minhton/message/message.h"

namespace minhton {

class JoinAlgorithmInterface : public AlgorithmInterface {
public:
  explicit JoinAlgorithmInterface(std::shared_ptr<AccessContainer> access)
      : AlgorithmInterface(access){};

  ~JoinAlgorithmInterface() override = default;

  void process(const MessageVariant &msg) override = 0;
  virtual void initiateJoin(NodeInfo &node_info) = 0;
  virtual void initiateJoin(const PhysicalNodeInfo &p_node_info) = 0;

  virtual void continueAcceptChildProcedure(const MessageInformAboutNeighbors &message) noexcept(
      false) = 0;

  static std::vector<MessageType> getSupportedMessageTypes() {
    return {MessageType::kJoin, MessageType::kJoinAccept, MessageType::kJoinAcceptAck};
  }
};

}  // namespace minhton

#endif
