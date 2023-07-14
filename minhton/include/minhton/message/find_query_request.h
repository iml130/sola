// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_FIND_QUERY_REQUEST_H_
#define MINHTON_MESSAGE_FIND_QUERY_REQUEST_H_

#include <utility>

#include "minhton/algorithms/esearch/find_query.h"
#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
class MessageFindQueryRequest : public MinhtonMessage<MessageFindQueryRequest> {
public:
  enum ForwardingDirection : uint8_t {
    kDirectionNone,
    kDirectionLeft,
    kDirectionRight,
  };

  MessageFindQueryRequest(MinhtonMessageHeader header, FindQuery query,
                          ForwardingDirection forwarding_direction,
                          std::pair<uint32_t, uint32_t> interval);

  FindQuery getFindQuery() const;
  ForwardingDirection getForwardingDirection() const;
  std::pair<uint32_t, uint32_t> getInterval() const;

  SERIALIZE(header_, query_, forwarding_direction_, interval_);

  MessageFindQueryRequest() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  FindQuery query_;

  ForwardingDirection forwarding_direction_ = kDirectionNone;

  std::pair<uint32_t, uint32_t> interval_;
};
}  // namespace minhton

#endif
