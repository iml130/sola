// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/join.h"

#include <string>

namespace minhton {

MessageJoin::MessageJoin(MinhtonMessageHeader header, minhton::NodeInfo entering_node,
                         SearchProgress search_progress)
    : header_(std::move(header)),
      entering_node_(std::move(entering_node)),
      search_progress_(search_progress) {
  header_.setMessageType(MessageType::kJoin);
  MessageLoggingAdditionalInfo logging_info{getEnteringNode().getLogicalNodeInfo().getUuid(), "",
                                            "Prog: " + std::to_string(search_progress)};
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageJoin::validateImpl() const {
  return getEnteringNode().getPhysicalNodeInfo().isInitialized();
}

minhton::NodeInfo MessageJoin::getEnteringNode() const { return entering_node_; }

SearchProgress MessageJoin::getSearchProgress() const { return search_progress_; }

}  // namespace minhton
