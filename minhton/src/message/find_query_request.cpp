// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/find_query_request.h"
namespace minhton {

MessageFindQueryRequest::MessageFindQueryRequest(const MinhtonMessageHeader &header,
                                                 FindQuery query,
                                                 ForwardingDirection forwarding_direction,
                                                 std::pair<uint32_t, uint32_t> interval)
    : header_(header),
      query_(std::move(query)),
      forwarding_direction_(forwarding_direction),
      interval_(interval) {
  header_.setMessageType(MessageType::kFindQueryRequest);

  MessageLoggingAdditionalInfo logging_info;
  if (forwarding_direction_ == MessageFindQueryRequest::ForwardingDirection::kDirectionNone) {
    logging_info.content = "no direction";
  } else if (forwarding_direction_ ==
             MessageFindQueryRequest::ForwardingDirection::kDirectionLeft) {
    logging_info.content = "direction left";
  } else if (forwarding_direction_ ==
             MessageFindQueryRequest::ForwardingDirection::kDirectionRight) {
    logging_info.content = "direction right";
  }
  header_.setAdditionalLoggingInfo(logging_info);

  validate();
}

bool MessageFindQueryRequest::validateImpl() const { return true; }

FindQuery MessageFindQueryRequest::getFindQuery() const { return this->query_; }

MessageFindQueryRequest::ForwardingDirection MessageFindQueryRequest::getForwardingDirection()
    const {
  return forwarding_direction_;
}

std::pair<uint32_t, uint32_t> MessageFindQueryRequest::getInterval() const { return interval_; }

}  // namespace minhton
