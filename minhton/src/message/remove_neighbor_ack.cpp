// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/remove_neighbor_ack.h"

namespace minhton {

MessageRemoveNeighborAck::MessageRemoveNeighborAck(MinhtonMessageHeader header)
    : header_(std::move(header)) {
  header_.setMessageType(MessageType::kRemoveNeighborAck);
  validate();
}

bool MessageRemoveNeighborAck::validateImpl() const { return true; }

}  // namespace minhton
