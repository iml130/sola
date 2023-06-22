// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/find_replacement.h"

#include <string>

namespace minhton {
MessageFindReplacement::MessageFindReplacement(const MinhtonMessageHeader &header,
                                               NodeInfo node_to_replace,
                                               SearchProgress search_progress)
    : header_(header),
      node_to_replace_(std::move(node_to_replace)),
      search_progress_(search_progress) {
  header_.setMessageType(MessageType::kFindReplacement);
  MessageLoggingAdditionalInfo logging_info{getNodeToReplace().getLogicalNodeInfo().getUuid(), "",
                                            "Prog: " + std::to_string(search_progress)};
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageFindReplacement::validateImpl() const {
  return this->getNodeToReplace().isInitialized();
}

minhton::NodeInfo MessageFindReplacement::getNodeToReplace() const {
  return this->node_to_replace_;
}

SearchProgress MessageFindReplacement::getSearchProgress() const { return this->search_progress_; }

}  // namespace minhton
