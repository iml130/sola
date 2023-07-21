// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_INFORM_ABOUT_NEIGHBORS_H_
#define MINHTON_MESSAGE_INFORM_ABOUT_NEIGHBORS_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// @brief * **Usage:** This message is an answer to a MessageGetNeighbors. It contains the node
/// information about the requested relationships and/or network information. The node sends the
/// message back.
/// * **Algorithm Association:** Updates & Response.
class MessageInformAboutNeighbors : public MinhtonMessage<MessageInformAboutNeighbors> {
public:
  /// @param neighbors Vector of the information about the requested relationships and/or network
  /// information.
  MessageInformAboutNeighbors(MinhtonMessageHeader header, std::vector<NodeInfo> neighbors);

  std::vector<NodeInfo> getRequestedNeighbors() const;

  SERIALIZE(header_, neighbors_)

  MessageInformAboutNeighbors() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  /// Vector of the node information.
  std::vector<NodeInfo> neighbors_;
};
}  // namespace minhton

#endif
