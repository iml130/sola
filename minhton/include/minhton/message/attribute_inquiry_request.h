// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_ATTRIBUTE_INQUIRY_REQUEST_H_
#define MINHTON_MESSAGE_ATTRIBUTE_INQUIRY_REQUEST_H_

#include <vector>

#include "minhton/message/message.h"
#include "solanet/serializer/serialize.h"

namespace minhton {
/// @brief * **Usage:** A node that is not a DSN may receive a MessageAttributeInquiryRequest during
/// an ongoing Entity Search. A MessageAttributeInquiryRequest always comes from a DSN, which can
/// also be the requesting node itself.
/// * **Algorithm Association:** Entity Search.
class MessageAttributeInquiryRequest : public MinhtonMessage<MessageAttributeInquiryRequest> {
public:
  /// @param inquire_all If set to true, all existing local data is added to the answer message,
  /// ignoring the missing_keys property.
  /// @param missing_keys The keys for which the node looks up its own local data. The corresponding
  /// value with a timestamp is added to the MessageAttributeInquiryAnswer if it is present.
  explicit MessageAttributeInquiryRequest(MinhtonMessageHeader header, bool inquire_all = false,
                                          std::vector<std::string> missing_keys = {});

  bool getInquireAll() const;
  std::vector<std::string> getMissingKeys() const;

  SERIALIZE(header_, missing_keys_, inquire_all_);

  MessageAttributeInquiryRequest() = default;

private:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

  bool inquire_all_ = false;

  std::vector<std::string> missing_keys_;
};
}  // namespace minhton

#endif
