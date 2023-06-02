// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/join_accept_ack.h"
namespace minhton {

MessageJoinAcceptAck::MessageJoinAcceptAck(const MinhtonMessageHeader &header) : header_(header) {
  header_.setMessageType(MessageType::kJoinAcceptAck);
  validate();
}

bool MessageJoinAcceptAck::validateImpl() const { return true; }

}  // namespace minhton
