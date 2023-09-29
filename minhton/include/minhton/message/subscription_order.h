// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_SUBSCRIPTION_ORDER_H_
#define MINHTON_MESSAGE_SUBSCRIPTION_ORDER_H_

#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** A node can send a MessageSubscriptionOrder to subscribe or unsubscribe
/// itself from the data another node maintains for the specified keys.
/// * **Algorithm Association:** Entity Search.
class MessageSubscriptionOrder : public MinhtonMessage<MessageSubscriptionOrder> {
public:
  /// @param keys Multiple keys for which the node wants to change its subscription.
  /// @param subscribe True subscribes the node to the specified keys, False unsubscribes the node.
  MessageSubscriptionOrder(MinhtonMessageHeader header, std::vector<NodeData::Key> keys,
                           bool subscribe = false);

  std::vector<NodeData::Key> getKeys() const;
  bool getSubscribe() const;

  SERIALIZE(header_, keys_, subscribe_);

  MessageSubscriptionOrder() = default;

private:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

  std::vector<NodeData::Key> keys_;

  bool subscribe_ = false;  // false = unsubscribe
};
}  // namespace minhton

#endif
