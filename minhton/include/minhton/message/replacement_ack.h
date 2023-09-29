// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_REPLACEMENT_ACK_H_
#define MINHTON_MESSAGE_REPLACEMENT_ACK_H_

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** The node that wants to leave the network receives a MessageReplacementOffer
/// from a node that is willing to replace its position. As a response, the leaving node sends all
/// of its necessary information in the MessageReplacementAck to the replacing node. After this
/// message was sent, the node can finally leave the network.
/// * **Algorithm Association:** Leave.
class MessageReplacementAck : public MinhtonMessage<MessageReplacementAck> {
public:
  struct LockedStates {
    bool locked;
    bool locked_right;
    bool locked_left;
  };

  /// @param neighbors A vector of all neighbors the leaving node has information about.
  /// @param lockedStates A struct tracking the lock state of this node, its right neighbor, and its
  /// left neighbor.
  MessageReplacementAck(MinhtonMessageHeader header, std::vector<minhton::NodeInfo> neighbors,
                        LockedStates lockedStates);

  std::vector<minhton::NodeInfo> getNeighbors() const;
  LockedStates getLockedStates() const;

  SERIALIZE(header_, neighbors_, locked_, locked_right_, locked_left_);

  MessageReplacementAck() = default;

private:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

  /// A vector of all neighbors the leaving node has information about
  std::vector<minhton::NodeInfo> neighbors_;

  /// Booleans indicating which nodes are locked
  bool locked_ = false;
  bool locked_right_ = false;
  bool locked_left_ = false;
};
}  // namespace minhton

#endif
