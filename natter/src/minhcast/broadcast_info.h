// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_BROADCAST_INFO_H_
#define NATTER_BROADCAST_INFO_H_

#include <cstdint>
#include <string>

#include "forwarding_limit.h"
#include "natter/minhcast_level_number.h"
#include "natter/uuid.h"
#include "utils/tree_helper.h"

namespace natter::minhcast {
struct BroadcastInfo {
  const std::tuple<UUID, LevelNumber> own_node;
  const std::tuple<UUID, LevelNumber> last_node;
  const std::tuple<UUID, LevelNumber> initial_node;
  const ForwardingLimit forwarding_limit;
  const std::string topic;
  const UUID msg_id;
  const std::string content;
  const uint32_t current_round;

  // Own node
  inline uint32_t ownLevel() const { return std::get<0>(getOwnNodePos()); }
  inline uint32_t ownNumber() const { return std::get<1>(getOwnNodePos()); }
  inline uint32_t ownFanout() const { return std::get<2>(getOwnNodePos()); }

  // Last node
  inline uint32_t lastLevel() const { return std::get<0>(getLastNodePos()); }
  inline uint32_t lastNumber() const { return std::get<1>(getLastNodePos()); }
  inline uint32_t lastFanout() const { return std::get<2>(getLastNodePos()); }

  // Initial node
  inline uint32_t initialLevel() const { return std::get<0>(getInitialNodePos()); }
  inline uint32_t initialNumber() const { return std::get<1>(getInitialNodePos()); }
  inline uint32_t initialFanout() const { return std::get<2>(getInitialNodePos()); }

  inline LevelNumber getOwnNodePos() const { return std::get<1>(own_node); }
  inline LevelNumber getLastNodePos() const { return std::get<1>(last_node); }
  inline LevelNumber getInitialNodePos() const { return std::get<1>(initial_node); }

  bool isInitialSender() const { return initial_node == own_node; }
  bool receivedDirectlyFromAbove() const { return lastLevel() + 1 == ownLevel(); }
  bool receivedFromAbove() const { return lastLevel() < ownLevel(); }
  bool parentFromInitial() const {
    return childFromNode(std::get<1>(initial_node), std::get<1>(own_node));
  }
  bool allowedDownForward() const { return ownLevel() < forwarding_limit.down(); }
  bool allowedUpForward() const { return ownLevel() > forwarding_limit.up(); }
  bool receivedFromLowerAdjacent() const {
    return lastLevel() > ownLevel() && lastLevel() - 1 > ownLevel();
  }
};
}  // namespace natter::minhcast

#endif  // DAISI_BROADCAST_INFO_H_
