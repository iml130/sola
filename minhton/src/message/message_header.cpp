// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/message_header.h"

#include <stdexcept>

#include "minhton/utils/uuid.h"

namespace minhton {

// adding a key-value map to get the string for easier debugging purposes
// (explicit external to allow external linkage)
extern const std::unordered_map<uint32_t, std::string> kMapMinhtonMessageTypeStrings = {
    {MessageType::kInit, "INIT"},

    {MessageType::kJoin, "JOIN"},
    {MessageType::kJoinAccept, "JOIN_ACCEPT"},
    {MessageType::kJoinAcceptAck, "JOIN_ACCEPT_ACK"},

    {MessageType::kFindQueryRequest, "FIND_QUERY_REQUEST"},
    {MessageType::kFindQueryAnswer, "FIND_QUERY_ANSWER"},
    {MessageType::kAttributeInquiryRequest, "ATTRIBUTE_INQUIRY_REQUEST"},
    {MessageType::kAttributeInquiryAnswer, "ATTRIBUTE_INQUIRY_ANSWER"},
    {MessageType::kSubscriptionOrder, "SUBSCRIPTION_ORDER"},
    {MessageType::kSubscriptionUpdate, "SUBSCRIPTION_UPDATE"},

    {MessageType::kSearchExact, "SEARCH_EXACT"},
    {MessageType::kSearchExactFailure, "SEARCH_EXACT_FAILURE"},
    {MessageType::kEmpty, "EMPTY"},

    {MessageType::kBootstrapDiscover, "BOOTSTRAP_DISCOVER"},
    {MessageType::kBootstrapResponse, "BOOTSTRAP_RESPONSE"},

    {MessageType::kRemoveNeighbor, "REMOVE_NEIGHBOR"},
    {MessageType::kRemoveNeighborAck, "REMOVE_NEIGHBOR_ACK"},
    {MessageType::kUpdateNeighbors, "UPDATE_NEIGHBORS"},
    {MessageType::kReplacementUpdate, "REPLACEMENT_UPDATE"},
    {MessageType::kGetNeighbors, "GET_NEIGHBORS"},
    {MessageType::kInformAboutNeighbors, "INFORM_ABOUT_NEIGHBORS"},

    {MessageType::kFindReplacement, "FIND_REPLACEMENT"},
    {MessageType::kReplacementNack, "REPLACEMENT_NACK"},
    {MessageType::kSignOffParentRequest, "SIGN_OFF_PARENT_REQUEST"},
    {MessageType::kLockNeighborRequest, "LOCK_NEIGHBOR_REQUEST"},
    {MessageType::kLockNeighborResponse, "LOCK_NEIGHBOR_RESPONSE"},
    {MessageType::kSignOffParentAnswer, "SIGN_OFF_PARENT_ANSWER"},
    {MessageType::kRemoveAndUpdateNeighbor, "REMOVE_AND_UPDATE_NEIGHBOR"},
    {MessageType::kReplacementOffer, "REPLACEMENT_OFFER"},
    {MessageType::kReplacementAck, "REPLACEMENT_ACK"},
    {MessageType::kUnlockNeighbor, "UNLOCK_NEIGHBOR"},
};

std::string getMessageTypeString(MessageType type) {
  auto type_string = kMapMinhtonMessageTypeStrings.find(type);
  if (type_string == kMapMinhtonMessageTypeStrings.end()) {
    return "Unknown";
  }
  return type_string->second;
}

MinhtonMessageHeader::MinhtonMessageHeader(const NodeInfo &sender, const NodeInfo &target,
                                           uint64_t ref_event_id)
    : sender_(sender), target_(target), ref_event_id_(ref_event_id), event_id_(generateEventId()) {}

void MinhtonMessageHeader::setEventId(const uint64_t event_id) {
  if (event_id == 0) {
    throw std::invalid_argument("EventId can't be equal to 0.");
  }

  if (event_id == this->ref_event_id_) {
    throw std::invalid_argument("EventId can't be equal to RefEventId.");
  }

  this->event_id_ = event_id;
}

void MinhtonMessageHeader::setRefEventId(const uint64_t ref_event_id) {
  if (ref_event_id == 0) {
    throw std::invalid_argument("EventId can't be equal to 0.");
  }
  if (ref_event_id == this->event_id_) {
    throw std::invalid_argument("RefEventId can't be equal to EventId.");
  }

  this->ref_event_id_ = ref_event_id;
}

void MinhtonMessageHeader::setSender(const NodeInfo &sender) { this->sender_ = sender; }

void MinhtonMessageHeader::setTarget(const NodeInfo &target) { this->target_ = target; }

void MinhtonMessageHeader::setMessageType(MessageType type) { this->message_type_ = type; }

MessageLoggingInfo MinhtonMessageHeader::getMessageLoggingInfo(bool receive) const {
  MessageLoggingInfo logging_info{
      (int)getMessageType(),
      receive ? MessageProcessingModes::kReceiving : MessageProcessingModes::kSending,
      getEventId(),
      getRefEventId(),
      getSender().getLogicalNodeInfo().getUuid(),
      getTarget().getLogicalNodeInfo().getUuid(),
      getAdditionalLoggingInfo()};
  return logging_info;
}

void MinhtonMessageHeader::setAdditionalLoggingInfo(
    const MessageLoggingAdditionalInfo &additional_info) {
  this->additional_logging_info_ = additional_info;
}

MessageLoggingAdditionalInfo MinhtonMessageHeader::getAdditionalLoggingInfo() const {
  return this->additional_logging_info_;
}

bool MinhtonMessageHeader::validate() const {
  bool network_info_init =
      sender_.getNetworkInfo().isInitialized() && target_.getNetworkInfo().isInitialized();
  bool message_type_set = message_type_ != MessageType::kInit;
  bool same_fanout = sender_.isInitialized() ? sender_.getFanout() == target_.getFanout() : true;
  bool event_id_set = event_id_ > 0;

  return network_info_init && message_type_set && same_fanout && event_id_set;
}

uint64_t MinhtonMessageHeader::getEventId() const { return this->event_id_; }

uint64_t MinhtonMessageHeader::getRefEventId() const { return this->ref_event_id_; }

NodeInfo MinhtonMessageHeader::getSender() const { return this->sender_; }

NodeInfo MinhtonMessageHeader::getTarget() const { return this->target_; }

MessageType MinhtonMessageHeader::getMessageType() const { return this->message_type_; };

}  // namespace minhton
