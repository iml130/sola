// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhcast/broadcast_info.h"

#include <catch2/catch_test_macros.hpp>

#include "utils/utils.h"

TEST_CASE("[MINHCAST] ForwardingLimit", "MINHCAST") {
  using namespace natter::minhcast;
  REQUIRE_NOTHROW(ForwardingLimit{2, 3});
  REQUIRE_THROWS(ForwardingLimit{3, 2});

  ForwardingLimit lim{1, 500};

  REQUIRE(lim.up() == 1);
  REQUIRE(lim.down() == 500);
}

TEST_CASE("[MINHCAST] BroadcastInfo NotInitialSender", "MINHCAST") {
  using namespace natter::minhcast;
  using namespace natter::utils;

  BroadcastInfo bc{
      .own_node = {generateUUID(), {2, 0, 3}},
      .last_node = {generateUUID(), {1, 0, 3}},
      .initial_node = {generateUUID(), {0, 0, 3}},
      .forwarding_limit{2, std::numeric_limits<Level>::max()},
      .topic = "TOPIC",
      .msg_id = generateUUID(),
      .content = "CONTENT",
      .current_round = 3,
  };

  REQUIRE(bc.ownLevel() == 2);
  REQUIRE(bc.ownNumber() == 0);
  REQUIRE(bc.ownFanout() == 3);

  REQUIRE(bc.lastLevel() == 1);
  REQUIRE(bc.lastNumber() == 0);
  REQUIRE(bc.lastFanout() == 3);

  REQUIRE(bc.initialLevel() == 0);
  REQUIRE(bc.initialNumber() == 0);
  REQUIRE(bc.initialFanout() == 3);

  REQUIRE_FALSE(bc.isInitialSender());
  REQUIRE(bc.receivedDirectlyFromAbove());
  REQUIRE(bc.receivedFromAbove());
  REQUIRE_FALSE(bc.parentFromInitial());
  REQUIRE(bc.allowedDownForward());
  REQUIRE_FALSE(bc.allowedUpForward());
  REQUIRE_FALSE(bc.receivedFromLowerAdjacent());
}
