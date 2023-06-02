// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/lock_neighbor_response.h"

#include <algorithm>
namespace minhton {

MessageLockNeighborResponse::MessageLockNeighborResponse(const MinhtonMessageHeader &header,
                                                         bool successful)
    : header_(header), successful_(successful) {
  header_.setMessageType(MessageType::kLockNeighborResponse);
  validate();
}

bool MessageLockNeighborResponse::validateImpl() const { return true; }

bool MessageLockNeighborResponse::wasSuccessful() const { return successful_; }

}  // namespace minhton
