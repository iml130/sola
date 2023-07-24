// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_MESSAGE_H_
#define NATTER_MESSAGE_H_

#include <string>

#include "solanet/uuid.h"

namespace natter {

/**
 * Class encapsulating a single message
 */
struct Message {
  Message(std::string topic, solanet::UUID sender_id, solanet::UUID message_id, std::string content,
          uint32_t round)
      : topic(std::move(topic)),
        sender_id(sender_id),
        message_id(message_id),
        content(std::move(content)),
        round(round) {}

  std::string topic{};
  solanet::UUID sender_id{};
  solanet::UUID message_id{};
  std::string content{};
  uint32_t round{};

  bool operator==(const Message &other) const {
    return topic == other.topic && sender_id == other.sender_id && message_id == other.message_id &&
           content == other.content && round == other.round;
  }
};
}  // namespace natter

#endif  // NATTER_MESSAGE_H_
