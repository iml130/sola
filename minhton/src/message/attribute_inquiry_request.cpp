// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/attribute_inquiry_request.h"

namespace minhton {

MessageAttributeInquiryRequest::MessageAttributeInquiryRequest(
    const MinhtonMessageHeader &header, bool inquire_all, std::vector<std::string> missing_keys)
    : header_(header), inquire_all_(inquire_all), missing_keys_(std::move(missing_keys)) {
  header_.setMessageType(MessageType::kAttributeInquiryRequest);

  MessageLoggingAdditionalInfo logging_info;
  logging_info.content = "missing_keys={";

  for (auto const &key : missing_keys_) {
    logging_info.content += key + ",";
  }
  logging_info.content += "}";

  if (inquire_all_) {
    logging_info.content += " inquire_all=true";
  } else {
    logging_info.content += " inquire_all=false";
  }
  header_.setAdditionalLoggingInfo(logging_info);
  validate();
}

bool MessageAttributeInquiryRequest::validateImpl() const {
  return inquire_all_ || !missing_keys_.empty();
}

bool MessageAttributeInquiryRequest::getInquireAll() const { return inquire_all_; }

std::vector<std::string> MessageAttributeInquiryRequest::getMissingKeys() const {
  return missing_keys_;
}

}  // namespace minhton
