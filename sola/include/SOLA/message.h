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

#include "solanet/serializer/serialize.h"
#include "solanet/uuid.h"

namespace sola {

struct TopicMessage {
  std::string topic;
  solanet::UUID sender;
  std::string content;
  solanet::UUID uuid;

  SERIALIZE(topic, sender, content, uuid);
};

using TopicMessageReceiveFct = std::function<void(const TopicMessage &m)>;

}  // namespace sola

#endif
