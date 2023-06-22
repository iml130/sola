// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

#include "core/routing_information.h"

using namespace minhton;

TEST_CASE("RoutingInformation areChildrenFull", "[RoutingInformation][Methods][areChildrenFull]") {
  minhton::NodeInfo node_4(0, 0, (uint16_t)4);
  minhton::RoutingInformation routing_info_4(node_4, Logger());

  minhton::NodeInfo child_0(1, 0, (uint16_t)4);
  minhton::NodeInfo child_1(1, 1, (uint16_t)4);
  minhton::NodeInfo child_2(1, 2, (uint16_t)4);
  minhton::NodeInfo child_3(1, 3, (uint16_t)4);

  child_0.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  child_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  child_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  child_3.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info_4.setChild(child_3, 3);
  REQUIRE_FALSE(routing_info_4.areChildrenFull());

  routing_info_4.setChild(child_2, 2);
  REQUIRE_FALSE(routing_info_4.areChildrenFull());

  routing_info_4.setChild(child_0, 0);
  REQUIRE_FALSE(routing_info_4.areChildrenFull());

  routing_info_4.setChild(child_1, 1);
  REQUIRE(routing_info_4.areChildrenFull());
}

TEST_CASE("RoutingInformation areRoutingTableNeighborsFull",
          "[RoutingInformation][Methods][areRoutingTableNeighborsFull]") {
  SECTION("Fanout 3") {
    minhton::NodeInfo node_1_0(1, 0, 3);
    minhton::NodeInfo node_1_1(1, 1, 3);
    minhton::NodeInfo node_1_2(1, 2, 3);

    node_1_0.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
    node_1_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
    node_1_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

    minhton::RoutingInformation routing_info_2(node_1_1, Logger());
    REQUIRE_FALSE(routing_info_2.areRoutingTableNeighborsFull());

    routing_info_2.updateRoutingTableNeighbor(node_1_0);
    REQUIRE_FALSE(routing_info_2.areRoutingTableNeighborsFull());

    routing_info_2.updateRoutingTableNeighbor(node_1_2);
    REQUIRE(routing_info_2.areRoutingTableNeighborsFull());
  }

  SECTION("Fanout 4") {
    minhton::NodeInfo node_1_0(1, 0, 4);
    minhton::NodeInfo node_1_1(1, 1, 4);
    minhton::NodeInfo node_1_2(1, 2, 4);
    minhton::NodeInfo node_1_3(1, 3, 4);

    node_1_0.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
    node_1_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
    node_1_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
    node_1_3.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

    minhton::RoutingInformation routing_info_3(node_1_0, Logger());
    REQUIRE_FALSE(routing_info_3.areRoutingTableNeighborsFull());

    routing_info_3.updateRoutingTableNeighbor(node_1_1);
    REQUIRE_FALSE(routing_info_3.areRoutingTableNeighborsFull());

    routing_info_3.updateRoutingTableNeighbor(node_1_3);
    REQUIRE_FALSE(routing_info_3.areRoutingTableNeighborsFull());

    routing_info_3.updateRoutingTableNeighbor(node_1_2);
    REQUIRE(routing_info_3.areRoutingTableNeighborsFull());
  }
}

TEST_CASE("RoutingInformation areRoutingTableNeighborChildrenFull",
          "[RoutingInformation][Methods][areRoutingTableNeighborChildrenFull]") {
  uint16_t fanout = 3;

  minhton::NodeInfo node_1_0(1, 0, fanout);
  node_1_0.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  minhton::NodeInfo node_1_1(1, 1, fanout);
  node_1_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  minhton::NodeInfo node_1_2(1, 2, fanout);
  node_1_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  minhton::NodeInfo node_2_0(2, 0, fanout);
  node_2_0.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  minhton::NodeInfo node_2_1(2, 1, fanout);
  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  minhton::NodeInfo node_2_2(2, 2, fanout);
  node_2_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  minhton::NodeInfo node_2_6(2, 6, fanout);
  node_2_6.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  minhton::NodeInfo node_2_7(2, 7, fanout);
  node_2_7.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  minhton::NodeInfo node_2_8(2, 8, fanout);
  node_2_8.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  minhton::RoutingInformation routing_info(node_1_1, Logger());
  routing_info.updateRoutingTableNeighbor(node_1_0);
  routing_info.updateRoutingTableNeighbor(node_1_2);

  REQUIRE_FALSE(routing_info.areRoutingTableNeighborChildrenFull());
  routing_info.updateRoutingTableNeighborChild(node_2_0);
  REQUIRE_FALSE(routing_info.areRoutingTableNeighborChildrenFull());
  routing_info.updateRoutingTableNeighborChild(node_2_1);
  REQUIRE_FALSE(routing_info.areRoutingTableNeighborChildrenFull());
  routing_info.updateRoutingTableNeighborChild(node_2_2);
  REQUIRE_FALSE(routing_info.areRoutingTableNeighborChildrenFull());
  routing_info.updateRoutingTableNeighborChild(node_2_6);
  REQUIRE_FALSE(routing_info.areRoutingTableNeighborChildrenFull());
  routing_info.updateRoutingTableNeighborChild(node_2_7);
  REQUIRE_FALSE(routing_info.areRoutingTableNeighborChildrenFull());
  routing_info.updateRoutingTableNeighborChild(node_2_8);
  REQUIRE(routing_info.areRoutingTableNeighborChildrenFull());
}

TEST_CASE("RoutingInformation Getter getRoutingTableNeighborsAndChildren Left/Right",
          "[RoutingInformation][Methods][getRoutingTableNeighborsAndChildren]") {
  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  std::vector<minhton::NodeInfo> neighbors = routing_info.getRoutingTableNeighborsAndChildren();
  REQUIRE(neighbors.size() == 8);  // 1+1+3+3=8

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 1, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 6, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 7, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 8, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);
}

TEST_CASE("RoutingInformation Getter getRoutingTableNeighbors",
          "[RoutingInformation][Methods][getRoutingTableNeighbors]") {
  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  std::vector<minhton::NodeInfo> neighbors = routing_info.getRoutingTableNeighbors();
  REQUIRE(neighbors.size() == 2);  // 1+1

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);
}

TEST_CASE("RoutingInformation Getter getAllLeftRoutingTableNeighborsAndChildren",
          "[RoutingInformation][Methods][getAllLeftRoutingTableNeighborsAndChildren]") {
  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  std::vector<minhton::NodeInfo> neighbors =
      routing_info.getAllLeftRoutingTableNeighborsAndChildren();

  REQUIRE(neighbors.size() == 4);

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 1, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);
}

TEST_CASE("RoutingInformation Getter getAllRightRTNsAndChildren",
          "[RoutingInformation][Methods][getAllRightRTNsAndChildren]") {
  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  std::vector<minhton::NodeInfo> neighbors =
      routing_info.getAllRightRoutingTableNeighborsAndChildren();

  REQUIRE(neighbors.size() == 4);

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 6, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 7, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 8, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);
}

TEST_CASE("RoutingInformation Getter getAllInitRTNeighborChildren",
          "[RoutingInformation][Methods][getAllInitRTNeighborChildren]") {
  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3);
  minhton::NodeInfo node_2_8(2, 8, (uint16_t)3);
  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_8.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info.updateRoutingTableNeighborChild(node_2_1);
  routing_info.updateRoutingTableNeighborChild(node_2_8);

  std::vector<minhton::NodeInfo> neighbors =
      routing_info.getAllInitializedRoutingTableNeighborChildren();

  REQUIRE(neighbors.size() == 2);

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 1, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 6, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 7, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 8, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);
}

TEST_CASE("RoutingInformation Getter getAllInitLeftRTNsAndChildren",
          "[RoutingInformation][Methods][getAllInitLeftRTNsAndChildren]") {
  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_1_0(1, 0, (uint16_t)3);
  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3);
  minhton::NodeInfo node_2_8(2, 8, (uint16_t)3);
  node_1_0.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_8.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info.updateRoutingTableNeighbor(node_1_0);
  routing_info.updateRoutingTableNeighborChild(node_2_1);
  routing_info.updateRoutingTableNeighborChild(node_2_8);

  std::vector<minhton::NodeInfo> neighbors =
      routing_info.getAllInitializedLeftRoutingTableNeighborsAndChildren();

  REQUIRE(neighbors.size() == 2);

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 1, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 6, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 7, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 8, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);
}

TEST_CASE("RoutingInformation Getter getAllInitRightRTNsAndChildren",
          "[RoutingInformation][Methods][getAllInitRightRTNsAndChildren]") {
  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_1_2(1, 2, (uint16_t)3);
  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3);
  minhton::NodeInfo node_2_8(2, 8, (uint16_t)3);
  node_1_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_8.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info.updateRoutingTableNeighbor(node_1_2);
  routing_info.updateRoutingTableNeighborChild(node_2_1);
  routing_info.updateRoutingTableNeighborChild(node_2_8);

  std::vector<minhton::NodeInfo> neighbors =
      routing_info.getAllInitializedRightRoutingTableNeighborsAndChildren();

  REQUIRE(neighbors.size() == 2);

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(1, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 0, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 1, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 2, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 6, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 7, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE_FALSE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 8, (uint16_t)3)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);
}

TEST_CASE("RoutingInformation Getter getDirectLeftNeighbor",
          "[RoutingInformation][Methods][getDirectLeftNeighbor]") {
  minhton::NodeInfo node(2, 4, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3);
  minhton::NodeInfo node_2_2(2, 2, (uint16_t)3);
  minhton::NodeInfo node_2_3(2, 3, (uint16_t)3);

  minhton::NodeInfo node_2_5(2, 5, (uint16_t)3);
  minhton::NodeInfo node_2_6(2, 6, (uint16_t)3);
  minhton::NodeInfo node_2_7(2, 7, (uint16_t)3);

  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_3.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_5.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_6.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_7.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info.updateRoutingTableNeighbor(node_2_1);
  routing_info.updateRoutingTableNeighbor(node_2_2);
  routing_info.updateRoutingTableNeighbor(node_2_3);
  routing_info.updateRoutingTableNeighbor(node_2_5);
  routing_info.updateRoutingTableNeighbor(node_2_6);
  routing_info.updateRoutingTableNeighbor(node_2_7);

  minhton::NodeInfo neighbor = routing_info.getDirectLeftNeighbor();

  REQUIRE(neighbor.isInitialized());
  REQUIRE(neighbor.getLevel() == 2);
  REQUIRE(neighbor.getNumber() == 3);
  REQUIRE(neighbor.getNetworkInfo().getAddress() == "1.2.3.4");
}

TEST_CASE("RoutingInformation Getter getDirectRightNeighbor",
          "[RoutingInformation][Methods][getDirectRightNeighbor]") {
  minhton::NodeInfo node(2, 4, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3);
  minhton::NodeInfo node_2_2(2, 2, (uint16_t)3);
  minhton::NodeInfo node_2_3(2, 3, (uint16_t)3);

  minhton::NodeInfo node_2_5(2, 5, (uint16_t)3);
  minhton::NodeInfo node_2_6(2, 6, (uint16_t)3);
  minhton::NodeInfo node_2_7(2, 7, (uint16_t)3);

  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_3.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_5.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_6.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_7.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info.updateRoutingTableNeighbor(node_2_1);
  routing_info.updateRoutingTableNeighbor(node_2_2);
  routing_info.updateRoutingTableNeighbor(node_2_3);
  routing_info.updateRoutingTableNeighbor(node_2_5);
  routing_info.updateRoutingTableNeighbor(node_2_6);
  routing_info.updateRoutingTableNeighbor(node_2_7);

  minhton::NodeInfo neighbor = routing_info.getDirectRightNeighbor();

  REQUIRE(neighbor.isInitialized());
  REQUIRE(neighbor.getLevel() == 2);
  REQUIRE(neighbor.getNumber() == 5);
  REQUIRE(neighbor.getNetworkInfo().getAddress() == "1.2.3.4");
}

TEST_CASE("RoutingInformation Getter getLeftmostNeighbor",
          "[RoutingInformation][Methods][getLeftmostNeighbor]") {
  minhton::NodeInfo node(2, 4, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3);
  minhton::NodeInfo node_2_2(2, 2, (uint16_t)3);
  minhton::NodeInfo node_2_3(2, 3, (uint16_t)3);

  minhton::NodeInfo node_2_5(2, 5, (uint16_t)3);
  minhton::NodeInfo node_2_6(2, 6, (uint16_t)3);
  minhton::NodeInfo node_2_7(2, 7, (uint16_t)3);

  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_3.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_5.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_6.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_7.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info.updateRoutingTableNeighbor(node_2_1);
  routing_info.updateRoutingTableNeighbor(node_2_2);
  routing_info.updateRoutingTableNeighbor(node_2_3);
  routing_info.updateRoutingTableNeighbor(node_2_5);
  routing_info.updateRoutingTableNeighbor(node_2_6);
  routing_info.updateRoutingTableNeighbor(node_2_7);

  minhton::NodeInfo neighbor = routing_info.getLeftmostNeighbor();

  REQUIRE(neighbor.isInitialized());
  REQUIRE(neighbor.getLevel() == 2);
  REQUIRE(neighbor.getNumber() == 1);
  REQUIRE(neighbor.getNetworkInfo().getAddress() == "1.2.3.4");
}

TEST_CASE("RoutingInformation Getter getRightmostNeighbor",
          "[RoutingInformation][Methods][getRightmostNeighbor]") {
  minhton::NodeInfo node(2, 4, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3);
  minhton::NodeInfo node_2_2(2, 2, (uint16_t)3);
  minhton::NodeInfo node_2_3(2, 3, (uint16_t)3);

  minhton::NodeInfo node_2_5(2, 5, (uint16_t)3);
  minhton::NodeInfo node_2_6(2, 6, (uint16_t)3);
  minhton::NodeInfo node_2_7(2, 7, (uint16_t)3);

  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_3.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_5.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_6.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_7.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info.updateRoutingTableNeighbor(node_2_1);
  routing_info.updateRoutingTableNeighbor(node_2_2);
  routing_info.updateRoutingTableNeighbor(node_2_3);
  routing_info.updateRoutingTableNeighbor(node_2_5);
  routing_info.updateRoutingTableNeighbor(node_2_6);
  routing_info.updateRoutingTableNeighbor(node_2_7);

  minhton::NodeInfo neighbor = routing_info.getRightmostNeighbor();

  REQUIRE(neighbor.isInitialized());
  REQUIRE(neighbor.getLevel() == 2);
  REQUIRE(neighbor.getNumber() == 7);
  REQUIRE(neighbor.getNetworkInfo().getAddress() == "1.2.3.4");
}

TEST_CASE("RoutingInformation Getter getLeftmostNeighborChild",
          "[RoutingInformation][Methods][getLeftmostNeighborChild]") {
  minhton::NodeInfo node(2, 4, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3);
  minhton::NodeInfo node_2_2(2, 2, (uint16_t)3);
  minhton::NodeInfo node_2_3(2, 3, (uint16_t)3);

  minhton::NodeInfo node_3_3(3, 3, (uint16_t)3);
  minhton::NodeInfo node_3_4(3, 4, (uint16_t)3);
  minhton::NodeInfo node_3_5(3, 5, (uint16_t)3);
  minhton::NodeInfo node_3_6(3, 6, (uint16_t)3);

  node_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_2_3.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_3_3.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_3_4.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_3_5.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
  node_3_6.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));

  routing_info.updateRoutingTableNeighbor(node_2_1);
  routing_info.updateRoutingTableNeighbor(node_2_2);
  routing_info.updateRoutingTableNeighbor(node_2_3);

  routing_info.updateRoutingTableNeighbor(node_3_3);
  minhton::NodeInfo neighbor = routing_info.getLeftmostNeighborChild();
  REQUIRE_FALSE(neighbor.isInitialized());

  routing_info.updateRoutingTableNeighborChild(node_3_3);
  routing_info.updateRoutingTableNeighborChild(node_3_4);
  routing_info.updateRoutingTableNeighborChild(node_3_5);
  routing_info.updateRoutingTableNeighborChild(node_3_6);

  neighbor = routing_info.getLeftmostNeighborChild();

  REQUIRE(neighbor.isInitialized());
  REQUIRE(neighbor.getLevel() == 3);
  REQUIRE(neighbor.getNumber() == 3);
  REQUIRE(neighbor.getNetworkInfo().getAddress() == "1.2.3.4");
}

TEST_CASE("RoutingInformationGeneralHelper updateNeighbor",
          "[RoutingInformationGeneralHelper][Methods][updateNeighbor]") {
  SECTION("Case 1") {
    uint16_t fanout = 2;
    minhton::NodeInfo node(1, 0, fanout);
    minhton::RoutingInformation routing_info(node, Logger());

    minhton::NodeInfo parent_uninit(0, 0, fanout);
    minhton::NodeInfo parent_1(0, 0, fanout);
    minhton::NodeInfo parent_2(0, 0, fanout);
    parent_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
    parent_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.5", 2001));

    minhton::NodeInfo child_uninit(2, 0, fanout);
    minhton::NodeInfo child_1_1(2, 0, fanout);
    child_1_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
    minhton::NodeInfo child_1_2(2, 0, fanout);
    child_1_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.6", 2020));

    minhton::NodeInfo child_2_1(2, 1, fanout);
    child_2_1.setNetworkInfo(minhton::NetworkInfo("1.2.3.4", 2000));
    minhton::NodeInfo child_2_2(2, 1, fanout);
    child_2_2.setNetworkInfo(minhton::NetworkInfo("1.2.3.6", 2020));

    minhton::NodeInfo adj_right_1 = parent_1;
    minhton::NodeInfo adj_right_2 = child_2_2;

    minhton::NodeInfo adj_left_1 = child_1_1;
    minhton::NodeInfo adj_left_2(3, 1, fanout);
    adj_left_2.setNetworkInfo(minhton::NetworkInfo("1.2.7.5", 2201));

    minhton::NodeInfo rt_neighbor_1(1, 1, fanout);
    rt_neighbor_1.setNetworkInfo(minhton::NetworkInfo("12.2.3.4", 2000));
    minhton::NodeInfo rt_neighbor_2(1, 1, fanout);
    rt_neighbor_2.setNetworkInfo(minhton::NetworkInfo("13.2.3.4", 2000));

    minhton::NodeInfo rt_neighbor_child_1(2, 3, fanout);
    rt_neighbor_child_1.setNetworkInfo(minhton::NetworkInfo("13.2.3.4", 2100));

    // initial setting of adjacents
    REQUIRE_FALSE(routing_info.getAdjacentLeft().isValidPeer());
    REQUIRE_FALSE(routing_info.getAdjacentRight().isValidPeer());
    routing_info.setAdjacentLeft(adj_left_1);
    routing_info.setAdjacentRight(adj_right_1);
    REQUIRE(routing_info.getAdjacentLeft().isInitialized());
    REQUIRE(routing_info.getAdjacentRight().isInitialized());

    // parent
    REQUIRE_FALSE(routing_info.getParent().isInitialized());
    routing_info.updateNeighbor(parent_1);
    REQUIRE(routing_info.getParent().isInitialized());
    REQUIRE(routing_info.getParent().getAddress() == "1.2.3.4");
    REQUIRE(routing_info.getParent().getPort() == 2000);
    routing_info.updateNeighbor(parent_2);
    REQUIRE(routing_info.getParent().isInitialized());
    REQUIRE(routing_info.getParent().getAddress() == "1.2.3.5");
    REQUIRE(routing_info.getParent().getPort() == 2001);
    REQUIRE(routing_info.getAdjacentRight().isInitialized() ==
            true);  // adjacent right must change at the same time too
    REQUIRE(routing_info.getAdjacentRight().getAddress() == "1.2.3.5");
    REQUIRE(routing_info.getAdjacentRight().getPort() == 2001);
    REQUIRE_THROWS_AS(routing_info.updateNeighbor(parent_uninit), std::invalid_argument);

    // childs
    REQUIRE_FALSE(routing_info.getChildren()[0].isInitialized());
    REQUIRE_FALSE(routing_info.getChildren()[1].isInitialized());
    REQUIRE_THROWS_AS(routing_info.updateNeighbor(child_uninit), std::invalid_argument);
    routing_info.updateNeighbor(child_1_1);
    REQUIRE(routing_info.getChildren()[0].isInitialized());
    REQUIRE(routing_info.getChildren()[0].getAddress() == child_1_1.getAddress());
    routing_info.updateNeighbor(child_1_2);
    REQUIRE(routing_info.getChildren()[0].isInitialized());
    REQUIRE(routing_info.getChildren()[0].getAddress() == child_1_2.getAddress());
    REQUIRE(routing_info.getAdjacentLeft().isInitialized() ==
            true);  // adjacent left must change at the same time too
    REQUIRE(routing_info.getAdjacentLeft().getAddress() == child_1_2.getAddress());
    routing_info.updateNeighbor(child_2_1);
    REQUIRE(routing_info.getChildren()[1].isInitialized());
    REQUIRE(routing_info.getChildren()[1].getAddress() == child_2_1.getAddress());
    routing_info.updateNeighbor(child_2_2);
    REQUIRE(routing_info.getChildren()[1].isInitialized());
    REQUIRE(routing_info.getChildren()[1].getAddress() == child_2_2.getAddress());

    // routing table neighbors
    routing_info.updateNeighbor(rt_neighbor_1);
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getAddress() == rt_neighbor_1.getAddress());
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getPort() == rt_neighbor_1.getPort());
    routing_info.updateNeighbor(rt_neighbor_2);
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getAddress() == rt_neighbor_2.getAddress());
    REQUIRE(routing_info.getRoutingTableNeighbors()[0].getPort() == rt_neighbor_2.getPort());

    // routing table neighbor childs
    routing_info.updateNeighbor(rt_neighbor_child_1);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].isInitialized());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].getAddress() ==
            rt_neighbor_child_1.getAddress());
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].getPort() ==
            rt_neighbor_child_1.getPort());
  }

  SECTION("Case 2") {
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

    minhton::RoutingInformation routing_info(node_3_4, Logger());
    routing_info.setAdjacentLeft(node_4_8);
    routing_info.setAdjacentRight(node_4_9);
    routing_info.updateNeighbor(node_2_2);  // parent

    routing_info.updateNeighbor(node_3_0);
    routing_info.updateNeighbor(node_3_1);
    routing_info.updateNeighbor(node_3_2);
    routing_info.updateNeighbor(node_3_3);
    routing_info.updateNeighbor(node_3_4);
    routing_info.updateNeighbor(node_3_5);
    routing_info.updateNeighbor(node_3_6);
    routing_info.updateNeighbor(node_3_7);

    REQUIRE(routing_info.getRoutingTableNeighbors()[0] == node_3_0);
    REQUIRE(routing_info.getRoutingTableNeighbors()[1] == node_3_2);
    REQUIRE(routing_info.getRoutingTableNeighbors()[2] == node_3_3);
    REQUIRE(routing_info.getRoutingTableNeighbors()[3] == node_3_5);
    REQUIRE(routing_info.getRoutingTableNeighbors()[4] == node_3_6);

    routing_info.updateNeighbor(node_4_0);
    routing_info.updateNeighbor(node_4_1);
    routing_info.updateNeighbor(node_4_2);
    routing_info.updateNeighbor(node_4_3);
    routing_info.updateNeighbor(node_4_4);
    routing_info.updateNeighbor(node_4_5);
    routing_info.updateNeighbor(node_4_6);
    routing_info.updateNeighbor(node_4_7);
    routing_info.updateNeighbor(node_4_10);
    routing_info.updateNeighbor(node_4_11);
    routing_info.updateNeighbor(node_4_12);
    routing_info.updateNeighbor(node_4_13);
    routing_info.updateNeighbor(node_4_14);
    routing_info.updateNeighbor(node_4_15);

    REQUIRE(routing_info.getRoutingTableNeighborChildren()[0] == node_4_0);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[1] == node_4_1);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[2] == node_4_4);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[3] == node_4_5);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[4] == node_4_6);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[5] == node_4_7);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[6] == node_4_10);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[7] == node_4_11);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[8] == node_4_12);
    REQUIRE(routing_info.getRoutingTableNeighborChildren()[9] == node_4_13);

    REQUIRE_FALSE(routing_info.getChild(0).isInitialized());
    REQUIRE_FALSE(routing_info.getChild(1).isInitialized());
    routing_info.updateNeighbor(node_4_8);
    routing_info.updateNeighbor(node_4_9);
    REQUIRE(routing_info.getChild(0) == node_4_8);
    REQUIRE(routing_info.getChild(1) == node_4_9);
  }
}

TEST_CASE("RoutingInformationGeneralHelper removeNeighbor",
          "[RoutingInformationGeneralHelper][Methods][removeNeighbor]") {
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

  minhton::RoutingInformation routing_info(node_3_4, Logger());
  routing_info.setAdjacentLeft(node_4_8);
  routing_info.setAdjacentRight(node_4_9);
  routing_info.updateNeighbor(node_2_2);  // parent
  routing_info.updateNeighbor(node_3_0);
  // routing_info.updateNeighbor(node_3_1);
  routing_info.updateNeighbor(node_3_2);
  routing_info.updateNeighbor(node_3_3);
  // routing_info.updateNeighbor(node_3_4);
  routing_info.updateNeighbor(node_3_5);
  routing_info.updateNeighbor(node_3_6);
  // routing_info.updateNeighbor(node_3_7);
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

  // cannot remove parent
  REQUIRE_THROWS_AS(routing_info.removeNeighbor(node_2_2), std::logic_error);
  REQUIRE(routing_info.getParent().isInitialized());

  REQUIRE(routing_info.getAdjacentLeft() == routing_info.getChild(0));
  REQUIRE(routing_info.getAdjacentRight() == routing_info.getChild(1));
  REQUIRE(routing_info.getAdjacentLeft().isInitialized());
  REQUIRE(routing_info.getAdjacentRight().isInitialized());

  // adjacent left
  routing_info.removeNeighbor(node_4_8);
  REQUIRE_FALSE(routing_info.getAdjacentLeft().isInitialized());
  REQUIRE_FALSE(routing_info.getChild(0).isInitialized());

  // adjacent right
  routing_info.removeNeighbor(node_4_9);
  REQUIRE_FALSE(routing_info.getAdjacentRight().isInitialized());
  REQUIRE_FALSE(routing_info.getChild(1).isInitialized());

  // rt neighbors
  REQUIRE(routing_info.getRoutingTableNeighbors()[2].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighbors()[2] == node_3_3);
  routing_info.removeNeighbor(node_3_3);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[2].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighbors()[2].getLogicalNodeInfo() ==
          node_3_3.getLogicalNodeInfo());

  REQUIRE(routing_info.getRoutingTableNeighbors()[4].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighbors()[4] == node_3_6);
  routing_info.removeNeighbor(node_3_6);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighbors()[4].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighbors()[4].getLogicalNodeInfo() ==
          node_3_6.getLogicalNodeInfo());

  // rt neighbor children
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[1] == node_4_1);
  routing_info.removeNeighbor(node_4_1);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[1].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].getLogicalNodeInfo() ==
          node_4_1.getLogicalNodeInfo());

  REQUIRE(routing_info.getRoutingTableNeighborChildren()[7].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[7] == node_4_11);
  routing_info.removeNeighbor(node_4_11);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[7].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[7].getLogicalNodeInfo() ==
          node_4_11.getLogicalNodeInfo());
}

TEST_CASE("RoutingInformationGeneralHelper calcRTNeighborParents",
          "[RoutingInformationGeneralHelper][Methods][calcRTNeighborParents]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node(3, 0, fanout);
  minhton::RoutingInformation routing_info(node, Logger());

  auto neighbors = routing_info.calcRoutingTableNeighborParents(node);
  REQUIRE(neighbors.size() == 2);

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 1, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 2, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);
}

TEST_CASE("RoutingInformationGeneralHelper combNodeVectorsWithoutDuplicate",
          "[RoutingInformationGeneralHelper][Methods][combNodeVectorsWithoutDuplicate]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node(0, 0, fanout, "1.2.3.4", 2000);

  minhton::NodeInfo node_a_init(5, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_b_init(5, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_c_init(5, 2, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_d_init(5, 3, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_a_uninit(5, 0, fanout);
  minhton::NodeInfo node_b_uninit(5, 1, fanout);
  minhton::NodeInfo node_c_uninit(5, 2, fanout);
  minhton::NodeInfo node_d_uninit(5, 3, fanout);

  minhton::RoutingInformation routing_info(node, Logger());

  SECTION("Case 1") {
    // no dublicates of same initialized
    std::vector<minhton::NodeInfo> v1{node_a_init, node_b_init};
    std::vector<minhton::NodeInfo> v2{node_b_init, node_c_init};

    auto combi = routing_info.combiningNodeVectorsWithoutDuplicate(v1, v2);
    REQUIRE(combi.size() == 3);

    bool inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_a_init.getLogicalNodeInfo() && n.isInitialized()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);

    inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_b_init.getLogicalNodeInfo() && n.isInitialized()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);

    inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_c_init.getLogicalNodeInfo() && n.isInitialized()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);
  }

  SECTION("Case 2") {
    // no dublicates of same initialized
    std::vector<minhton::NodeInfo> v1{node_a_init, node_b_init};
    std::vector<minhton::NodeInfo> v2{node_b_uninit, node_c_init};

    auto combi = routing_info.combiningNodeVectorsWithoutDuplicate(v1, v2);
    REQUIRE(combi.size() == 3);

    bool inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_a_init.getLogicalNodeInfo() && n.isInitialized()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);

    inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_b_init.getLogicalNodeInfo() && n.isInitialized()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);

    inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_c_init.getLogicalNodeInfo() && n.isInitialized()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);
  }

  SECTION("Case 3") {
    std::vector<minhton::NodeInfo> v1{node_a_uninit, node_b_uninit};
    std::vector<minhton::NodeInfo> v2{node_b_uninit, node_c_init, node_d_uninit};

    auto combi = routing_info.combiningNodeVectorsWithoutDuplicate(v1, v2);
    REQUIRE(combi.size() == 4);

    bool inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_a_uninit.getLogicalNodeInfo()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);

    inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_b_uninit.getLogicalNodeInfo()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);

    inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_c_init.getLogicalNodeInfo() && n.isInitialized()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);

    inside = false;
    for (auto const &n : combi) {
      if (n.getLogicalNodeInfo() == node_d_init.getLogicalNodeInfo()) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);
  }

  SECTION("Case 4") {
    std::vector<minhton::NodeInfo> v1{};
    std::vector<minhton::NodeInfo> v2{node_b_uninit, node_c_init, node_d_uninit};

    auto combi = routing_info.combiningNodeVectorsWithoutDuplicate(v1, v2);
    REQUIRE(combi.size() == 3);
  }

  SECTION("Case 5") {
    std::vector<minhton::NodeInfo> v1{node_b_uninit, node_c_init, node_d_uninit};
    std::vector<minhton::NodeInfo> v2{};

    auto combi = routing_info.combiningNodeVectorsWithoutDuplicate(v1, v2);
    REQUIRE(combi.size() == 3);
  }
}

TEST_CASE("RoutingInformationGeneralHelper getRoutingTableNeighborsAndChildren",
          "[RoutingInformationGeneralHelper][Methods][getRoutingTableNeighborsAndChildren]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node(2, 0, fanout);
  minhton::RoutingInformation routing_info(node, Logger());

  routing_info.updateNeighbor(minhton::NodeInfo(2, 1, fanout, "1.2.3.4", 2000));
  auto neighbors = routing_info.getRoutingTableNeighborsAndChildren();

  REQUIRE(neighbors.size() == 2 + 4);

  bool inside = false;
  for (auto const &neighbor_node : neighbors) {
    if (neighbor_node.isInitialized() &&
        neighbor_node.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 1, fanout)) {
      inside = true;
    }
  }
  REQUIRE(inside);
}

TEST_CASE(
    "RoutingInformationGeneralHelper getInitializedRoutingTableNeighborsAndChildren",
    "[RoutingInformationGeneralHelper][Methods][getInitializedRoutingTableNeighborsAndChildren]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node(2, 0, fanout);
  minhton::RoutingInformation routing_info(node, Logger());

  REQUIRE(routing_info.getInitializedRoutingTableNeighborsAndChildren().size() == 0);

  routing_info.updateNeighbor(minhton::NodeInfo(2, 1, fanout, "1.2.3.4", 2000));
  auto neighbors = routing_info.getInitializedRoutingTableNeighborsAndChildren();
  REQUIRE(neighbors.size() == 1);
  REQUIRE(neighbors[0].getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 1, fanout));
  REQUIRE(neighbors[0].isInitialized());

  routing_info.updateNeighbor(minhton::NodeInfo(3, 3, fanout, "1.2.3.4", 2000));
  REQUIRE(routing_info.getInitializedRoutingTableNeighborsAndChildren().size() == 2);
}

TEST_CASE("RoutingInformationGeneralHelper getNodeInfoByPosition",
          "[RoutingInformationGeneralHelper][Methods][getNodeInfoByPosition]") {
  minhton::RoutingInformation routing_info_empty{};
  // REQUIRE_THROWS_AS(routing_info_empty.getNodeInfoByPosition(3, 2), std::logic_error);

  minhton::NodeInfo node(1, 1, 3);
  minhton::RoutingInformation routing_info(node, Logger());

  // REQUIRE_THROWS_AS(routing_info_empty.getNodeInfoByPosition(1, 1000), std::invalid_argument);

  minhton::NodeInfo node_1_0(1, 0, (uint16_t)3, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_1(2, 1, (uint16_t)3, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_8(2, 8, (uint16_t)3, "1.2.3.4", 2000);
  minhton::NodeInfo node_0_0(0, 0, (uint16_t)3, "1.2.3.4", 2000);

  routing_info.updateRoutingTableNeighbor(node_1_0);
  routing_info.updateRoutingTableNeighborChild(node_2_1);
  routing_info.updateRoutingTableNeighborChild(node_2_8);

  auto answer_1 = routing_info.getNodeInfoByPosition(2, 8);
  auto answer_2 = routing_info.getNodeInfoByPosition(2, 7);
  auto answer_3 = routing_info.getNodeInfoByPosition(2, 2);
  auto answer_4 = routing_info.getNodeInfoByPosition(2, 0);
  auto answer_5 = routing_info.getNodeInfoByPosition(2, 1);
  auto answer_6 = routing_info.getNodeInfoByPosition(1, 1);
  auto answer_7 = routing_info.getNodeInfoByPosition(1, 0);

  REQUIRE(answer_1 == node_2_8);
  REQUIRE(answer_5 == node_2_1);
  REQUIRE(answer_7 == node_1_0);

  REQUIRE_FALSE(answer_2.isInitialized());
  REQUIRE_FALSE(answer_3.isInitialized());
  REQUIRE_FALSE(answer_4.isInitialized());
  REQUIRE_FALSE(answer_6.isInitialized());

  auto answer_8 = routing_info.getNodeInfoByPosition(2, 4);   // adjacent left
  auto answer_9 = routing_info.getNodeInfoByPosition(2, 5);   // adjacent right
  auto answer_10 = routing_info.getNodeInfoByPosition(0, 0);  // parent

  REQUIRE_FALSE(answer_8.isInitialized());
  REQUIRE_FALSE(answer_9.isInitialized());
  REQUIRE_FALSE(answer_10.isInitialized());

  routing_info.updateNeighbor(node_0_0);
  auto answer_11 = routing_info.getNodeInfoByPosition(0, 0);  // parent
  REQUIRE(answer_11 == node_0_0);
}

TEST_CASE("RoutingInformationLeaveHelper getAllUniqueKnownExistingNeighbors",
          "[RoutingInformationLeaveHelper][Methods][getAllUniqueKnownExistingNeighbors]") {
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

  minhton::RoutingInformation routing_info(node_3_4, Logger());
  routing_info.setAdjacentLeft(node_4_8);
  routing_info.setAdjacentRight(node_4_9);
  routing_info.updateNeighbor(node_2_2);
  routing_info.updateNeighbor(node_3_0);
  routing_info.updateNeighbor(node_3_2);
  routing_info.updateNeighbor(node_3_3);
  routing_info.updateNeighbor(node_3_5);
  routing_info.updateNeighbor(node_3_6);
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

  auto neighbors = routing_info.getAllUniqueKnownExistingNeighbors();
  REQUIRE(neighbors.size() == 18);

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 2, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 0, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 2, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 3, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 5, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 6, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 0, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 1, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 4, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 5, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 6, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 7, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 8, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 9, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 10, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 11, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 12, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 13, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  routing_info.removeNeighbor(node_4_4);
  routing_info.removeNeighbor(node_3_6);
  routing_info.removeNeighbor(node_4_9);
  neighbors = routing_info.getAllUniqueKnownExistingNeighbors();
  REQUIRE(neighbors.size() == 15);
}

TEST_CASE("RoutingInformationLeaveHelper getAllUniqueSymmetricalExistingNeighbors",
          "[RoutingInformationLeaveHelper][Methods][getAllUniqueSymmetricalExistingNeighbors]") {
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

  minhton::RoutingInformation routing_info(node_3_4, Logger());
  routing_info.setAdjacentLeft(node_4_8);
  routing_info.setAdjacentRight(node_4_9);
  routing_info.updateNeighbor(node_2_2);
  routing_info.updateNeighbor(node_3_0);
  routing_info.updateNeighbor(node_3_2);
  routing_info.updateNeighbor(node_3_3);
  routing_info.updateNeighbor(node_3_5);
  routing_info.updateNeighbor(node_3_6);
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

  auto neighbors = routing_info.getAllUniqueSymmetricalExistingNeighbors();
  REQUIRE(neighbors.size() == 8);

  bool inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(2, 2, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 0, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 2, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 3, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 5, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 6, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 8, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  inside = false;
  for (auto const &n : neighbors) {
    if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(4, 9, fanout)) {
      inside = true;
      break;
    }
  }
  REQUIRE(inside);

  routing_info.removeNeighbor(node_4_4);
  routing_info.removeNeighbor(node_3_6);
  routing_info.removeNeighbor(node_4_9);
  neighbors = routing_info.getAllUniqueSymmetricalExistingNeighbors();
  REQUIRE(neighbors.size() == 6);
}

TEST_CASE("RoutingInformationLeaveHelper getLowestNode",
          "[RoutingInformationLeaveHelper][Methods][getLowestNode]") {
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

  SECTION("Case 1") {
    minhton::RoutingInformation routing_info(node_0_0, Logger());

    routing_info.setAdjacentLeft(node_3_3);
    routing_info.setAdjacentRight(node_3_4);
    routing_info.setChild(node_1_0, 0);
    routing_info.setChild(node_1_1, 1);

    auto lowest = routing_info.getLowestNode();
    REQUIRE((lowest.getLogicalNodeInfo() == node_3_3.getLogicalNodeInfo() ||
             lowest.getLogicalNodeInfo() == node_3_4.getLogicalNodeInfo()));
    REQUIRE(lowest.isInitialized());
  }

  SECTION("Case 2") {
    minhton::RoutingInformation routing_info(node_3_0, Logger());

    routing_info.setAdjacentLeft(node_4_0);
    routing_info.setAdjacentRight(node_4_1);
    routing_info.setChild(node_4_0, 0);
    routing_info.setChild(node_4_1, 1);
    routing_info.updateNeighbor(node_2_0);
    routing_info.updateNeighbor(node_3_1);
    routing_info.updateNeighbor(node_3_2);
    routing_info.updateNeighbor(node_3_4);
    routing_info.updateNeighbor(node_4_2);
    routing_info.updateNeighbor(node_4_3);
    routing_info.updateNeighbor(node_4_4);
    routing_info.updateNeighbor(node_4_5);
    routing_info.updateNeighbor(node_4_8);
    routing_info.updateNeighbor(node_4_9);

    auto lowest = routing_info.getLowestNode();
    REQUIRE(lowest.getLevel() == 4);
    REQUIRE(lowest.isInitialized());
  }

  SECTION("Case 3") {
    for (uint32_t i = 0; i <= 15; i++) {
      minhton::NodeInfo current_node(4, i, fanout, "1.2.3.4", 2000);
      minhton::RoutingInformation routing_info(current_node, Logger());

      routing_info.updateNeighbor(node_4_0);
      routing_info.updateNeighbor(node_4_1);
      routing_info.updateNeighbor(node_4_2);
      routing_info.updateNeighbor(node_4_3);
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
      routing_info.updateNeighbor(node_4_14);
      routing_info.updateNeighbor(node_4_15);

      auto lowest = routing_info.getLowestNode();
      REQUIRE(lowest.isInitialized());
      REQUIRE(lowest.getLevel() == 4);
    }
  }
}

TEST_CASE(
    "RoutingInformationGeneralHelper removeRoutingTableNeighborChildrenFromVector",
    "[RoutingInformationGeneralHelper][Methods][removeRoutingTableNeighborChildrenFromVector]") {
  SECTION("Fanout 2") {
    uint16_t fanout = 2;

    std::vector<minhton::NodeInfo> vec = {
        NodeInfo(0, 0, fanout),
        NodeInfo(1, 0, fanout),
        NodeInfo(2, 0, fanout),
        NodeInfo(2, 2, fanout, "1.2.3.4", 3000),
        NodeInfo(2, 3, fanout),
        NodeInfo(3, 0, fanout, "127.0.0.1", 6666),  // rt child
        NodeInfo(3, 1, fanout),                     // rt child
        NodeInfo(3, 2, fanout),                     // child
        NodeInfo(3, 3, fanout, "4.3.2.1", 1234),    // child
        NodeInfo(3, 4, fanout),                     // rt child
        NodeInfo(3, 5, fanout),                     // rt child
        NodeInfo(3, 6, fanout, "1.2.36.4", 2312),   // rt child
        NodeInfo(3, 7, fanout),                     // rt child
    };

    // we are 2:1

    auto result = RoutingInformation::removeRoutingTableNeighborChildrenFromVector(
        NodeInfo(2, 1, fanout), vec);
    REQUIRE((result.size() == vec.size() - 6));

    auto has_0_0 = std::find(result.begin(), result.end(), NodeInfo(0, 0, fanout)) != result.end();
    REQUIRE(has_0_0);

    auto has_2_3 = std::find(result.begin(), result.end(), NodeInfo(2, 3, fanout)) != result.end();
    REQUIRE(has_2_3);

    auto has_1_0 = std::find(result.begin(), result.end(), NodeInfo(1, 0, fanout)) != result.end();
    REQUIRE(has_1_0);

    auto has_2_2 = std::find(result.begin(), result.end(),
                             NodeInfo(2, 2, fanout, "1.2.3.4", 3000)) != result.end();
    REQUIRE(has_2_2);

    auto has_3_2 = std::find(result.begin(), result.end(), NodeInfo(3, 2, fanout)) != result.end();
    REQUIRE(has_3_2);

    auto has_3_3 = std::find(result.begin(), result.end(),
                             NodeInfo(3, 3, fanout, "4.3.2.1", 1234)) != result.end();
    REQUIRE(has_3_3);

    auto has_not_3_0 = std::find(result.begin(), result.end(),
                                 NodeInfo(3, 0, fanout, "127.0.0.1", 6666)) == result.end();
    REQUIRE(has_not_3_0);

    auto has_not_3_1 =
        std::find(result.begin(), result.end(), NodeInfo(3, 1, fanout)) == result.end();
    REQUIRE(has_not_3_1);

    auto has_not_3_4 =
        std::find(result.begin(), result.end(), NodeInfo(3, 4, fanout)) == result.end();
    REQUIRE(has_not_3_4);

    auto has_not_3_5 =
        std::find(result.begin(), result.end(), NodeInfo(3, 5, fanout)) == result.end();
    REQUIRE(has_not_3_5);

    auto has_not_3_6 = std::find(result.begin(), result.end(),
                                 NodeInfo(3, 6, fanout, "1.2.36.4", 2312)) == result.end();
    REQUIRE(has_not_3_6);

    auto has_not_3_7 =
        std::find(result.begin(), result.end(), NodeInfo(3, 7, fanout)) == result.end();
    REQUIRE(has_not_3_7);
  }

  SECTION("Fanout 2") {
    uint16_t fanout = 3;

    std::vector<minhton::NodeInfo> vec = {
        NodeInfo(0, 0, fanout), NodeInfo(1, 0, fanout), NodeInfo(1, 1, fanout),
        NodeInfo(2, 0, fanout),  // rt child
        NodeInfo(2, 1, fanout),  // rt child
        NodeInfo(2, 2, fanout),  // rt child
        NodeInfo(2, 3, fanout),  // rt child
        NodeInfo(2, 4, fanout),  // rt child
        NodeInfo(2, 5, fanout),  // rt child
        NodeInfo(2, 6, fanout), NodeInfo(2, 7, fanout), NodeInfo(2, 8, fanout),
    };

    // we are 1:2

    auto result = RoutingInformation::removeRoutingTableNeighborChildrenFromVector(
        NodeInfo(1, 2, fanout), vec);
    REQUIRE((result.size() == 6));

    auto has_0_0 = std::find(result.begin(), result.end(), NodeInfo(0, 0, fanout)) != result.end();
    REQUIRE(has_0_0);

    auto has_1_0 = std::find(result.begin(), result.end(), NodeInfo(1, 0, fanout)) != result.end();
    REQUIRE(has_1_0);

    auto has_1_1 = std::find(result.begin(), result.end(), NodeInfo(1, 1, fanout)) != result.end();
    REQUIRE(has_1_1);

    auto has_not_2_0 =
        std::find(result.begin(), result.end(), NodeInfo(2, 0, fanout)) == result.end();
    REQUIRE(has_not_2_0);

    auto has_not_2_1 =
        std::find(result.begin(), result.end(), NodeInfo(2, 1, fanout)) == result.end();
    REQUIRE(has_not_2_1);

    auto has_not_2_2 =
        std::find(result.begin(), result.end(), NodeInfo(2, 2, fanout)) == result.end();
    REQUIRE(has_not_2_2);

    auto has_not_2_3 =
        std::find(result.begin(), result.end(), NodeInfo(2, 3, fanout)) == result.end();
    REQUIRE(has_not_2_3);

    auto has_not_2_4 =
        std::find(result.begin(), result.end(), NodeInfo(2, 4, fanout)) == result.end();
    REQUIRE(has_not_2_4);

    auto has_not_2_5 =
        std::find(result.begin(), result.end(), NodeInfo(2, 5, fanout)) == result.end();
    REQUIRE(has_not_2_5);
  }
}
