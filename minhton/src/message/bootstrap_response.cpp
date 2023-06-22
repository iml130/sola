// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/bootstrap_response.h"

namespace minhton {

MessageBootstrapResponse::MessageBootstrapResponse(const MinhtonMessageHeader &header,
                                                   NodeInfo node_to_join)
    : header_(header), node_to_join_(std::move(node_to_join)) {
  header_.setMessageType(MessageType::kBootstrapResponse);
  MessageLoggingAdditionalInfo logging_info{getNodeToJoin().getLogicalNodeInfo().getUuid(), "", ""};
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageBootstrapResponse::validateImpl() const {
  return this->getNodeToJoin().isInitialized();
}

minhton::NodeInfo MessageBootstrapResponse::getNodeToJoin() const { return this->node_to_join_; }

}  // namespace minhton
