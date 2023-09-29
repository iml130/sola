// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/get_neighbors.h"

namespace minhton {

MessageGetNeighbors::MessageGetNeighbors(MinhtonMessageHeader header, NodeInfo send_back_to_node,
                                         std::vector<NeighborRelationship> relationships)
    : header_(std::move(header)),
      send_back_to_node_(std::move(send_back_to_node)),
      relationships_(std::move(relationships)) {
  header_.setMessageType(MessageType::kGetNeighbors);

  MessageLoggingAdditionalInfo logging_info{getSendBackToNode().getLogicalNodeInfo().getUuid(), "",
                                            ""};
  std::string text = "relationships_to_request={";
  for (auto const &rel : relationships_) {
    // TODO add positions to request
    std::string current_text = std::to_string(static_cast<uint8_t>(rel)) + ",";
    text += current_text;
  }
  text += "}";
  logging_info.content = text;
  header_.setAdditionalLoggingInfo(logging_info);

  validate();
}

bool MessageGetNeighbors::validateImpl() const { return !relationships_.empty(); }

std::vector<minhton::NeighborRelationship> MessageGetNeighbors::getRelationships() const {
  return relationships_;
}

minhton::NodeInfo MessageGetNeighbors::getSendBackToNode() const { return send_back_to_node_; }

}  // namespace minhton
