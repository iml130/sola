// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "core/logical_node_info.h"

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <unordered_map>

#include "core/constants.h"
using namespace minhton;

TEST_CASE("LogicalNodeInfo Constructors", "[LogicalNodeInfo][Init]") {
  uint16_t chosen_fanout = 4;
  uint32_t chosen_level = 2;
  uint32_t chosen_num = 2;

  // empty constructor
  minhton::LogicalNodeInfo test_1;
  REQUIRE(test_1.getLevel() == 0);
  REQUIRE(test_1.getNumber() == 0);
  REQUIRE_FALSE(test_1.isInitialized());
  REQUIRE(test_1.getFanout() == 0);
  REQUIRE_FALSE(test_1.isRoot());

  // constructor with fanout
  // nothing set, except for the fanout
  minhton::LogicalNodeInfo test_2(chosen_fanout);
  REQUIRE(test_2.getLevel() == 0);
  REQUIRE(test_2.getNumber() == 0);
  REQUIRE_FALSE(test_2.isInitialized());
  REQUIRE(test_2.getFanout() == chosen_fanout);

  // constructor with position, but no fanout
  // initialized false
  minhton::LogicalNodeInfo test_3(chosen_level, chosen_level);
  REQUIRE(test_3.getLevel() == chosen_level);
  REQUIRE(test_3.getNumber() == chosen_num);
  REQUIRE_FALSE(test_3.isInitialized());
  REQUIRE(test_3.getFanout() == 0);

  // constructor with position, but no fanout
  // initialized true
  minhton::LogicalNodeInfo test_4(chosen_level, chosen_level, (uint16_t)2);
  REQUIRE(test_4.getLevel() == chosen_level);
  REQUIRE(test_4.getNumber() == chosen_num);
  REQUIRE(test_4.isInitialized());
  REQUIRE(test_4.getFanout() == 2);

  // checking exceptions with invalid fanouts
  uint16_t under_min_fanout = 1;
  uint16_t over_max_fanout = kFanoutMaximum + 1;

  // empty constructor with invalid fanout
  // under minimum fanout
  REQUIRE_THROWS_AS(new minhton::LogicalNodeInfo(under_min_fanout), std::invalid_argument);

  // empty constructor with invalid fanout
  // over maximum fanout
  REQUIRE_THROWS_AS(new minhton::LogicalNodeInfo(over_max_fanout), std::invalid_argument);

  // normal constructor with invalid fanout
  // under minimum fanout
  REQUIRE_THROWS_AS(new minhton::LogicalNodeInfo(1, 0, under_min_fanout), std::invalid_argument);

  // normal constructor with invalid fanout
  // over maximum fanout
  REQUIRE_THROWS_AS(new minhton::LogicalNodeInfo(1, 0, over_max_fanout), std::invalid_argument);

  // checking exceptions with invalid positions with the normal constructor
  REQUIRE_THROWS_AS(new minhton::LogicalNodeInfo(0, 1, kFanoutDefault), std::invalid_argument);
}

TEST_CASE("LogicalNodeInfo Setter/Getter PeerToString",
          "[LogicalNodeInfo][Attributes][PeerToString]") {
  minhton::LogicalNodeInfo test(3);
  test.setPosition(1, 2);
  REQUIRE(test.getString().find("1:2 | m=3") != std::string::npos);
}

TEST_CASE("LogicalNodeInfo Setter/Getter Fanout", "[LogicalNodeInfo][Attributes][Fanout]") {
  minhton::LogicalNodeInfo peer;

  SECTION("Valid Fanouts") {
    peer.setFanout(kFanoutDefault);
    REQUIRE(peer.getFanout() == kFanoutDefault);

    peer.setFanout(kFanoutMinimum);
    REQUIRE(peer.getFanout() == kFanoutMinimum);

    peer.setFanout(kFanoutMaximum);
    REQUIRE(peer.getFanout() == kFanoutMaximum);

    peer.setFanout(kFanoutDefault + 10);
    REQUIRE(peer.getFanout() == kFanoutDefault + 10);
  }

  SECTION("Invalid Fanouts lower bound") {
    REQUIRE_THROWS_AS(peer.setFanout(kFanoutMinimum - 1), std::invalid_argument);
  }

  SECTION("Invalid Fanouts upper bound") {
    REQUIRE_THROWS_AS(peer.setFanout(kFanoutMaximum + 1), std::invalid_argument);
  }
}

TEST_CASE("LogicalNodeInfo setPosition, Getter Level/Number",
          "[LogicalNodeInfo][Method][setPosition]") {
  // fanout is set
  // setPosition with valid fanout throws no exception
  minhton::LogicalNodeInfo test_1(kFanoutDefault);
  REQUIRE(test_1.getLevel() == 0);
  REQUIRE(test_1.getNumber() == 0);

  REQUIRE_NOTHROW(test_1.setPosition(1, 1));
  REQUIRE(test_1.getLevel() == 1);
  REQUIRE(test_1.getNumber() == 1);

  // invalid position
  // position wont cange if setPosition throws eception
  minhton::LogicalNodeInfo test_2(kFanoutDefault);
  test_2.setPosition(0, 0);

  REQUIRE_THROWS_AS(test_2.setPosition(0, 1), std::invalid_argument);
  REQUIRE(test_2.getLevel() == 0);
  REQUIRE(test_2.getNumber() == 0);

  // some other valid positions
  minhton::LogicalNodeInfo test_3(kFanoutDefault);

  REQUIRE_NOTHROW(test_3.setPosition(7, 0));
  REQUIRE(test_3.getLevel() == 7);
  REQUIRE(test_3.getNumber() == 0);

  REQUIRE_NOTHROW(test_3.setPosition(4, 15));
  REQUIRE(test_3.getLevel() == 4);
  REQUIRE(test_3.getNumber() == 15);

  REQUIRE_NOTHROW(test_3.setPosition(10, 555));
  REQUIRE(test_3.getLevel() == 10);
  REQUIRE(test_3.getNumber() == 555);

  test_3.setPosition(0, 0);
  test_3.setFanout(5);

  REQUIRE_NOTHROW(test_3.setPosition(2, 24));
  REQUIRE(test_3.getLevel() == 2);
  REQUIRE(test_3.getNumber() == 24);

  REQUIRE_NOTHROW(test_3.setPosition(3, 124));
  REQUIRE(test_3.getLevel() == 3);
  REQUIRE(test_3.getNumber() == 124);

  REQUIRE_NOTHROW(test_3.setFanout(6));
  REQUIRE_NOTHROW(test_3.setPosition(2, 35));
  REQUIRE(test_3.getLevel() == 2);
  REQUIRE(test_3.getNumber() == 35);
}

TEST_CASE("LogicalNodeInfo setPosition through LogicalNodeInfo",
          "[LogicalNodeInfo][Method][setPosition]") {
  minhton::LogicalNodeInfo peer1;
  minhton::LogicalNodeInfo peer2(1, 1, kFanoutDefault);

  peer1.setPosition(peer2);
  REQUIRE(peer1.getLevel() == peer2.getLevel());
  REQUIRE(peer1.getNumber() == peer2.getNumber());
  REQUIRE(peer1.getFanout() == peer2.getFanout());
  REQUIRE(peer1.isInitialized() == peer2.isInitialized());
}

TEST_CASE("LogicalNodeInfo isInitialized", "[LogicalNodeInfo][Attributes][initialized]") {
  minhton::LogicalNodeInfo test_1(2);
  REQUIRE_FALSE(test_1.isInitialized());
  test_1.setPosition(1, 1);
  REQUIRE_FALSE(test_1.isInitialized());

  // constructor with initalized set false as default value
  minhton::LogicalNodeInfo test_3(1, 1);
  REQUIRE_FALSE(test_3.isInitialized());

  // constructor with initalized set true in normal constructor with fanout
  minhton::LogicalNodeInfo test_5(1, 1, (uint16_t)2);
  REQUIRE(test_5.isInitialized());
}

TEST_CASE("LogicalNodeInfo isSameLevel", "[LogicalNodeInfo][Method][isSameLevel]") {
  SECTION("Both peers not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0);
    minhton::LogicalNodeInfo peer2(0, 0);
    REQUIRE_FALSE(peer1.isSameLevel(peer2));
  }

  SECTION("First peer not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0);
    minhton::LogicalNodeInfo peer2(0, 0, (uint16_t)2);
    REQUIRE_FALSE(peer1.isSameLevel(peer2));
  }

  SECTION("Second peer not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0, (uint16_t)2);
    minhton::LogicalNodeInfo peer2(0, 0);
    REQUIRE_FALSE(peer1.isSameLevel(peer2));
  }

  SECTION("Both initialized and same level") {
    minhton::LogicalNodeInfo peer1(0, 0, (uint16_t)2);
    minhton::LogicalNodeInfo peer2(0, 0, (uint16_t)2);
    REQUIRE(peer1.isSameLevel(peer2));

    peer1.setFanout(kFanoutDefault);
    peer2.setFanout(kFanoutDefault);
    peer1.setPosition(1, 0);
    peer2.setPosition(1, 0);
    REQUIRE(peer1.isSameLevel(peer2));
  }

  SECTION("Both initialized and different levels") {
    minhton::LogicalNodeInfo peer1(0, 0, (uint16_t)2);
    minhton::LogicalNodeInfo peer2(1, 1, (uint16_t)2);
    REQUIRE_FALSE(peer1.isSameLevel(peer2));

    peer1.setFanout(kFanoutDefault);
    peer2.setFanout(kFanoutDefault);
    peer1.setPosition(1, 1);
    peer2.setPosition(2, 2);
    REQUIRE_FALSE(peer1.isSameLevel(peer2));
  }

  SECTION("Both initialized and different levels with normal constructor") {
    minhton::LogicalNodeInfo peer1(0, 0, kFanoutDefault);
    minhton::LogicalNodeInfo peer2(1, 1, kFanoutDefault);
    REQUIRE_FALSE(peer1.isSameLevel(peer2));

    peer1.setPosition(1, 1);
    peer2.setPosition(2, 2);
    REQUIRE_FALSE(peer1.isSameLevel(peer2));
  }
}

TEST_CASE("LogicalNodeInfo isDeeperThan", "[LogicalNodeInfo][Method][isDeeperThan]") {
  SECTION("Both peers not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0);
    minhton::LogicalNodeInfo peer2(0, 0);
    REQUIRE_FALSE(peer1.isDeeperThan(peer2));
  }

  SECTION("First peer not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0);
    minhton::LogicalNodeInfo peer2(0, 0, (uint16_t)2);
    REQUIRE_FALSE(peer1.isDeeperThan(peer2));
  }

  SECTION("Second peer not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0, (uint16_t)2);
    minhton::LogicalNodeInfo peer2(0, 0);
    REQUIRE_FALSE(peer1.isDeeperThan(peer2));
  }

  SECTION("Both initialized and same level") {
    minhton::LogicalNodeInfo peer1(0, 0, (uint16_t)2);
    minhton::LogicalNodeInfo peer2(0, 0, (uint16_t)2);
    REQUIRE_FALSE(peer1.isDeeperThan(peer2));

    peer1.setFanout(kFanoutDefault);
    peer2.setFanout(kFanoutDefault);
    peer1.setPosition(1, 0);
    peer2.setPosition(1, 0);
    REQUIRE_FALSE(peer1.isDeeperThan(peer2));
  }

  SECTION("Both initialized with normal constructor and level variations") {
    minhton::LogicalNodeInfo peer1(0, 0, kFanoutDefault);
    minhton::LogicalNodeInfo peer2(1, 1, kFanoutDefault);
    REQUIRE_FALSE(peer1.isDeeperThan(peer2));

    peer1.setPosition(1, 1);
    REQUIRE_FALSE(peer1.isDeeperThan(peer2));

    peer1.setPosition(2, 0);
    REQUIRE(peer1.isDeeperThan(peer2));
    REQUIRE_FALSE(peer2.isDeeperThan(peer1));

    peer2.setPosition(14, 0);
    REQUIRE_FALSE(peer1.isDeeperThan(peer2));
    REQUIRE(peer2.isDeeperThan(peer1));
  }
}

TEST_CASE("LogicalNodeInfo isDeeperThanOrSameLevel",
          "[LogicalNodeInfo][Method][isDeeperThanOrSameLevel]") {
  SECTION("Both peers not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0);
    minhton::LogicalNodeInfo peer2(0, 0);
    REQUIRE_FALSE(peer1.isDeeperThanOrSameLevel(peer2));
  }

  SECTION("First peer not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0);
    minhton::LogicalNodeInfo peer2(0, 0, (uint16_t)2);
    REQUIRE_FALSE(peer1.isDeeperThanOrSameLevel(peer2));
  }

  SECTION("Second peer not initialized") {
    minhton::LogicalNodeInfo peer1(0, 0, (uint16_t)2);
    minhton::LogicalNodeInfo peer2(0, 0);
    REQUIRE_FALSE(peer1.isDeeperThanOrSameLevel(peer2));
  }

  SECTION("Both initialized and same level") {
    minhton::LogicalNodeInfo peer1(0, 0, (uint16_t)2);
    minhton::LogicalNodeInfo peer2(0, 0, (uint16_t)2);
    REQUIRE(peer1.isDeeperThanOrSameLevel(peer2));

    peer1.setFanout(kFanoutDefault);
    peer2.setFanout(kFanoutDefault);
    peer1.setPosition(1, 0);
    peer2.setPosition(1, 0);
    REQUIRE(peer1.isDeeperThanOrSameLevel(peer2));
  }

  SECTION("Both initialized with normal constructor and level variations") {
    minhton::LogicalNodeInfo peer1(0, 0, kFanoutDefault);
    minhton::LogicalNodeInfo peer2(1, 1, kFanoutDefault);
    REQUIRE_FALSE(peer1.isDeeperThanOrSameLevel(peer2));
    REQUIRE(peer2.isDeeperThanOrSameLevel(peer1));

    peer1.setPosition(1, 1);
    REQUIRE(peer1.isDeeperThanOrSameLevel(peer2));
    REQUIRE(peer2.isDeeperThanOrSameLevel(peer1));

    peer1.setPosition(2, 0);
    REQUIRE(peer1.isDeeperThanOrSameLevel(peer2));
    REQUIRE_FALSE(peer2.isDeeperThanOrSameLevel(peer1));

    peer2.setPosition(14, 0);
    REQUIRE_FALSE(peer1.isDeeperThanOrSameLevel(peer2));
    REQUIRE(peer2.isDeeperThanOrSameLevel(peer1));
  }
}

TEST_CASE("LogicalNodeInfo isRoot", "[LogicalNodeInfo][Method][isRoot]") {
  minhton::LogicalNodeInfo peer(0, 0, kFanoutDefault);
  REQUIRE(peer.isRoot());

  peer.setPosition(1, 0);
  REQUIRE_FALSE(peer.isRoot());

  peer.setPosition(1, 1);
  REQUIRE_FALSE(peer.isRoot());

  peer.setPosition(2, 0);
  REQUIRE_FALSE(peer.isRoot());

  peer.setPosition(3, 1);
  REQUIRE_FALSE(peer.isRoot());

  peer.setPosition(0, 0);
  REQUIRE(peer.isRoot());
}

TEST_CASE("LogicalNodeInfo LogicalNodeInfoHasher", "[LogicalNodeInfo][LogicalNodeInfoHasher]") {
  minhton::LogicalNodeInfo peer_1(0, 0, (uint16_t)2);
  minhton::LogicalNodeInfo peer_2(1, 0, (uint16_t)2);
  minhton::LogicalNodeInfo peer_3(1, 1, (uint16_t)2);
  minhton::LogicalNodeInfo peer_4(1, 1, (uint16_t)3);
  minhton::LogicalNodeInfo peer_5(1, 1);

  std::unordered_map<minhton::LogicalNodeInfo, int, LogicalNodeInfoHasher> test_map =
      std::unordered_map<minhton::LogicalNodeInfo, int, LogicalNodeInfoHasher>();
  test_map[peer_1] = 1;
  test_map[peer_2] = 2;
  test_map[peer_3] = 3;
  test_map[peer_4] = 4;
  test_map[peer_5] = 5;

  REQUIRE(test_map[peer_1] == 1);
  REQUIRE(test_map[peer_2] == 2);
  REQUIRE(test_map[peer_3] == 3);
  REQUIRE(test_map[peer_4] == 4);
  REQUIRE(test_map[peer_5] == 5);

  minhton::LogicalNodeInfoHasher hasher = minhton::LogicalNodeInfoHasher();

  REQUIRE(hasher(peer_1) == hasher(peer_1));
  REQUIRE(hasher(peer_1) != hasher(peer_2));
  REQUIRE(hasher(peer_1) != hasher(peer_3));
  REQUIRE(hasher(peer_1) != hasher(peer_4));
  REQUIRE(hasher(peer_1) != hasher(peer_5));
  REQUIRE(hasher(peer_2) == hasher(peer_2));
  REQUIRE(hasher(peer_2) != hasher(peer_3));
  REQUIRE(hasher(peer_2) != hasher(peer_4));
  REQUIRE(hasher(peer_2) != hasher(peer_5));
  REQUIRE(hasher(peer_3) == hasher(peer_3));
  REQUIRE(hasher(peer_3) != hasher(peer_4));
  REQUIRE(hasher(peer_3) != hasher(peer_5));
  REQUIRE(hasher(peer_4) == hasher(peer_4));
  REQUIRE(hasher(peer_4) != hasher(peer_5));
  REQUIRE(hasher(peer_5) == hasher(peer_5));
}

TEST_CASE("LogicalNodeInfo Equal Unequal", "[LogicalNodeInfo][Method][Equal, Unequal]") {
  minhton::LogicalNodeInfo p1(7, 0);
  minhton::LogicalNodeInfo p2(0, 0);
  minhton::LogicalNodeInfo p3(1, 0);
  minhton::LogicalNodeInfo p4(1, 1);

  minhton::LogicalNodeInfo p5(0, 0, (uint16_t)3);
  minhton::LogicalNodeInfo p6(1, 0, (uint16_t)3);
  minhton::LogicalNodeInfo p7(1, 1, (uint16_t)3);

  minhton::LogicalNodeInfo p8(0, 0, (uint16_t)15);
  minhton::LogicalNodeInfo p9(1, 0, (uint16_t)15);
  minhton::LogicalNodeInfo p10(1, 1, (uint16_t)15);

  REQUIRE(p1 == p1);
  REQUIRE(p2 == p2);
  REQUIRE(p3 == p3);
  REQUIRE(p4 == p4);
  REQUIRE(p5 == p5);
  REQUIRE(p6 == p6);
  REQUIRE(p7 == p7);
  REQUIRE(p8 == p8);
  REQUIRE(p9 == p9);
  REQUIRE(p10 == p10);

  REQUIRE(p1 != p2);
  REQUIRE(p1 != p3);
  REQUIRE(p1 != p4);
  REQUIRE(p1 != p5);
  REQUIRE(p1 != p6);
  REQUIRE(p1 != p7);
  REQUIRE(p1 != p8);
  REQUIRE(p1 != p9);
  REQUIRE(p1 != p10);
  REQUIRE(p2 != p3);
  REQUIRE(p2 != p4);
  REQUIRE(p2 != p5);
  REQUIRE(p2 != p6);
  REQUIRE(p2 != p7);
  REQUIRE(p2 != p8);
  REQUIRE(p2 != p9);
  REQUIRE(p2 != p10);
  REQUIRE(p3 != p4);
  REQUIRE(p3 != p5);
  REQUIRE(p3 != p6);
  REQUIRE(p3 != p7);
  REQUIRE(p3 != p8);
  REQUIRE(p3 != p9);
  REQUIRE(p3 != p10);
  REQUIRE(p3 != p5);
  REQUIRE(p4 != p6);
  REQUIRE(p4 != p7);
  REQUIRE(p4 != p8);
  REQUIRE(p4 != p9);
  REQUIRE(p4 != p10);
  REQUIRE(p5 != p6);
  REQUIRE(p5 != p7);
  REQUIRE(p5 != p8);
  REQUIRE(p5 != p9);
  REQUIRE(p5 != p10);
  REQUIRE(p6 != p7);
  REQUIRE(p6 != p8);
  REQUIRE(p6 != p9);
  REQUIRE(p6 != p10);
  REQUIRE(p7 != p8);
  REQUIRE(p7 != p9);
  REQUIRE(p7 != p10);
  REQUIRE(p8 != p9);
  REQUIRE(p8 != p10);
  REQUIRE(p9 != p10);
}
