// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_SUBSCRIPTION_UPDATE_H_
#define MINHTON_MESSAGE_SUBSCRIPTION_UPDATE_H_

#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
class MessageSubscriptionUpdate : public MinhtonMessage<MessageSubscriptionUpdate> {
public:
  MessageSubscriptionUpdate(MinhtonMessageHeader header, NodeData::Key key, NodeData::Value value);

  NodeData::Key getKey() const;
  NodeData::Value getValue() const;

  SERIALIZE(header_, key_, value_)

  MessageSubscriptionUpdate() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  NodeData::Key key_;
  NodeData::Value value_;
};
}  // namespace minhton

#endif
