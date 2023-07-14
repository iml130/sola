// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/find_query_answer.h"

namespace minhton {

MessageFindQueryAnswer::MessageFindQueryAnswer(MinhtonMessageHeader header,
                                               NodeData::NodesWithAttributes nodes_with_attributes)
    : header_(std::move(header)), nodes_with_attributes_(std::move(nodes_with_attributes)) {
  header_.setMessageType(MessageType::kFindQueryAnswer);
  validate();
}

bool MessageFindQueryAnswer::validateImpl() const { return true; }

NodeData::NodesWithAttributes MessageFindQueryAnswer::getFulfillingNodesWithAttributes() const {
  return this->nodes_with_attributes_;
}

}  // namespace minhton
