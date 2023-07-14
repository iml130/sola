// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/inform_about_neighbors.h"

namespace minhton {

MessageInformAboutNeighbors::MessageInformAboutNeighbors(MinhtonMessageHeader header,
                                                         std::vector<NodeInfo> neighbors)
    : header_(std::move(header)), neighbors_(std::move(neighbors)) {
  header_.setMessageType(MessageType::kInformAboutNeighbors);

  MessageLoggingAdditionalInfo logging_info;
  std::string text = "neighbors={";
  for (auto const &neighbor : neighbors_) {
    std::string current_text = neighbor.getString() + ",";
    text += current_text;
  }
  text += "}";
  logging_info.content = text;
  header_.setAdditionalLoggingInfo(logging_info);

  validate();
}

bool MessageInformAboutNeighbors::validateImpl() const { return !neighbors_.empty(); }

std::vector<minhton::NodeInfo> MessageInformAboutNeighbors::getRequestedNeighbors() const {
  return neighbors_;
}

}  // namespace minhton
