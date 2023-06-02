// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_MESSAGE_H_
#define SOLA_MESSAGE_H_

#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace sola {

struct Message {
  std::string sender;
  std::string content;
};
struct TopicMessage {
  std::string topic;
  std::string sender;
  std::string content;
  std::array<uint8_t, 16> uuid;
};

using TopicMessageReceiveFct = std::function<void(const TopicMessage &m)>;
using MessageReceiveFct = std::function<void(const Message &m)>;

}  // namespace sola

#endif
