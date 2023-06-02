// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/message/replacement_offer.h"
namespace minhton {

MessageReplacementOffer::MessageReplacementOffer(const MinhtonMessageHeader &header)
    : header_(header) {
  header_.setMessageType(MessageType::kReplacementOffer);
  validate();
}

bool MessageReplacementOffer::validateImpl() const { return true; }

}  // namespace minhton
