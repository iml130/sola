// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/lock_neighbor_request.h"

namespace minhton {

MessageLockNeighborRequest::MessageLockNeighborRequest(MinhtonMessageHeader header)
    : header_(std::move(header)) {
  header_.setMessageType(MessageType::kLockNeighborRequest);
  validate();
}

bool MessageLockNeighborRequest::validateImpl() const { return true; }

}  // namespace minhton
