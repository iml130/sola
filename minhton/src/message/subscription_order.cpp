// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/subscription_order.h"
namespace minhton {

MessageSubscriptionOrder::MessageSubscriptionOrder(const MinhtonMessageHeader &header,
                                                   std::vector<NodeData::Key> keys, bool subscribe)
    : header_(header), keys_(std::move(keys)), subscribe_(subscribe) {
  header_.setMessageType(MessageType::kSubscriptionOrder);
  MessageLoggingAdditionalInfo logging_info;
  logging_info.content =
      std::string("subscribe=") + std::string(this->getSubscribe() ? "true" : "false");
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageSubscriptionOrder::validateImpl() const { return true; }

std::vector<NodeData::Key> MessageSubscriptionOrder::getKeys() const { return this->keys_; }

bool MessageSubscriptionOrder::getSubscribe() const { return this->subscribe_; }

}  // namespace minhton
