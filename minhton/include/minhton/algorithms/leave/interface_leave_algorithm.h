// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_LEAVE_LEAVE_ALGORITHM_INTERFACE_H_
#define MINHTON_ALGORITHMS_LEAVE_LEAVE_ALGORITHM_INTERFACE_H_

#include <memory>

#include "minhton/algorithms/algorithm_interface.h"
#include "minhton/core/physical_node_info.h"
#include "minhton/message/message.h"

namespace minhton {

class LeaveAlgorithmInterface : public AlgorithmInterface {
public:
  LeaveAlgorithmInterface(std::shared_ptr<AccessContainer> access) : AlgorithmInterface(access){};

  ~LeaveAlgorithmInterface() override = default;

  void process(const MessageVariant &msg) override = 0;
  virtual void initiateSelfDeparture() = 0;
  virtual bool canLeaveWithoutReplacement() = 0;

  virtual void replaceMyself(const NodeInfo &node_to_replace,
                             std::vector<NodeInfo> neighbors_of_node_to_replace) = 0;

  static std::vector<MessageType> getSupportedMessageTypes() {
    return {
        MessageType::kFindReplacement,      MessageType::kReplacementAck,
        MessageType::kReplacementNack,      MessageType::kReplacementOffer,
        MessageType::kReplacementUpdate,    MessageType::kSignOffParentRequest,
        MessageType::kSignOffParentAnswer,  MessageType::kLockNeighborRequest,
        MessageType::kLockNeighborResponse, MessageType::kUnlockNeighbor,
    };
  }
};

}  // namespace minhton

#endif
