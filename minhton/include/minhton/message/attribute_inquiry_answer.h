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
#include "minhton/message/serialize.h"

namespace minhton {
class MessageAttributeInquiryAnswer : public MinhtonMessage<MessageAttributeInquiryAnswer> {
public:
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
