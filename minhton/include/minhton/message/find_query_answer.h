// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_FIND_QUERY_ANSWER_H_
#define MINHTON_MESSAGE_FIND_QUERY_ANSWER_H_

#include <unordered_map>
#include <vector>

#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/core/node_info.h"
#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** A DSN sends a MessageFindQueryAnswer back to the requesting node (that sent
/// this DSN a MessageFindQueryRequest) with the data that was aquired.
/// * **Algorithm Association:** Entity Search.
class MessageFindQueryAnswer : public MinhtonMessage<MessageFindQueryAnswer> {
public:
  /// @param nodes_with_attributes Mapping NodeInfos to Attributes. The attributes consist of
  /// multiple NoteDatas. NodeDatas have a string key and a corresponding value of varying types.
  MessageFindQueryAnswer(MinhtonMessageHeader header,
                         NodeData::NodesWithAttributes nodes_with_attributes);

  NodeData::NodesWithAttributes getFulfillingNodesWithAttributes() const;

  SERIALIZE(header_, nodes_with_attributes_);

  MessageFindQueryAnswer() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  NodeData::NodesWithAttributes nodes_with_attributes_;
};
}  // namespace minhton

#endif
