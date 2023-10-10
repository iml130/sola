// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_UPDATE_NEIGHBORS_H_
#define MINHTON_MESSAGE_UPDATE_NEIGHBORS_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

using NeighborsAndRelationships =
    std::vector<std::tuple<minhton::NodeInfo, minhton::NeighborRelationship>>;

namespace minhton {
/// @brief * **Usage:** Informing a node about a new or updated neighbor. Used mainly in the join
/// accept and leave procedure. The relationship of how to update the node has to be given.
/// * **Algorithm Association:** Updates & Response.
class MessageUpdateNeighbors : public MinhtonMessage<MessageUpdateNeighbors> {
public:
  /// @param neighbors_and_relationships Vector of tuples of nodes and their neighbor relationship.
  /// @param should_acknowledge Whether an acknowledgement should be sent back.
  MessageUpdateNeighbors(MinhtonMessageHeader header,
                         NeighborsAndRelationships neighbors_and_relationships,
                         bool should_acknowledge = false);

  NeighborsAndRelationships getNeighborsToUpdate() const;
  bool getShouldAcknowledge() const;

  SERIALIZE(header_, neighbors_and_relationships_, should_acknowledge_)

  MessageUpdateNeighbors() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  /// Vector of tuples of nodes and their neighbor relationship.
  NeighborsAndRelationships neighbors_and_relationships_;

  bool should_acknowledge_ = false;
};
}  // namespace minhton

#endif
