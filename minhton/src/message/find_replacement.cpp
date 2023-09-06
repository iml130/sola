// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/find_replacement.h"

#include <string>

namespace minhton {
MessageFindReplacement::MessageFindReplacement(MinhtonMessageHeader header,
                                               NodeInfo node_to_replace,
                                               SearchProgress search_progress, uint16_t hop_count)
    : header_(std::move(header)),
      node_to_replace_(std::move(node_to_replace)),
      search_progress_(search_progress),
      hop_count_(hop_count) {
  header_.setMessageType(MessageType::kFindReplacement);
  MessageLoggingAdditionalInfo logging_info{getNodeToReplace().getLogicalNodeInfo().getUuid(), "",
                                            "Prog: " + std::to_string(search_progress)};
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageFindReplacement::validateImpl() const { return getNodeToReplace().isInitialized(); }

minhton::NodeInfo MessageFindReplacement::getNodeToReplace() const { return node_to_replace_; }

SearchProgress MessageFindReplacement::getSearchProgress() const { return search_progress_; }

uint16_t MessageFindReplacement::getHopCount() const { return hop_count_; }

}  // namespace minhton
