// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_REMOVE_NEIGHBOR_H_
#define MINHTON_MESSAGE_REMOVE_NEIGHBOR_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// A node is leaving the network. This is a notification to remove the given node from the routing
/// information.

class MessageRemoveNeighbor : public MinhtonMessage<MessageRemoveNeighbor> {
public:
  MessageRemoveNeighbor(MinhtonMessageHeader header, NodeInfo removed_position_node,
                        bool acknowledge = false);

  NodeInfo getRemovedPositionNode() const;
  bool getShouldAcknowledge() const;

  SERIALIZE(header_, removed_position_node_, acknowledge_)

  MessageRemoveNeighbor() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  /// The position of the node to remove
  NodeInfo removed_position_node_;

  bool acknowledge_ = false;
};
}  // namespace minhton

#endif
