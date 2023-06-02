// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/attribute_inquiry_answer.h"

namespace minhton {

MessageAttributeInquiryAnswer::MessageAttributeInquiryAnswer(
    const MinhtonMessageHeader &header, NodeInfo inquired_node,
    std::unordered_map<std::string, NodeData::ValueAndType> attribute_values_and_types,
    std::vector<std::string> removed_attribute_keys)
    : header_(header),
      inquired_node_(std::move(inquired_node)),
      attribute_values_and_types_(std::move(attribute_values_and_types)),
      removed_attribute_keys_(std::move(removed_attribute_keys)) {
  header_.setMessageType(MessageType::kAttributeInquiryAnswer);
  validate();
}

bool MessageAttributeInquiryAnswer::validateImpl() const {
  return getInquiredNode().isInitialized();
}

std::unordered_map<std::string, NodeData::ValueAndType>
MessageAttributeInquiryAnswer::getAttributeValuesAndTypes() const {
  return attribute_values_and_types_;
}

std::vector<std::string> MessageAttributeInquiryAnswer::getRemovedAttributeKeys() const {
  return removed_attribute_keys_;
}

NodeInfo MessageAttributeInquiryAnswer::getInquiredNode() const { return inquired_node_; }

}  // namespace minhton
