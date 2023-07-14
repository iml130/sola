// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/signoff_parent_request.h"

namespace minhton {

MessageSignoffParentRequest::MessageSignoffParentRequest(MinhtonMessageHeader header)
    : header_(std::move(header)) {
  header_.setMessageType(MessageType::kSignOffParentRequest);
  validate();
}

bool MessageSignoffParentRequest::validateImpl() const { return true; }

}  // namespace minhton
