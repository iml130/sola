// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_REPLACEMENT_UPDATE_H_
#define MINHTON_MESSAGE_REPLACEMENT_UPDATE_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// During the replacement process, the neighbor nodes of the leaving node / the replaced position
/// need to get an update about the network information of the replaced position. Through the
/// information in this message the neighbors can update the network information and eventually also
/// remove the removed position from their information if they also have it as their neighbor.
class MessageReplacementUpdate : public MinhtonMessage<MessageReplacementUpdate> {
public:
  MessageReplacementUpdate(const MinhtonMessageHeader &header, NodeInfo removed_position_node,
                           NodeInfo replaced_position_node, LogicalNodeInfo new_l_node_info,
                           bool should_acknowledge = false);

  NodeInfo getRemovedPositionNode() const;
  NodeInfo getReplacedPositionNode() const;
  LogicalNodeInfo getNewLogicalNodeInfo() const;
  bool getShouldAcknowledge() const;

  SERIALIZE(header_, removed_position_node_, replaced_position_node_, new_l_node_info_,
            should_acknowledge_);

  MessageReplacementUpdate() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  /// The node who is replacing and who's position will be empty afterwards.
  NodeInfo removed_position_node_;

  /// The node who is leaving the network and who's position will be replaced by the replacing node
  /// afterwards.
  NodeInfo replaced_position_node_;

  /// The LogicalNodeInfo that has the replaced position and a new uuid
  LogicalNodeInfo new_l_node_info_;

  bool should_acknowledge_ = false;
};
}  // namespace minhton

#endif
