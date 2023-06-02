// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/remove_and_update_neighbor.h"

namespace minhton {

MessageRemoveAndUpdateNeighbors::MessageRemoveAndUpdateNeighbors(const MinhtonMessageHeader &header,
                                                                 MessageRemoveNeighbor remove_msg,
                                                                 MessageUpdateNeighbors update_msg,
                                                                 bool should_acknowledge)
    : header_(header),
      remove_msg_(std::move(remove_msg)),
      update_msg_(std::move(update_msg)),
      should_acknowledge_(should_acknowledge) {
  header_.setMessageType(MessageType::kRemoveAndUpdateNeighbor);
  validate();
}

bool MessageRemoveAndUpdateNeighbors::validateImpl() const { return true; }

MessageRemoveNeighbor MessageRemoveAndUpdateNeighbors::getMessageRemoveNeighbor() const {
  return remove_msg_;
}

MessageUpdateNeighbors MessageRemoveAndUpdateNeighbors::getMessageUpdateNeighbors() const {
  return update_msg_;
}

bool MessageRemoveAndUpdateNeighbors::getShouldAcknowledge() const { return should_acknowledge_; }

}  // namespace minhton
