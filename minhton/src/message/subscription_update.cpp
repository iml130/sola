// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/subscription_update.h"
namespace minhton {

MessageSubscriptionUpdate::MessageSubscriptionUpdate(const MinhtonMessageHeader &header,
                                                     NodeData::Key key, NodeData::Value value)
    : header_(header), key_(std::move(key)), value_(value) {
  header_.setMessageType(MessageType::kSubscriptionUpdate);
  MessageLoggingAdditionalInfo logging_info;
  logging_info.content = "key=" + key_;
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageSubscriptionUpdate::validateImpl() const { return true; }

NodeData::Key MessageSubscriptionUpdate::getKey() const { return this->key_; }

NodeData::Value MessageSubscriptionUpdate::getValue() const { return this->value_; }

}  // namespace minhton
