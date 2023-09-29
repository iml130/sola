// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_ATTRIBUTE_INQUIRY_ANSWER_H_
#define MINHTON_MESSAGE_ATTRIBUTE_INQUIRY_ANSWER_H_

#include <unordered_map>

#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** The message that the node which received a MessageAttributeInquiryRequest
/// sends back.
/// * **Algorithm Association:** Entity Search.
class MessageAttributeInquiryAnswer : public MinhtonMessage<MessageAttributeInquiryAnswer> {
public:
  /// @param inquired_node NodeInfo of the current node that received the Attribute Inquiry Request
  /// @param attribute_values_and_types A map including the local data with timestamps and the
  /// information whether a value is static or dynamic.
  /// @param removed_attribute_keys A vector of the keys for which their distributed data shall be
  /// removed.
  MessageAttributeInquiryAnswer(
      MinhtonMessageHeader header, NodeInfo inquired_node,
      std::unordered_map<std::string, NodeData::ValueAndType> attribute_values_and_types = {},
      std::vector<std::string> removed_attribute_keys = {});

  NodeInfo getInquiredNode() const;
  std::unordered_map<std::string, NodeData::ValueAndType> getAttributeValuesAndTypes() const;
  std::vector<std::string> getRemovedAttributeKeys() const;

  SERIALIZE(header_, inquired_node_, attribute_values_and_types_, removed_attribute_keys_);

  MessageAttributeInquiryAnswer() = default;

protected:
  friend MinhtonMessage;

  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  NodeInfo inquired_node_;

  std::unordered_map<std::string, NodeData::ValueAndType> attribute_values_and_types_;

  std::vector<std::string> removed_attribute_keys_;
};
}  // namespace minhton

#endif
