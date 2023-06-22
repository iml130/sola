// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

#include "core/routing_information.h"

using namespace minhton;

TEST_CASE("RoutingInformationTable getRoutingTableNeighbors",
          "[RoutingInformationTable][Method][getRoutingTableNeighbors]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node(2, 1, fanout);
  minhton::RoutingInformation routing_info(node, Logger());

  REQUIRE(routing_info.getRoutingTableNeighbors().size() == 3);

  REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo() ==
          minhton::LogicalNodeInfo(2, 0, fanout));
  REQUIRE(routing_info.getRoutingTableNeighbors()[0].isValidPeer());
  REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[0].isInitialized());

  REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo() ==
          minhton::LogicalNodeInfo(2, 2, fanout));
  REQUIRE(routing_info.getRoutingTableNeighbors()[1].isValidPeer());
  REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[1].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo() ==
          minhton::LogicalNodeInfo(2, 3, fanout));
  REQUIRE(routing_info.getRoutingTableNeighbors()[2].isValidPeer());
  REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[2].isInitialized());

  minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 1234);
  routing_info.updateRoutingTableNeighbor(node_2_0);

  REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo() ==
          minhton::LogicalNodeInfo(2, 0, fanout));
  REQUIRE(routing_info.getRoutingTableNeighbors()[0].isValidPeer());
  REQUIRE(routing_info.getRoutingTableNeighbors()[0].getNetworkInfo().isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighbors()[0].isInitialized());
}

TEST_CASE("RoutingInformationTable getRoutingTableNeighborChildren",
          "[RoutingInformationTable][Method][getRoutingTableNeighborChildren]") {
  uint16_t fanout = 3;

  minhton::NodeInfo node(2, 2, fanout);
  minhton::RoutingInformation routing_info(node, Logger());

  // (2:0), (2:1), (2:3), (2:4), (2:5), (2:8)

  REQUIRE(routing_info.getRoutingTableNeighborChildren().size() == (2 + 4) * fanout);

  REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 2].getLogicalNodeInfo() ==
          minhton::LogicalNodeInfo(3, 2, fanout));
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 2]
              .getLogicalNodeInfo()
              .isInitialized());
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 2]
                    .getNetworkInfo()
                    .isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[1 * fanout + 0].getLogicalNodeInfo() ==
          minhton::LogicalNodeInfo(3, 3, fanout));
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[1 * fanout + 0]
              .getLogicalNodeInfo()
              .isInitialized());
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[1 * fanout + 0]
                    .getNetworkInfo()
                    .isInitialized());
}

TEST_CASE("RoutingInformation updateRoutingTableNeighbor",
          "[RoutingInformation][Methods][updateRoutingTableNeighbor]") {
  minhton::RoutingInformation routing_info_empty;
  REQUIRE_THROWS_AS(routing_info_empty.updateRoutingTableNeighbor(minhton::NodeInfo()),
                    std::logic_error);

  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  // uninitialized
  REQUIRE_THROWS_AS(routing_info.updateRoutingTableNeighbor(minhton::NodeInfo()),
                    std::invalid_argument);

  // wrong fanout
  REQUIRE_THROWS_AS(routing_info.updateRoutingTableNeighbor(minhton::NodeInfo(0, 0, 2)),
                    std::invalid_argument);

  // everything fine
  minhton::NodeInfo neighbor_4(1, 0, 3, "1.2.3.4", 2000);
  REQUIRE_NOTHROW(routing_info.updateRoutingTableNeighbor(neighbor_4));
  REQUIRE(routing_info.getRoutingTableNeighbors()[0].isInitialized());
}

TEST_CASE("RoutingInformation updateRoutingTableNeighborChild",
          "[RoutingInformation][Methods][updateRoutingTableNeighborChild]") {
  minhton::RoutingInformation routing_info_empty;
  REQUIRE_THROWS_AS(routing_info_empty.updateRoutingTableNeighborChild(minhton::NodeInfo()),
                    std::logic_error);

  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  // uninitialized
  REQUIRE_THROWS_AS(routing_info.updateRoutingTableNeighborChild(minhton::NodeInfo()),
                    std::invalid_argument);

  // wrong fanout
  REQUIRE_THROWS_AS(routing_info.updateRoutingTableNeighborChild(minhton::NodeInfo(0, 0, 2)),
                    std::invalid_argument);

  // everything fine
  minhton::NodeInfo neighbor_4(2, 0, 3, "1.2.3.4", 2000);
  REQUIRE_NOTHROW(routing_info.updateRoutingTableNeighborChild(neighbor_4));
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[0].isInitialized());
}

TEST_CASE("RoutingInformationTable resetRoutingTableNeighbor",
          "[RoutingInformationTable][Method][resetRoutingTableNeighbor]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);

  minhton::RoutingInformation routing_info(node_2_1, Logger());
  routing_info.updateRoutingTableNeighbor(node_2_0);
  routing_info.updateRoutingTableNeighbor(node_2_2);
  routing_info.updateRoutingTableNeighbor(node_2_3);

  REQUIRE(routing_info.getRoutingTableNeighbors()[0].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighbors()[1].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighbors()[2].isInitialized());

  routing_info.resetRoutingTableNeighbor(node_2_0);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[0].isInitialized());

  routing_info.resetRoutingTableNeighbor(node_2_2);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[1].isInitialized());

  routing_info.resetRoutingTableNeighbor(node_2_3);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[2].isInitialized());
}

TEST_CASE("RoutingInformationTable resetRoutingTableNeighborChild",
          "[RoutingInformationTable][Method][resetRoutingTableNeighborChild]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node(3, 4, fanout, "1.2.3.4", 2000);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_4_0(4, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_1(4, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_2(4, 2, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_3(4, 3, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_4(4, 4, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_5(4, 5, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_6(4, 6, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_7(4, 7, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_8(4, 8, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_9(4, 9, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_10(4, 10, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_11(4, 11, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_12(4, 12, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_13(4, 13, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_14(4, 14, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_4_15(4, 15, fanout, "1.2.3.4", 2000);

  routing_info.updateNeighbor(node_4_0);
  routing_info.updateNeighbor(node_4_1);
  routing_info.updateNeighbor(node_4_4);
  routing_info.updateNeighbor(node_4_5);
  routing_info.updateNeighbor(node_4_6);
  routing_info.updateNeighbor(node_4_7);
  routing_info.updateNeighbor(node_4_8);
  routing_info.updateNeighbor(node_4_9);
  routing_info.updateNeighbor(node_4_10);
  routing_info.updateNeighbor(node_4_11);
  routing_info.updateNeighbor(node_4_12);
  routing_info.updateNeighbor(node_4_13);

  // (4:0), (4:1), (4:4), (4:5), (4:6), (4:7), (4:10), (4:11), (4:12), (4:13)

  bool uninit = false;
  for (auto const &rtc_node : routing_info.getRoutingTableNeighborChildren()) {
    if (!rtc_node.isInitialized()) {
      uninit = true;
    }
  }
  REQUIRE_FALSE(uninit);

  bool inside = false;
  routing_info.resetRoutingTableNeighborChild(node_4_0);
  for (auto const &rtc_node : routing_info.getRoutingTableNeighborChildren()) {
    if (rtc_node.isInitialized() &&
        rtc_node.getLogicalNodeInfo() == node_4_0.getLogicalNodeInfo()) {
      inside = true;
    }
  }
  REQUIRE_FALSE(inside);
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[0].getLogicalNodeInfo() ==
          node_4_0.getLogicalNodeInfo());
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[0].isInitialized());

  inside = false;
  routing_info.resetRoutingTableNeighborChild(node_4_5);
  for (auto const &rtc_node : routing_info.getRoutingTableNeighborChildren()) {
    if (rtc_node.isInitialized() &&
        rtc_node.getLogicalNodeInfo() == node_4_5.getLogicalNodeInfo()) {
      inside = true;
    }
  }
  REQUIRE_FALSE(inside);
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[3].getLogicalNodeInfo() ==
          node_4_5.getLogicalNodeInfo());
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[3].isInitialized());

  inside = false;
  routing_info.resetRoutingTableNeighborChild(node_4_13);
  for (auto const &rtc_node : routing_info.getRoutingTableNeighborChildren()) {
    if (rtc_node.isInitialized() &&
        rtc_node.getLogicalNodeInfo() == node_4_13.getLogicalNodeInfo()) {
      inside = true;
    }
  }
  REQUIRE_FALSE(inside);
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[9].getLogicalNodeInfo() ==
          node_4_13.getLogicalNodeInfo());
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[9].isInitialized());
}

TEST_CASE("RoutingInformationTable Constructor", "[RoutingInformationTable][Init]") {
  SECTION("Fanout 2") {
    uint16_t fanout = 2;

    minhton::NodeInfo node(3, 1, fanout);
    minhton::RoutingInformation routing_info(node, Logger());

    REQUIRE(routing_info.getRoutingTableNeighbors().size() == 4);
    REQUIRE(routing_info.getRoutingTableNeighborChildren().size() == 4 * fanout);

    // routing table neighbors of routing table neighbors in correct order: (3:0), (3:2),
    // (3 : 3),
    // (3:5)
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 0, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[0].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 2, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[1].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[1].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 3, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[2].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[2].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[3].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 5, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[3].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[3].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[3].isInitialized());

    // routing table neighbor children of 3:0 are 4:0 and 4:1
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(4, 0, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 0]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 0]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 0].isInitialized() ==
            false);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(4, 1, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 1]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 1]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0 * fanout + 1].isInitialized() ==
            false);

    // routing table neighbor children of 3:2 are 4:6 and 4:7
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2 * fanout + 0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(4, 6, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2 * fanout + 0]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2 * fanout + 0]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2 * fanout + 0].isInitialized() ==
            false);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2 * fanout + 1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(4, 7, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2 * fanout + 1]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2 * fanout + 1]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2 * fanout + 1].isInitialized() ==
            false);
  }

  SECTION("Fanout 3") {
    uint16_t fanout = 3;

    minhton::NodeInfo node(2, 8, fanout);
    minhton::RoutingInformation routing_info(node, Logger());

    REQUIRE(routing_info.getRoutingTableNeighbors().size() == 4);
    REQUIRE(routing_info.getRoutingTableNeighborChildren().size() == 4 * fanout);

    // routing table neighbors of left routing table neighbors in correct order: (2:2), (2:5),
    // (2:6), (2:7)
    REQUIRE(routing_info.getRoutingTableNeighbors()[3].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 7, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[3].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[3].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[3].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 6, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[2].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[2].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 5, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[1].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[1].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 2, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[0].isInitialized());

    // routing table neighbor children of 2:7 are 3:21, 3:22, 3:23
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 21, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 0]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 0]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 0].isInitialized() ==
            false);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 22, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 1]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 1]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 1].isInitialized() ==
            false);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 2].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 23, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 2]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 2]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3 * fanout + 2].isInitialized() ==
            false);
  }

  SECTION("Fanout 4") {
    uint16_t fanout = 4;

    minhton::NodeInfo node(1, 1, fanout);
    minhton::RoutingInformation routing_info(node, Logger());

    REQUIRE(routing_info.getRoutingTableNeighbors().size() == 3);
    REQUIRE(routing_info.getRoutingTableNeighborChildren().size() == 3 * fanout);

    // routing table neighbors of in correct order: (1:0), (1:2), (1:3)
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(1, 0, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[0].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(1, 2, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[1].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[1].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(1, 3, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[2].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[2].isInitialized());

    // routing table neighbor children of 1:0 are 2:0, 2:1, 2:2, 2:3
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 0, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0].getLogicalNodeInfo().getFanout() ==
            fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[0].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 1, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].getLogicalNodeInfo().getFanout() ==
            fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[1].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 2, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2].getLogicalNodeInfo().getFanout() ==
            fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[2].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 3, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3].getLogicalNodeInfo().getFanout() ==
            fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[3].isInitialized());
  }

  SECTION("Fanout 5") {
    uint16_t fanout = 5;

    minhton::NodeInfo node(2, 14, fanout);
    minhton::RoutingInformation routing_info(node, Logger());

    REQUIRE(routing_info.getRoutingTableNeighbors().size() == 12);
    REQUIRE(routing_info.getRoutingTableNeighborChildren().size() == 12 * fanout);

    // correct order: (2:4), (2:9), (2:10), (2:11), (2:12), (2:13), (2:15), (2:16), (2:17),
    // (2 : 18),
    // (2:19), (2:24)
    REQUIRE(routing_info.getRoutingTableNeighbors()[5].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 13, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[5].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[5].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[5].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[4].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 12, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[4].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[4].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[4].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[3].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 11, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[3].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[3].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[3].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 10, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[2].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[2].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 9, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[1].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[1].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[1].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 4, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[0].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[6].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 15, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[6].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[6].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[6].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[7].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 16, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[7].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[7].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[7].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[8].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 17, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[8].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[8].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[8].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[9].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 18, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[9].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[9].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[9].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[10].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 19, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[10].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[10].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[10].isInitialized());

    REQUIRE(routing_info.getRoutingTableNeighbors()[11].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(2, 24, fanout));
    REQUIRE(routing_info.getRoutingTableNeighbors()[11].getLogicalNodeInfo().getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighbors()[11].isValidPeer());
    REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[11].isInitialized());

    // routing table neighbor children of 2:15 are 3:75, 3:76, 3:77, 3:78, 3:79
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 0].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 75, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 0]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 0]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 0].isInitialized() ==
            false);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 1].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 76, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 1]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 1]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 1].isInitialized() ==
            false);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 2].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 77, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 2]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 2]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 2].isInitialized() ==
            false);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 3].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 78, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 3]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 3]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 3].isInitialized() ==
            false);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 4].getLogicalNodeInfo() ==
            minhton::LogicalNodeInfo(3, 79, fanout));
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 4]
                .getLogicalNodeInfo()
                .getFanout() == fanout);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 4]
                .getLogicalNodeInfo()
                .isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6 * fanout + 4].isInitialized() ==
            false);
  }
}

TEST_CASE("RoutingInformationTable getRoutingTableNeighbors Left/Right",
          "[RoutingInformationTable][Method][getRoutingTableNeighbors]") {
  SECTION("Fanout 2") {
    uint16_t fanout = 2;

    minhton::NodeInfo node(3, 4, fanout, "1.2.3.4", 2000);
    minhton::RoutingInformation routing_info(node, Logger());

    auto left_neighbors = routing_info.getLeftRoutingTableNeighborsLeftToRight();
    auto left_neighbors_rev = routing_info.getLeftRoutingTableNeighborsRightToLeft();
    auto right_neighbors = routing_info.getRightRoutingTableNeighborsLeftToRight();
    auto right_neighbors_rev = routing_info.getRightRoutingTableNeighborsRightToLeft();

    REQUIRE(left_neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 0, fanout));
    REQUIRE(left_neighbors[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 2, fanout));
    REQUIRE(left_neighbors[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 3, fanout));

    REQUIRE(left_neighbors_rev[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 0, fanout));
    REQUIRE(left_neighbors_rev[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 2, fanout));
    REQUIRE(left_neighbors_rev[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 3, fanout));

    REQUIRE(right_neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 5, fanout));
    REQUIRE(right_neighbors[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 6, fanout));

    REQUIRE(right_neighbors_rev[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 5, fanout));
    REQUIRE(right_neighbors_rev[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 6, fanout));

    REQUIRE(left_neighbors.size() == 3);       // (3:1), (3:2), (3:3)
    REQUIRE(left_neighbors_rev.size() == 3);   // (3:1), (3:2), (3:3)
    REQUIRE(right_neighbors.size() == 2);      // (3:5), (3:6)
    REQUIRE(right_neighbors_rev.size() == 2);  // (3:5), (3:6)
    REQUIRE(left_neighbors.size() + right_neighbors.size() ==
            routing_info.getRoutingTableNeighbors().size());

    for (auto const &left : left_neighbors) {
      REQUIRE(left < node);
    }
    for (auto const &right : right_neighbors) {
      REQUIRE(right > node);
    }
  }

  SECTION("Fanout 3") {
    uint16_t fanout = 3;

    minhton::NodeInfo node(3, 13, fanout, "1.2.3.4", 2000);
    minhton::RoutingInformation routing_info(node, Logger());

    auto left_neighbors = routing_info.getLeftRoutingTableNeighborsLeftToRight();
    auto left_neighbors_rev = routing_info.getLeftRoutingTableNeighborsRightToLeft();
    auto right_neighbors = routing_info.getRightRoutingTableNeighborsLeftToRight();
    auto right_neighbors_rev = routing_info.getRightRoutingTableNeighborsRightToLeft();

    REQUIRE(left_neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 4, fanout));
    REQUIRE(left_neighbors[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 7, fanout));
    REQUIRE(left_neighbors[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 10, fanout));
    REQUIRE(left_neighbors[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 11, fanout));
    REQUIRE(left_neighbors[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 12, fanout));

    REQUIRE(left_neighbors_rev[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 4, fanout));
    REQUIRE(left_neighbors_rev[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 7, fanout));
    REQUIRE(left_neighbors_rev[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 10, fanout));
    REQUIRE(left_neighbors_rev[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 11, fanout));
    REQUIRE(left_neighbors_rev[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 12, fanout));

    REQUIRE(right_neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 14, fanout));
    REQUIRE(right_neighbors[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 15, fanout));
    REQUIRE(right_neighbors[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 16, fanout));
    REQUIRE(right_neighbors[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 19, fanout));
    REQUIRE(right_neighbors[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 22, fanout));

    REQUIRE(right_neighbors_rev[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 14, fanout));
    REQUIRE(right_neighbors_rev[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 15, fanout));
    REQUIRE(right_neighbors_rev[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 16, fanout));
    REQUIRE(right_neighbors_rev[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 19, fanout));
    REQUIRE(right_neighbors_rev[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 22, fanout));

    REQUIRE(left_neighbors.size() == 5);
    REQUIRE(left_neighbors_rev.size() == 5);
    REQUIRE(right_neighbors.size() == 5);
    REQUIRE(right_neighbors_rev.size() == 5);
    REQUIRE(left_neighbors.size() + right_neighbors.size() ==
            routing_info.getRoutingTableNeighbors().size());

    for (auto const &left : left_neighbors) {
      REQUIRE(left < node);
    }
    for (auto const &right : right_neighbors) {
      REQUIRE(right > node);
    }
  }
}

TEST_CASE("RoutingInformationTable getRoutingTableNeighborChildren Left/Right",
          "[RoutingInformationTable][Method][getRoutingTableNeighborChildren]") {
  SECTION("Fanout 2") {
    uint16_t fanout = 2;

    minhton::NodeInfo node(3, 4, fanout, "1.2.3.4", 2000);
    minhton::RoutingInformation routing_info(node, Logger());

    auto left_neighbors_rev = routing_info.getLeftRoutingTableNeighborChildrenRightToLeft();
    auto right_neighbors_rev = routing_info.getRightRoutingTableNeighborChildrenRightToLeft();
    auto left_neighbors = routing_info.getLeftRoutingTableNeighborChildrenLeftToRight();
    auto right_neighbors = routing_info.getRightRoutingTableNeighborChildrenLeftToRight();

    REQUIRE(left_neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 0, fanout));
    REQUIRE(left_neighbors[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 1, fanout));
    REQUIRE(left_neighbors[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 4, fanout));
    REQUIRE(left_neighbors[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 5, fanout));
    REQUIRE(left_neighbors[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 6, fanout));
    REQUIRE(left_neighbors[5].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 7, fanout));

    REQUIRE(left_neighbors_rev[5].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 0, fanout));
    REQUIRE(left_neighbors_rev[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 1, fanout));
    REQUIRE(left_neighbors_rev[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 4, fanout));
    REQUIRE(left_neighbors_rev[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 5, fanout));
    REQUIRE(left_neighbors_rev[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 6, fanout));
    REQUIRE(left_neighbors_rev[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 7, fanout));

    REQUIRE(right_neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 10, fanout));
    REQUIRE(right_neighbors[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 11, fanout));
    REQUIRE(right_neighbors[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 12, fanout));
    REQUIRE(right_neighbors[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 13, fanout));

    REQUIRE(right_neighbors_rev[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 10, fanout));
    REQUIRE(right_neighbors_rev[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 11, fanout));
    REQUIRE(right_neighbors_rev[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 12, fanout));
    REQUIRE(right_neighbors_rev[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 13, fanout));

    REQUIRE(left_neighbors.size() == 3 * fanout);       // children of (3:0), (3:2), (3:3)
    REQUIRE(left_neighbors_rev.size() == 3 * fanout);   // children of (3:0), (3:2), (3:3)
    REQUIRE(right_neighbors.size() == 2 * fanout);      // children of (3:5), (3:6)
    REQUIRE(right_neighbors_rev.size() == 2 * fanout);  // children of (3:5), (3:6)
    REQUIRE(left_neighbors.size() + right_neighbors.size() ==
            routing_info.getRoutingTableNeighborChildren().size());

    for (auto const &left : left_neighbors) {
      REQUIRE(left < node);
    }
    for (auto const &right : right_neighbors) {
      REQUIRE(right > node);
    }
  }

  SECTION("Fanout 3") {
    uint16_t fanout = 3;

    minhton::NodeInfo node(2, 3, fanout, "1.2.3.4", 2000);
    minhton::RoutingInformation routing_info(node, Logger());

    auto left_neighbors_rev = routing_info.getLeftRoutingTableNeighborChildrenRightToLeft();
    auto right_neighbors_rev = routing_info.getRightRoutingTableNeighborChildrenRightToLeft();
    auto left_neighbors = routing_info.getLeftRoutingTableNeighborChildrenLeftToRight();
    auto right_neighbors = routing_info.getRightRoutingTableNeighborChildrenLeftToRight();

    REQUIRE(left_neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 0, fanout));
    REQUIRE(left_neighbors[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 1, fanout));
    REQUIRE(left_neighbors[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 2, fanout));
    REQUIRE(left_neighbors[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 3, fanout));
    REQUIRE(left_neighbors[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 4, fanout));
    REQUIRE(left_neighbors[5].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 5, fanout));
    REQUIRE(left_neighbors[6].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 6, fanout));
    REQUIRE(left_neighbors[7].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 7, fanout));
    REQUIRE(left_neighbors[8].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 8, fanout));

    REQUIRE(left_neighbors_rev[8].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 0, fanout));
    REQUIRE(left_neighbors_rev[7].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 1, fanout));
    REQUIRE(left_neighbors_rev[6].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 2, fanout));
    REQUIRE(left_neighbors_rev[5].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 3, fanout));
    REQUIRE(left_neighbors_rev[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 4, fanout));
    REQUIRE(left_neighbors_rev[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 5, fanout));
    REQUIRE(left_neighbors_rev[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 6, fanout));
    REQUIRE(left_neighbors_rev[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 7, fanout));
    REQUIRE(left_neighbors_rev[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 8, fanout));

    REQUIRE(right_neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 12, fanout));
    REQUIRE(right_neighbors[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 13, fanout));
    REQUIRE(right_neighbors[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 14, fanout));
    REQUIRE(right_neighbors[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 15, fanout));
    REQUIRE(right_neighbors[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 16, fanout));
    REQUIRE(right_neighbors[5].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 17, fanout));
    REQUIRE(right_neighbors[6].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 18, fanout));
    REQUIRE(right_neighbors[7].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 19, fanout));
    REQUIRE(right_neighbors[8].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 20, fanout));

    REQUIRE(right_neighbors_rev[8].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 12, fanout));
    REQUIRE(right_neighbors_rev[7].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 13, fanout));
    REQUIRE(right_neighbors_rev[6].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 14, fanout));
    REQUIRE(right_neighbors_rev[5].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 15, fanout));
    REQUIRE(right_neighbors_rev[4].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 16, fanout));
    REQUIRE(right_neighbors_rev[3].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 17, fanout));
    REQUIRE(right_neighbors_rev[2].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 18, fanout));
    REQUIRE(right_neighbors_rev[1].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 19, fanout));
    REQUIRE(right_neighbors_rev[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 20, fanout));

    for (auto const &left : left_neighbors) {
      REQUIRE(left < node);
    }
    for (auto const &right : right_neighbors) {
      REQUIRE(right > node);
    }
  }
}
