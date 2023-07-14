// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/update_neighbors.h"

namespace minhton {

MessageUpdateNeighbors::MessageUpdateNeighbors(
    MinhtonMessageHeader header, NeighborsAndRelationships neighbors_and_relationships,
    bool should_acknowledge)
    : header_(std::move(header)),
      neighbors_and_relationships_(std::move(neighbors_and_relationships)),
      should_acknowledge_(should_acknowledge) {
  header_.setMessageType(MessageType::kUpdateNeighbors);

  MessageLoggingAdditionalInfo logging_info;
  std::string text = "neighbors_to_update={";
  for (auto const &neighbor_rel : neighbors_and_relationships_) {
    minhton::NodeInfo neighbor = std::get<0>(neighbor_rel);
    minhton::NeighborRelationship rel = std::get<1>(neighbor_rel);

    std::string current_text = "(" + neighbor.getString() + "," + std::to_string(rel) + "),";
    text += current_text;
  }
  text += "}";
  logging_info.content = text;
  header_.setAdditionalLoggingInfo(logging_info);

  validate();
}

bool MessageUpdateNeighbors::validateImpl() const { return !neighbors_and_relationships_.empty(); }

NeighborsAndRelationships MessageUpdateNeighbors::getNeighborsToUpdate() const {
  return neighbors_and_relationships_;
}

bool MessageUpdateNeighbors::getShouldAcknowledge() const { return should_acknowledge_; }

}  // namespace minhton
