// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_GET_NEIGHBORS_H_
#define MINHTON_MESSAGE_GET_NEIGHBORS_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// Currently only used in the join accept procedure in rare cases by the parent to get the correct
/// adjacent neighbors for the child. The sender is giving a vector of neighbor relationships which
/// he wants to know about from the targets routing information. The answer is supposed to be a
/// InformAboutNeighbors message.
class MessageGetNeighbors : public MinhtonMessage<MessageGetNeighbors> {
public:
  MessageGetNeighbors(MinhtonMessageHeader header, NodeInfo send_back_to_node,
                      std::vector<NeighborRelationship> relationships);

  NodeInfo getSendBackToNode() const;
  std::vector<NeighborRelationship> getRelationships() const;

  SERIALIZE(header_, send_back_to_node_, relationships_);

  MessageGetNeighbors() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  NodeInfo send_back_to_node_;

  /// A vector of all the neighbor relationships the sender wants to know about.
  std::vector<NeighborRelationship> relationships_;
};
}  // namespace minhton

#endif
