// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/bootstrap_discover.h"

namespace minhton {

MessageBootstrapDiscover::MessageBootstrapDiscover(const MinhtonMessageHeader &header,
                                                   std::string discovery_msg)
    : header_(header), discovery_msg_(std::move(discovery_msg)) {
  header_.setMessageType(MessageType::kBootstrapDiscover);
  MessageLoggingAdditionalInfo logging_info;
  logging_info.content = "discovery_message=" + this->getDiscoveryMessage();
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageBootstrapDiscover::validateImpl() const { return true; }

std::string MessageBootstrapDiscover::getDiscoveryMessage() const { return this->discovery_msg_; }
}  // namespace minhton
