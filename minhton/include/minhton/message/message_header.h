// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_HEADER_H_
#define MINHTON_MESSAGE_HEADER_H_

#include <cstdint>
#include <map>

#include "minhton/core/constants.h"
#include "minhton/core/node_info.h"
#include "minhton/message/message_logging.h"
#include "minhton/message/serialize.h"
#include "minhton/message/types.h"

namespace minhton {
///
/// The header contains various information about the sender, receiver, message type, protocol
/// version, a timestamp, and so on. Works with MinhtonMessages.
///
class MinhtonMessageHeader {
public:
  MinhtonMessageHeader() = default;
  MinhtonMessageHeader(const NodeInfo &sender, const NodeInfo &target, uint64_t ref_event_id = 0);
  uint64_t getEventId() const;
  uint64_t getRefEventId() const;
  NodeInfo getSender() const;
  NodeInfo getTarget() const;
  MessageType getMessageType() const;

  void setSender(const NodeInfo &sender);
  void setTarget(const NodeInfo &target);
  void setEventId(uint64_t event_id);
  void setRefEventId(uint64_t ref_event_id);
  void setMessageType(MessageType type);

  /// Helper method to get the message logging info for this message
  /// \param receive whether the receive flag should be set to RECEIVEING or SENDING
  /// \returns the filled message logging info
  MessageLoggingInfo getMessageLoggingInfo(bool receive) const;

  void setAdditionalLoggingInfo(const MessageLoggingAdditionalInfo &additional_info);
  MessageLoggingAdditionalInfo getAdditionalLoggingInfo() const;

  /// Validation method used for the final check before sending.
  /// For other purposes, a different check is performed when creating a specific message.
  bool validate() const;

  SERIALIZE(sender_, target_, message_type_, ref_event_id_, event_id_, additional_logging_info_);

private:
  NodeInfo sender_;
  NodeInfo target_;
  MessageType message_type_ = MessageType::kInit;
  uint64_t ref_event_id_ = 0;
  uint64_t event_id_ = 0;
  MessageLoggingAdditionalInfo additional_logging_info_;
};

std::string getMessageTypeString(MessageType type);
}  // namespace minhton
#endif
