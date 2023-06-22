// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

#include "algorithms/leave/minhton_leave_algorithm.h"
#include "message/join.h"

using namespace minhton;

class MinhtonLeaveAlgorithmForTest : public MinhtonLeaveAlgorithm {
public:
  explicit MinhtonLeaveAlgorithmForTest(std::shared_ptr<AccessContainer> access)
      : MinhtonLeaveAlgorithm(access){};

  using MinhtonLeaveAlgorithm::getAdjacentLeftFromVector;
  using MinhtonLeaveAlgorithm::getAdjacentRightFromVector;
};

TEST_CASE("LeaveAlgorithmGeneral getAdjacentFromVector Left and Right",
          "[LeaveAlgorithmGeneral][getAdjacentFromVector]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);

  minhton::NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_1_1(1, 1, fanout, "1.2.3.4", 2000);

  minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);

  minhton::NodeInfo node_3_0(3, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_3_1(3, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_3_2(3, 2, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_3_3(3, 3, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_3_4(3, 4, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_3_5(3, 5, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_3_6(3, 6, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_3_7(3, 7, fanout, "1.2.3.4", 2000);

  auto routing_info = std::make_shared<RoutingInformation>(node_0_0, Logger());
  auto access = std::make_shared<AccessContainer>();
  access->routing_info = routing_info;
  MinhtonLeaveAlgorithmForTest leave_algo(access);

  SECTION("Case 1") {
    std::vector<minhton::NodeInfo> neighbors = {node_3_1, node_3_2, node_3_4, node_2_0};

    auto adj_left = leave_algo.getAdjacentLeftFromVector(node_3_0, neighbors);
    auto adj_right = leave_algo.getAdjacentRightFromVector(node_3_0, neighbors);

    REQUIRE_FALSE(adj_left.isInitialized());
    REQUIRE(adj_right.isInitialized());
    REQUIRE(adj_right.getLogicalNodeInfo() == node_2_0.getLogicalNodeInfo());
  }

  SECTION("Case 2") {
    std::vector<minhton::NodeInfo> neighbors = {node_3_6, node_3_5, node_3_3, node_2_3};

    auto adj_left = leave_algo.getAdjacentLeftFromVector(node_3_7, neighbors);
    auto adj_right = leave_algo.getAdjacentRightFromVector(node_3_7, neighbors);

    REQUIRE_FALSE(adj_right.isInitialized());
    REQUIRE(adj_left.isInitialized());
    REQUIRE(adj_left.getLogicalNodeInfo() == node_2_3.getLogicalNodeInfo());
  }

  SECTION("Case 3") {
    std::vector<minhton::NodeInfo> neighbors = {node_3_2, node_0_0, node_1_0,
                                                node_2_0, node_2_2, node_2_3};

    auto adj_left = leave_algo.getAdjacentLeftFromVector(node_2_1, neighbors);
    auto adj_right = leave_algo.getAdjacentRightFromVector(node_2_1, neighbors);

    REQUIRE(adj_right.isInitialized());
    REQUIRE(adj_right.getLogicalNodeInfo() == node_0_0.getLogicalNodeInfo());
    REQUIRE(adj_left.isInitialized());
    REQUIRE(adj_left.getLogicalNodeInfo() == node_3_2.getLogicalNodeInfo());
  }
}

TEST_CASE("RoutingInformationJoinHelper getFurthestPNAtEndOfThisLevel",
          "[RoutingInformationJoinHelper][Methods][getFurthestPNAtEndOfThisLevel]") {
  // only works for old prio set
  // TODO: update tests for new prio set (DSNs)

  // uint16_t fanout = 2;

  // minhton::NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);

  // minhton::NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_1_1(1, 1, fanout, "1.2.3.4", 2000);

  // minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);

  // minhton::NodeInfo node_3_0(3, 0, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_3_1(3, 1, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_3_2(3, 2, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_3_3(3, 3, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_3_4(3, 4, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_3_5(3, 5, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_3_6(3, 6, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_3_7(3, 7, fanout, "1.2.3.4", 2000);

  // minhton::NodeInfo node_4_0(4, 0, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_1(4, 1, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_2(4, 2, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_3(4, 3, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_4(4, 4, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_5(4, 5, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_6(4, 6, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_7(4, 7, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_8(4, 8, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_9(4, 9, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_10(4, 10, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_11(4, 11, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_12(4, 12, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_13(4, 13, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_14(4, 14, fanout, "1.2.3.4", 2000);
  // minhton::NodeInfo node_4_15(4, 15, fanout, "1.2.3.4", 2000);

  // SECTION("Case 1") {
  //   minhton::RoutingInformation routing_info(node_4_10, Logger());

  //   routing_info.updateNeighbor(node_4_6);
  //   REQUIRE(routing_info.getRoutingTableNeighbors()[1].isInitialized());
  //   auto furthest = routing_info.getFurthestPrioNodeAtEndOfThisLevel();
  //   REQUIRE(furthest.isInitialized() == false);

  //   routing_info.updateNeighbor(node_4_9);
  //   REQUIRE(routing_info.getRoutingTableNeighbors()[3].isInitialized());
  //   furthest = routing_info.getFurthestPrioNodeAtEndOfThisLevel();
  //   REQUIRE(furthest.isInitialized() == false);

  //   routing_info.updateNeighbor(node_4_8);
  //   REQUIRE(routing_info.getRoutingTableNeighbors()[2].isInitialized());
  //   furthest = routing_info.getFurthestPrioNodeAtEndOfThisLevel();
  //   REQUIRE(furthest.getLogicalNodeInfo() == node_4_8.getLogicalNodeInfo());
  //   REQUIRE(furthest.isInitialized());

  //   routing_info.updateNeighbor(node_4_2);
  //   REQUIRE(routing_info.getRoutingTableNeighbors()[0].isInitialized());
  //   furthest = routing_info.getFurthestPrioNodeAtEndOfThisLevel();
  //   REQUIRE(furthest.getLogicalNodeInfo() == node_4_2.getLogicalNodeInfo());
  //   REQUIRE(furthest.isInitialized());
  // }

  // SECTION("Case 2") {
  //   minhton::RoutingInformation routing_info(node_3_1, Logger());

  //   routing_info.updateNeighbor(node_3_5);
  //   REQUIRE(routing_info.getRoutingTableNeighbors()[3].isInitialized());
  //   auto furthest = routing_info.getFurthestPrioNodeAtEndOfThisLevel();
  //   REQUIRE(furthest.isInitialized() == false);

  //   routing_info.updateNeighbor(node_3_3);
  //   REQUIRE(routing_info.getRoutingTableNeighbors()[2].isInitialized());
  //   furthest = routing_info.getFurthestPrioNodeAtEndOfThisLevel();
  //   REQUIRE(furthest.getLogicalNodeInfo() == node_3_3.getLogicalNodeInfo());
  //   REQUIRE(furthest.isInitialized());

  //   routing_info.updateNeighbor(node_3_2);
  //   REQUIRE(routing_info.getRoutingTableNeighbors()[1].isInitialized());
  //   furthest = routing_info.getFurthestPrioNodeAtEndOfThisLevel();
  //   REQUIRE(furthest.getLogicalNodeInfo() == node_3_3.getLogicalNodeInfo());
  //   REQUIRE(furthest.isInitialized());
  // }
}

TEST_CASE("RoutingInformationJoinHelper getLowestPrioNode",
          "[RoutingInformationJoinHelper][Methods][getLowestPrioNode]") {
  //   // only works for old prio set
  // TODO: update tests for new prio set (DSNs)

  //   // uint16_t fanout = 2;

  //   // minhton::NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);

  //   // minhton::NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_1_1(1, 1, fanout, "1.2.3.4", 2000);

  //   // minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);

  //   // minhton::NodeInfo node_3_0(3, 0, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_3_1(3, 1, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_3_2(3, 2, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_3_3(3, 3, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_3_4(3, 4, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_3_5(3, 5, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_3_6(3, 6, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_3_7(3, 7, fanout, "1.2.3.4", 2000);

  //   // minhton::NodeInfo node_4_0(4, 0, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_1(4, 1, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_2(4, 2, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_3(4, 3, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_4(4, 4, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_5(4, 5, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_6(4, 6, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_7(4, 7, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_8(4, 8, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_9(4, 9, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_10(4, 10, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_11(4, 11, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_12(4, 12, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_13(4, 13, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_14(4, 14, fanout, "1.2.3.4", 2000);
  //   // minhton::NodeInfo node_4_15(4, 15, fanout, "1.2.3.4", 2000);

  //   // SECTION("Case 1") {
  //   //   minhton::RoutingInformation routing_info(node_0_0, Logger());

  //   //   routing_info.setAdjacentLeft(node_3_3);
  //   //   routing_info.setAdjacentRight(node_3_4);
  //   //   routing_info.setChild(node_1_0, 0);
  //   //   routing_info.setChild(node_1_1, 1);

  //   //   auto lowest = routing_info.getLowestPrioNode();
  //   //   REQUIRE(lowest.getLogicalNodeInfo() == node_3_3.getLogicalNodeInfo());
  //   //   REQUIRE(lowest.getLogicalNodeInfo().isPrioNode());
  //   //   REQUIRE(lowest.isInitialized());
  //   // }

  //   // SECTION("Case 2") {
  //   //   minhton::RoutingInformation routing_info(node_3_0, Logger());

  //   //   routing_info.setAdjacentLeft(node_4_0);
  //   //   routing_info.setAdjacentRight(node_4_1);
  //   //   routing_info.setChild(node_4_0, 0);
  //   //   routing_info.setChild(node_4_1, 1);
  //   //   routing_info.updateNeighbor(node_2_0);
  //   //   routing_info.updateNeighbor(node_3_1);
  //   //   routing_info.updateNeighbor(node_3_2);
  //   //   routing_info.updateNeighbor(node_3_4);
  //   //   routing_info.updateNeighbor(node_4_2);
  //   //   routing_info.updateNeighbor(node_4_3);
  //   //   routing_info.updateNeighbor(node_4_4);
  //   //   routing_info.updateNeighbor(node_4_5);
  //   //   routing_info.updateNeighbor(node_4_8);
  //   //   routing_info.updateNeighbor(node_4_9);

  //   //   auto lowest = routing_info.getLowestPrioNode();
  //   //   REQUIRE(lowest.getLogicalNodeInfo().isPrioNode());
  //   //   REQUIRE(lowest.isInitialized());
  //   //   REQUIRE(lowest.getLevel() == 4);
  //   // }

  //   // SECTION("Case 3") {
  //   //   // every node on the last level must return an existing prio node
  //   //   // if the level is full

  //   //   for (uint32_t i = 0; i <= 15; i++) {
  //   //     minhton::NodeInfo current_node(4, i, fanout, "1.2.3.4", 2000);
  //   //     minhton::RoutingInformation routing_info(current_node, Logger());

  //   //     routing_info.updateNeighbor(node_4_0);
  //   //     routing_info.updateNeighbor(node_4_1);
  //   //     routing_info.updateNeighbor(node_4_2);
  //   //     routing_info.updateNeighbor(node_4_3);
  //   //     routing_info.updateNeighbor(node_4_4);
  //   //     routing_info.updateNeighbor(node_4_5);
  //   //     routing_info.updateNeighbor(node_4_6);
  //   //     routing_info.updateNeighbor(node_4_7);
  //   //     routing_info.updateNeighbor(node_4_8);
  //   //     routing_info.updateNeighbor(node_4_9);
  //   //     routing_info.updateNeighbor(node_4_10);
  //   //     routing_info.updateNeighbor(node_4_11);
  //   //     routing_info.updateNeighbor(node_4_12);
  //   //     routing_info.updateNeighbor(node_4_13);
  //   //     routing_info.updateNeighbor(node_4_14);
  //   //     routing_info.updateNeighbor(node_4_15);

  //   //     auto lowest = routing_info.getLowestPrioNode();
  //   //     REQUIRE(lowest.isInitialized());
  //   //     REQUIRE(lowest.getLogicalNodeInfo().isPrioNode());
  //   //     REQUIRE(lowest.getLevel() == 4);
  //   //   }
  //   // }

  //   // SECTION("Case 4") {
  //   //   // we dont know a lower prio node or none on the same level

  //   //   minhton::RoutingInformation routing_info(node_3_7, Logger());
  //   //   routing_info.setParent(node_2_3);
  //   //   routing_info.setAdjacentLeft(node_2_3);

  //   //   auto lowest = routing_info.getLowestPrioNode();
  //   //   REQUIRE(lowest.isInitialized() == false);
  //   // }

  //   // SECTION("Case 5") {
  //   //   // we dont know any prio node

  //   //   minhton::RoutingInformation routing_info(node_3_0, Logger());
  //   //   routing_info.setParent(node_2_0);
  //   //   routing_info.setAdjacentRight(node_2_0);

  //   //   auto lowest = routing_info.getLowestPrioNode();
  //   //   REQUIRE(lowest.isInitialized() == false);
  //   // }
}
