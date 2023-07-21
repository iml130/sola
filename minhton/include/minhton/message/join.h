// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_JOIN_H_
#define MINHTON_MESSAGE_JOIN_H_

#include "minhton/message/find_end.h"
#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// @brief * **Usage:** When a node wants to join the network, it sends a MessageJoin to one node in
/// the network. If the fitting join position has been found, the target accepts the node as a new
/// child. Otherwise, the target forwards the message with the set entering node further.
/// * **Algorithm Association:** Join.
class MessageJoin : public MinhtonMessage<MessageJoin> {
public:
  /// @param entering_node The node that wants to enter the network. NetworkInfo must be set
  /// appropriately.
  /// @param search_progress Represents the procedure progress for differentiating the phases.
  MessageJoin(MinhtonMessageHeader header, NodeInfo entering_node,
              SearchProgress search_progress = SearchProgress::kNone);

  NodeInfo getEnteringNode() const;
  SearchProgress getSearchProgress() const;

  SERIALIZE(header_, entering_node_, search_progress_)

  MessageJoin() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  /// The node that wants to enter the network. PhysicalNodeInfo must be set appropriately.
  NodeInfo entering_node_;

  /// Stores information about what steps were already done (only for the minhton algorithm)
  SearchProgress search_progress_ = SearchProgress::kNone;
};
}  // namespace minhton

#endif
