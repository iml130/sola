// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "core/node_info.h"

#include <catch2/catch_test_macros.hpp>
#include <unordered_map>
using namespace minhton;

TEST_CASE("NodeInfo Constructor", "[NodeInfo][Init]") {
  // empty constructor
  minhton::NodeInfo test;
  REQUIRE(test.getLogicalNodeInfo().getLevel() == 0);
  REQUIRE(test.getLogicalNodeInfo().getNumber() == 0);
  REQUIRE_FALSE(test.isValidPeer());
  REQUIRE(test.getPhysicalNodeInfo().getAddress() == "");
  REQUIRE(test.getPhysicalNodeInfo().getPort() == 0);
  REQUIRE_FALSE(test.getPhysicalNodeInfo().isInitialized());
  REQUIRE_FALSE(test.isInitialized());

  // constructor with level and number
  minhton::NodeInfo test_2(1, 1, 2);
  REQUIRE(test_2.getLogicalNodeInfo().getLevel() == 1);
  REQUIRE(test_2.getLogicalNodeInfo().getNumber() == 1);
  REQUIRE(test_2.isValidPeer());
  REQUIRE(test_2.getPhysicalNodeInfo().getAddress() == "");
  REQUIRE(test_2.getPhysicalNodeInfo().getPort() == 0);
  REQUIRE_FALSE(test_2.getPhysicalNodeInfo().isInitialized());
  REQUIRE_FALSE(test_2.isInitialized());

  // constructor with invalid arguments
  REQUIRE_THROWS_AS(new minhton::NodeInfo(0, 0, 0), std::invalid_argument);
  REQUIRE_THROWS_AS(new minhton::NodeInfo(0, 0, 1), std::invalid_argument);
  REQUIRE_THROWS_AS(new minhton::NodeInfo(0, 1, 2), std::invalid_argument);
}

TEST_CASE("NodeInfo getLogicalNodeInfo setLogicalNodeInfo",
          "[NodeInfo][Method][getLogicalNodeInfo setLogicalNodeInfo") {
  minhton::NodeInfo test;
  minhton::LogicalNodeInfo peer(2, 14, (uint16_t)5);

  // with initialized peer
  test.setLogicalNodeInfo(peer);
  REQUIRE(test.getLogicalNodeInfo().getLevel() == peer.getLevel());
  REQUIRE(test.getLogicalNodeInfo().getNumber() == peer.getNumber());
  REQUIRE(test.getLogicalNodeInfo().getFanout() == peer.getFanout());
  REQUIRE(test.isValidPeer() == peer.isInitialized());
  REQUIRE(test.isValidPeer() == peer.isInitialized());

  // with uninitialized peer
  minhton::NodeInfo test_2;
  minhton::LogicalNodeInfo peer_2;
  test_2.setLogicalNodeInfo(peer_2);
  REQUIRE(test_2.getLogicalNodeInfo().getLevel() == peer_2.getLevel());
  REQUIRE(test_2.getLogicalNodeInfo().getNumber() == peer_2.getNumber());
  REQUIRE(test_2.getLogicalNodeInfo().getFanout() == peer_2.getFanout());
  REQUIRE(test_2.isValidPeer() == peer_2.isInitialized());
  REQUIRE(test_2.isValidPeer() == peer_2.isInitialized());
}

TEST_CASE("NodeInfo getPhysicalNodeInfo setPhysicalNodeInfo",
          "[NodeInfo][Method][getPhysicalNodeInfo setPhysicalNodeInfo") {
  minhton::NodeInfo test;
  minhton::PhysicalNodeInfo net("1.2.3.4", 3000);

  // with initialized PhysicalNodeInfo
  test.setPhysicalNodeInfo(net);
  REQUIRE(test.getPhysicalNodeInfo().getAddress() == net.getAddress());
  REQUIRE(test.getPhysicalNodeInfo().getPort() == net.getPort());
  REQUIRE(test.getPhysicalNodeInfo().isInitialized() == net.isInitialized());

  // with uninitialized PhysicalNodeInfo
  minhton::NodeInfo test_2;
  minhton::PhysicalNodeInfo net_2;
  test_2.setPhysicalNodeInfo(net_2);
  REQUIRE(test_2.getPhysicalNodeInfo().getAddress() == net_2.getAddress());
  REQUIRE(test_2.getPhysicalNodeInfo().getPort() == net_2.getPort());
  REQUIRE(test_2.getPhysicalNodeInfo().isInitialized() == net_2.isInitialized());
}

TEST_CASE("NodeInfo isInitialized", "[NodeInfo][Method][isInitialized") {
  minhton::NodeInfo test;

  minhton::PhysicalNodeInfo net_uninit;
  minhton::PhysicalNodeInfo net_init("1.2.3.4", 2098);
  REQUIRE_FALSE(net_uninit.isInitialized());
  REQUIRE(net_init.isInitialized());

  minhton::LogicalNodeInfo peer_uninit;
  minhton::LogicalNodeInfo peer_init(1, 2, (uint16_t)3);
  REQUIRE_FALSE(peer_uninit.isInitialized());
  REQUIRE(peer_init.isInitialized());

  // uninit + uninit = uninit
  test.setPhysicalNodeInfo(net_uninit);
  test.setLogicalNodeInfo(peer_uninit);
  REQUIRE_FALSE(test.isInitialized());

  // uninit + init = uninit
  test.setPhysicalNodeInfo(net_uninit);
  test.setLogicalNodeInfo(peer_init);
  REQUIRE_FALSE(test.isInitialized());

  // init + uninit = uninit
  test.setPhysicalNodeInfo(net_init);
  test.setLogicalNodeInfo(peer_uninit);
  REQUIRE_FALSE(test.isInitialized());

  // init + init = init
  test.setPhysicalNodeInfo(net_init);
  test.setLogicalNodeInfo(peer_init);
  REQUIRE(test.isInitialized());
}

TEST_CASE("NodeInfo NodeInfoHasher", "[NodeInfo][NodeInfoHasher]") {
  minhton::NodeInfo node_1(0, 0, (uint16_t)2, "1.2.3.4", 2000);
  minhton::NodeInfo node_2(0, 0, (uint16_t)2, "1.2.3.5", 2000);
  minhton::NodeInfo node_3(1, 0, (uint16_t)2, "1.2.3.5", 2000);
  minhton::NodeInfo node_4(1, 0, (uint16_t)3, "1.2.3.5", 2000);
  minhton::NodeInfo node_5(1, 0, (uint16_t)3, "1.2.3.5", 2001);

  std::unordered_map<minhton::NodeInfo, int, NodeInfoHasher> test_map =
      std::unordered_map<minhton::NodeInfo, int, NodeInfoHasher>();
  test_map[node_1] = 1;
  test_map[node_2] = 2;
  test_map[node_3] = 3;
  test_map[node_4] = 4;
  test_map[node_5] = 5;

  REQUIRE(test_map[node_1] == 1);
  REQUIRE(test_map[node_2] == 2);
  REQUIRE(test_map[node_3] == 3);
  REQUIRE(test_map[node_4] == 4);
  REQUIRE(test_map[node_5] == 5);

  minhton::NodeInfoHasher hasher = minhton::NodeInfoHasher();

  REQUIRE(hasher(node_1) == hasher(node_1));
  REQUIRE(hasher(node_1) != hasher(node_2));
  REQUIRE(hasher(node_1) != hasher(node_3));
  REQUIRE(hasher(node_1) != hasher(node_4));
  REQUIRE(hasher(node_1) != hasher(node_5));
  REQUIRE(hasher(node_2) == hasher(node_2));
  REQUIRE(hasher(node_2) != hasher(node_3));
  REQUIRE(hasher(node_2) != hasher(node_4));
  REQUIRE(hasher(node_2) != hasher(node_5));
  REQUIRE(hasher(node_3) == hasher(node_3));
  REQUIRE(hasher(node_3) != hasher(node_4));
  REQUIRE(hasher(node_3) != hasher(node_5));
  REQUIRE(hasher(node_4) == hasher(node_4));
  REQUIRE(hasher(node_4) != hasher(node_5));
  REQUIRE(hasher(node_5) == hasher(node_5));
}

TEST_CASE("NodeInfo Equal, Unequal", "[NodeInfo][Equal, Unequal]") {
  minhton::NodeInfo p1(0, 0, (uint16_t)2, "1.2.3.4", 2000);
  minhton::NodeInfo p2(1, 0, (uint16_t)2, "1.2.3.5", 2000);
  minhton::NodeInfo p3(0, 0, (uint16_t)2, "1.2.3.4", 2100);
  minhton::NodeInfo p4(0, 0, (uint16_t)8, "1.7.3.4", 2010);
  minhton::NodeInfo p5(7, 5, (uint16_t)6, "1.2.8.4", 2000);
  minhton::NodeInfo p6(0, 0, (uint16_t)5, "1.2.3.4", 2000);
  minhton::NodeInfo p7(0, 0, (uint16_t)30, "1.2.2.4", 2100);
  minhton::NodeInfo p8(1, 0, (uint16_t)3, "1.2.3.4", 2000);
  minhton::NodeInfo p9(4, 0, (uint16_t)3, "1.2.3.4", 2004);
  minhton::NodeInfo p10(2, 5, (uint16_t)3, "1.2.3.4", 2200);

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
