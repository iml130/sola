// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_MINHCAST_MINHCAST_MESSAGE_H_
#define NATTER_MINHCAST_MINHCAST_MESSAGE_H_

#include <cstdint>
#include <string>
#include <tuple>
#include <utility>

#include "forwarding_limit.h"
#include "natter/minhcast_level_number.h"
#include "solanet/serializer/serialize.h"
#include "solanet/uuid.h"

namespace natter::minhcast {
class MinhcastMessage {
public:
  MinhcastMessage() = default;
  MinhcastMessage(std::string topic, solanet::UUID message_id,
                  std::tuple<solanet::UUID, minhcast::LevelNumber> initial_node,
                  std::tuple<solanet::UUID, minhcast::LevelNumber> last_node, std::string content,
                  uint32_t round, ForwardingLimit forwarding_limit, bool inner = false)
      : topic_(std::move(topic)),
        message_id_(std::move(message_id)),
        initial_node_(std::move(initial_node)),
        last_node_(std::move(last_node)),
        content_(std::move(content)),
        round_(round),
        forwarding_limit_(std::move(forwarding_limit)),
        inner_(inner) {}

  std::string getTopic() const { return topic_; }
  solanet::UUID getMessageID() const { return message_id_; }
  solanet::UUID getInitialNodeUUID() const { return std::get<0>(initial_node_); }
  solanet::UUID getLastNodeUUID() const { return std::get<0>(last_node_); }
  minhcast::LevelNumber getInitialNodePos() const { return std::get<1>(initial_node_); }
  minhcast::LevelNumber getLastNodePos() const { return std::get<1>(last_node_); }
  std::string getContent() const { return content_; }
  uint32_t getRound() const { return round_; }
  bool isInnerForward() const { return inner_; }
  ForwardingLimit getForwardingLimit() const { return forwarding_limit_; }

  SERIALIZE(topic_, message_id_, initial_node_, last_node_, content_, round_, forwarding_limit_,
            inner_);

private:
  std::string topic_;
  solanet::UUID message_id_;
  std::tuple<solanet::UUID, minhcast::LevelNumber> initial_node_;
  std::tuple<solanet::UUID, minhcast::LevelNumber> last_node_;
  std::string content_;
  uint32_t round_ = 0;
  ForwardingLimit forwarding_limit_;

  bool inner_ = false;
};
}  // namespace natter::minhcast

#endif  // DAISI_MINHCAST_MESSAGE_H_
