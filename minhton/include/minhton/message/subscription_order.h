// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_SUBSCRIPTION_ORDER_H_
#define MINHTON_MESSAGE_SUBSCRIPTION_ORDER_H_

#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
class MessageSubscriptionOrder : public MinhtonMessage<MessageSubscriptionOrder> {
public:
  MessageSubscriptionOrder(MinhtonMessageHeader header, std::vector<NodeData::Key> keys,
                           bool subscribe = false);

  std::vector<NodeData::Key> getKeys() const;
  bool getSubscribe() const;

  SERIALIZE(header_, keys_, subscribe_);

  MessageSubscriptionOrder() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  std::vector<NodeData::Key> keys_;

  bool subscribe_ = false;  // false = unsubscribe
};
}  // namespace minhton

#endif
