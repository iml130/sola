// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

#include "message/message.h"
#include "message/types_all.h"
#include "utils/uuid.h"

using namespace minhton;

TEST_CASE("MinhtonMessageHeader General", "[MinhtonMessageHeader][General]") {
  NodeInfo sender, target;
  MinhtonMessageHeader header = MinhtonMessageHeader(sender, target);
  REQUIRE(header.getEventId() != 0);
  REQUIRE(header.getEventId() != header.getRefEventId());
  REQUIRE(MinhtonMessageHeader(sender, target).getEventId() !=
          MinhtonMessageHeader(sender, target).getEventId());
}

TEST_CASE("MinhtonMessageHeader IdCollisionCheck", "[MinhtonMessageHeader][IdCollisionCheck]") {
  NodeInfo sender, target;

  for (uint16_t index = 0; index < UINT16_MAX; index++)
    REQUIRE(MinhtonMessageHeader(sender, target).getEventId() !=
            MinhtonMessageHeader().getEventId());

  for (uint16_t index = 0; index < UINT16_MAX; index++)
    REQUIRE_NOTHROW(MinhtonMessageHeader(sender, target));
}

TEST_CASE("MinhtonMessageHeader Constructor", "[MinhtonMessageHeader][Constructor]") {
  NodeInfo sender, target;
  MinhtonMessageHeader header = MinhtonMessageHeader(sender, target);
  REQUIRE(header.getEventId() != header.getRefEventId());

  header = MinhtonMessageHeader(sender, target, UINT64_MAX);
  REQUIRE(header.getRefEventId() == UINT64_MAX);
  REQUIRE(header.getEventId() != header.getRefEventId());
}

TEST_CASE("MinhtonMessageHeader setEventId", "[MinhtonMessageHeader][setEventId]") {
  MinhtonMessageHeader header = MinhtonMessageHeader();
  header.setEventId(1);
  REQUIRE(header.getEventId() == 1);
  header.setEventId(UINT64_MAX);
  REQUIRE(header.getEventId() == UINT64_MAX);
  REQUIRE_THROWS_AS(header.setEventId(0), std::invalid_argument);
}

TEST_CASE("MinhtonMessageHeader setRefEventId", "[MinhtonMessageHeader][setRefEventId]") {
  MinhtonMessageHeader header = MinhtonMessageHeader();
  header.setRefEventId(1);
  REQUIRE(header.getRefEventId() == 1);
  header.setRefEventId(UINT64_MAX);
  REQUIRE(header.getRefEventId() == UINT64_MAX);
  REQUIRE_THROWS_AS(header.setRefEventId(0), std::invalid_argument);
}
