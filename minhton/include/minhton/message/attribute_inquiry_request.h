// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_ATTRIBUTE_INQUIRY_REQUEST_H_
#define MINHTON_MESSAGE_ATTRIBUTE_INQUIRY_REQUEST_H_

#include <vector>

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
class MessageAttributeInquiryRequest : public MinhtonMessage<MessageAttributeInquiryRequest> {
public:
  explicit MessageAttributeInquiryRequest(const MinhtonMessageHeader &header,
                                          bool inquire_all = false,
                                          std::vector<std::string> missing_keys = {});

  bool getInquireAll() const;
  std::vector<std::string> getMissingKeys() const;

  SERIALIZE(header_, missing_keys_, inquire_all_);

  MessageAttributeInquiryRequest() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  bool inquire_all_ = false;

  std::vector<std::string> missing_keys_;
};
}  // namespace minhton

#endif
