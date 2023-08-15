// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_REMOVE_AND_UPDATE_NEIGHBORS_H_
#define MINHTON_MESSAGE_REMOVE_AND_UPDATE_NEIGHBORS_H_

#include "minhton/message/message.h"
#include "minhton/message/remove_neighbor.h"
#include "minhton/message/serialize.h"
#include "minhton/message/update_neighbors.h"

namespace minhton {
class MessageRemoveAndUpdateNeighbors : public MinhtonMessage<MessageRemoveAndUpdateNeighbors> {
public:
  MessageRemoveAndUpdateNeighbors(MinhtonMessageHeader header, MessageRemoveNeighbor remove_msg,
                                  MessageUpdateNeighbors update_msg,
                                  bool should_acknowledge = false);

  MessageRemoveNeighbor getMessageRemoveNeighbor() const;
  MessageUpdateNeighbors getMessageUpdateNeighbors() const;
  bool getShouldAcknowledge() const;

  SERIALIZE(header_, remove_msg_, update_msg_, should_acknowledge_)

  MessageRemoveAndUpdateNeighbors() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  MessageRemoveNeighbor remove_msg_;

  MessageUpdateNeighbors update_msg_;

  bool should_acknowledge_ = false;
};
}  // namespace minhton

#endif
