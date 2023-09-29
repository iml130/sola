// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "core/routing_information.h"

#include <catch2/catch_test_macros.hpp>

using namespace minhton;

TEST_CASE("RoutingInformation Default Constructor", "[RoutingInformation][Init]") {
  minhton::RoutingInformation routing_info;

  REQUIRE_FALSE(routing_info.getParent().getPhysicalNodeInfo().isInitialized());
  REQUIRE_FALSE(routing_info.getParent().getLogicalNodeInfo().isInitialized());

  REQUIRE_FALSE(routing_info.getAdjacentLeft().getPhysicalNodeInfo().isInitialized());
  REQUIRE_FALSE(routing_info.getAdjacentLeft().getLogicalNodeInfo().isInitialized());

  REQUIRE_FALSE(routing_info.getAdjacentRight().getPhysicalNodeInfo().isInitialized());
  REQUIRE_FALSE(routing_info.getAdjacentRight().getLogicalNodeInfo().isInitialized());

  REQUIRE(routing_info.getChildren().size() == 0);
  REQUIRE(routing_info.getInitializedChildren().size() == 0);

  REQUIRE(routing_info.getRoutingTableNeighbors().size() == 0);
  REQUIRE(routing_info.getRoutingTableNeighborChildren().size() == 0);
  REQUIRE(routing_info.getRoutingTableNeighbors().size() == 0);
  REQUIRE(routing_info.getRoutingTableNeighborChildren().size() == 0);

  REQUIRE_THROWS_AS(routing_info.getFanout(), std::logic_error);
}

TEST_CASE("RoutingInformation Constructor", "[RoutingInformation][Init]") {
  uint16_t fanout = 6;

  minhton::NodeInfo node(1, 1, fanout);
  minhton::RoutingInformation routing_info(node, Logger());

  REQUIRE(routing_info.getParent().getLogicalNodeInfo().isInitialized());
  REQUIRE_FALSE(routing_info.getParent().getPhysicalNodeInfo().isInitialized());

  REQUIRE_FALSE(routing_info.getAdjacentLeft().getPhysicalNodeInfo().isInitialized());
  REQUIRE_FALSE(routing_info.getAdjacentLeft().getLogicalNodeInfo().isInitialized());

  REQUIRE_FALSE(routing_info.getAdjacentRight().getPhysicalNodeInfo().isInitialized());
  REQUIRE_FALSE(routing_info.getAdjacentRight().getLogicalNodeInfo().isInitialized());

  REQUIRE(routing_info.getChildren().size() == fanout);
  REQUIRE(routing_info.getInitializedChildren().size() == 0);

  REQUIRE(routing_info.getRoutingTableNeighbors().size() > 0);
  REQUIRE(routing_info.getRoutingTableNeighborChildren().size() > 0);
  REQUIRE(routing_info.getRoutingTableNeighbors().size() > 0);
  REQUIRE(routing_info.getRoutingTableNeighborChildren().size() > 0);
}

TEST_CASE("RoutingInformation Getter Fanout", "[RoutingInformation][Attributes][Fanout]") {
  minhton::RoutingInformation routing_info_1;
  REQUIRE_THROWS_AS(routing_info_1.getFanout(), std::logic_error);

  minhton::NodeInfo node_1(0, 0, (uint16_t)2);
  minhton::NodeInfo node_2(0, 0, (uint16_t)3);
  minhton::NodeInfo node_3(0, 0, (uint16_t)15);

  minhton::RoutingInformation routing_info_2(node_1, Logger());
  REQUIRE(routing_info_2.getFanout() == 2);

  minhton::RoutingInformation routing_info_3(node_2, Logger());
  REQUIRE(routing_info_3.getFanout() == 3);

  minhton::RoutingInformation routing_info_15(node_3, Logger());
  REQUIRE(routing_info_15.getFanout() == 15);
}

TEST_CASE("RoutingInformation Setter/Getter Parent", "[RoutingInformation][Attributes][Parent]") {
  // not setting parent when routing infos aren't initialized
  minhton::RoutingInformation routing_info_1;
  REQUIRE_THROWS_AS(routing_info_1.setParent(minhton::NodeInfo(1, 0, (uint16_t)2)),
                    std::logic_error);

  // not setting parent for root
  minhton::NodeInfo node_2(0, 0, (uint16_t)15);
  minhton::RoutingInformation routing_info_2(node_2, Logger());
  REQUIRE_THROWS_AS(routing_info_2.setParent(minhton::NodeInfo(1, 0, (uint16_t)15)),
                    std::logic_error);

  // setting parent with different fanout
  minhton::NodeInfo node_3(1, 1, (uint16_t)15);
  minhton::RoutingInformation routing_info_3(node_3, Logger());
  REQUIRE_THROWS_AS(routing_info_3.setParent(minhton::NodeInfo(0, 0, (uint16_t)14)),
                    std::invalid_argument);

  // setting wrong and right parents
  minhton::NodeInfo node_4(2, 2, (uint16_t)3);
  minhton::RoutingInformation routing_info_4(node_4, Logger());  // right parent is
  REQUIRE_THROWS_AS(routing_info_4.setParent(minhton::NodeInfo(1, 1, (uint16_t)3)),
                    std::invalid_argument);

  // setting uninitialized parent
  minhton::NodeInfo node_4_parent(1, 0, (uint16_t)3);
  REQUIRE_THROWS_AS(routing_info_4.setParent(node_4_parent), std::invalid_argument);

  // setting initalized parent at right position
  node_4_parent.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("1.2.3.4", 2000));
  REQUIRE(node_4_parent.isInitialized());
  REQUIRE_NOTHROW(routing_info_4.setParent(node_4_parent));
  REQUIRE(routing_info_4.getParent().getLevel() == 1);
  REQUIRE(routing_info_4.getParent().getNumber() == 0);
  REQUIRE(routing_info_4.getParent().isInitialized());
}

TEST_CASE("RoutingInformation Setter/Getter Child", "[RoutingInformation][Attributes][Children]") {
  minhton::RoutingInformation routing_info_1;
  REQUIRE(routing_info_1.getChildren().size() == 0);
  REQUIRE(routing_info_1.getInitializedChildren().size() == 0);
  // setting while routing infos are uninitialized
  REQUIRE_THROWS_AS(routing_info_1.setChild(minhton::NodeInfo(), 0), std::logic_error);
  REQUIRE_THROWS_AS(routing_info_1.getChild(0), std::logic_error);

  minhton::NodeInfo node_2(0, 0, (uint16_t)2);
  minhton::RoutingInformation routing_info_2(node_2, Logger());
  REQUIRE(routing_info_2.getChildren().size() == 2);
  REQUIRE(routing_info_2.getChildren()[0].getLogicalNodeInfo().isInitialized());
  REQUIRE(routing_info_2.getChildren()[1].getLogicalNodeInfo().isInitialized());
  // setting at invalid position
  REQUIRE_THROWS_AS(routing_info_2.setChild(minhton::NodeInfo(), 3), std::out_of_range);
  REQUIRE_THROWS_AS(routing_info_2.setChild(minhton::NodeInfo(), 4), std::out_of_range);

  // setting with wrong tree position
  REQUIRE_THROWS_AS(routing_info_2.setChild(minhton::NodeInfo(1, 1, (uint16_t)2), 0),
                    std::invalid_argument);

  // setting with wrong fanout
  REQUIRE_THROWS_AS(routing_info_2.setChild(minhton::NodeInfo(1, 0, (uint16_t)3), 0),
                    std::invalid_argument);

  // setting with uninitialized PhysicalNodeInfo
  minhton::NodeInfo node_2_child_0(1, 0, (uint16_t)2);
  minhton::NodeInfo node_2_child_1(1, 1, (uint16_t)2);
  REQUIRE_THROWS_AS(routing_info_2.setChild(node_2_child_0, 0), std::invalid_argument);
  REQUIRE_THROWS_AS(routing_info_2.setChild(node_2_child_1, 0), std::invalid_argument);

  // setting correctly with initialized PhysicalNodeInfo
  node_2_child_0.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("1.2.3.4", 2000));
  node_2_child_1.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("1.2.3.5", 2001));
  REQUIRE_NOTHROW(routing_info_2.setChild(node_2_child_0, 0));
  REQUIRE(routing_info_2.getChild(0).getLevel() == 1);
  REQUIRE(routing_info_2.getChild(0).getNumber() == 0);
  REQUIRE(routing_info_2.getChild(0).getAddress() == "1.2.3.4");
  REQUIRE(routing_info_2.getChild(0).isInitialized());
  REQUIRE(routing_info_2.getInitializedChildren().size() == 1);

  REQUIRE_NOTHROW(routing_info_2.setChild(node_2_child_1, 1));
  REQUIRE(routing_info_2.getChild(1).isInitialized());
  REQUIRE(routing_info_2.getInitializedChildren().size() == 2);

  routing_info_2.resetChild(0);
  REQUIRE_FALSE(routing_info_2.getChild(0).isInitialized());
  REQUIRE(routing_info_2.getChild(0).getLogicalNodeInfo().isInitialized());

  REQUIRE_THROWS_AS(routing_info_2.resetChild(15), std::invalid_argument);
  REQUIRE_THROWS_AS(routing_info_2.getChild(2), std::out_of_range);
  REQUIRE_THROWS_AS(routing_info_2.getChild(3), std::out_of_range);
}

TEST_CASE("RoutingInformation Setter/Getter AdjacentLeft AdjacentRight",
          "[RoutingInformation][Attributes][AdjacentLeft]") {
  minhton::RoutingInformation routing_info_1;
  // setting with uninitialized routing info
  REQUIRE_THROWS_AS(routing_info_1.setAdjacentLeft(minhton::NodeInfo()), std::logic_error);
  REQUIRE_THROWS_AS(routing_info_1.setAdjacentRight(minhton::NodeInfo()), std::logic_error);

  // setting with wrong fanout
  minhton::NodeInfo node_5(0, 0, (uint16_t)5);
  minhton::RoutingInformation routing_info_5(node_5, Logger());
  REQUIRE_THROWS_AS(routing_info_5.setAdjacentLeft(minhton::NodeInfo(1, 1, (uint16_t)4)),
                    std::invalid_argument);
  REQUIRE_THROWS_AS(routing_info_5.setAdjacentRight(minhton::NodeInfo(1, 2, (uint16_t)3)),
                    std::invalid_argument);

  // setting with uninitialized PhysicalNodeInfo
  REQUIRE_THROWS_AS(routing_info_5.setAdjacentLeft(minhton::NodeInfo(1, 1, (uint16_t)5)),
                    std::invalid_argument);
  REQUIRE_THROWS_AS(routing_info_5.setAdjacentRight(minhton::NodeInfo(1, 3, (uint16_t)5)),
                    std::invalid_argument);

  // setting on the wrong horizontal side
  minhton::NodeInfo child_on_left(1, 0, (uint16_t)5, "1.2.3.4", 2000);
  minhton::NodeInfo child_on_right(1, 3, (uint16_t)5, "1.2.3.4", 2000);
  minhton::NodeInfo child_identical(0, 0, (uint16_t)5, "1.2.3.4", 2000);
  REQUIRE_THROWS_AS(routing_info_5.setAdjacentLeft(child_identical), std::invalid_argument);
  REQUIRE_THROWS_AS(routing_info_5.setAdjacentRight(child_identical), std::invalid_argument);
  REQUIRE_THROWS_AS(routing_info_5.setAdjacentLeft(child_on_right), std::invalid_argument);
  REQUIRE_THROWS_AS(routing_info_5.setAdjacentRight(child_on_left), std::invalid_argument);

  // setting on right side
  REQUIRE_NOTHROW(routing_info_5.setAdjacentLeft(child_on_left));
  REQUIRE_NOTHROW(routing_info_5.setAdjacentRight(child_on_right));
  REQUIRE(routing_info_5.getAdjacentLeft().isInitialized());
  REQUIRE(routing_info_5.getAdjacentRight().isInitialized());

  routing_info_5.resetAdjacentRight();
  routing_info_5.resetAdjacentLeft();
  REQUIRE_FALSE(routing_info_5.getAdjacentLeft().isInitialized());
  REQUIRE_FALSE(routing_info_5.getAdjacentRight().isInitialized());
}

TEST_CASE("RoutingInformation resetChildOrRoutingTableNeighborChild",
          "[RoutingInformation][Method][resetChildOrRoutingTableNeighborChild]") {
  uint16_t fanout = 2;

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

  minhton::RoutingInformation routing_info(node_2_1, Logger());
  routing_info.updateRoutingTableNeighbor(node_2_0);
  routing_info.updateRoutingTableNeighbor(node_2_2);
  routing_info.updateRoutingTableNeighbor(node_2_3);
  routing_info.updateRoutingTableNeighborChild(node_3_0);
  routing_info.updateRoutingTableNeighborChild(node_3_1);
  routing_info.updateRoutingTableNeighborChild(node_3_4);
  routing_info.updateRoutingTableNeighborChild(node_3_5);
  routing_info.updateRoutingTableNeighborChild(node_3_6);
  routing_info.updateRoutingTableNeighborChild(node_3_7);

  routing_info.setChild(node_3_2, 0);
  routing_info.setChild(node_3_3, 1);
  REQUIRE(routing_info.getChild(0).isInitialized());
  REQUIRE(routing_info.getChild(1).isInitialized());

  REQUIRE(routing_info.getRoutingTableNeighborChildren()[0].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[1].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[2].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[3].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[4].isInitialized());
  REQUIRE(routing_info.getRoutingTableNeighborChildren()[5].isInitialized());

  routing_info.resetChildOrRoutingTableNeighborChild(node_3_0);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[0].isInitialized());

  routing_info.resetChildOrRoutingTableNeighborChild(node_3_1);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[1].isInitialized());

  routing_info.resetChildOrRoutingTableNeighborChild(node_3_4);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[2].isInitialized());

  routing_info.resetChildOrRoutingTableNeighborChild(node_3_5);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[3].isInitialized());

  routing_info.resetChildOrRoutingTableNeighborChild(node_3_6);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[4].isInitialized());

  routing_info.resetChildOrRoutingTableNeighborChild(node_3_7);
  REQUIRE_FALSE(routing_info.getRoutingTableNeighborChildren()[5].isInitialized());

  REQUIRE(routing_info.getChild(0).isInitialized());
  routing_info.resetChildOrRoutingTableNeighborChild(node_3_2);
  REQUIRE_FALSE(routing_info.getChild(0).isInitialized());

  REQUIRE(routing_info.getChild(1).isInitialized());
  routing_info.resetChildOrRoutingTableNeighborChild(node_3_3);
  REQUIRE_FALSE(routing_info.getChild(1).isInitialized());
}

TEST_CASE("RoutingInformationLeaveHelper atLeastOneChildExists",
          "[RoutingInformationLeaveHelper][Methods][atLeastOneChildExists]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
  minhton::RoutingInformation routing_info(node_1_0, Logger());

  REQUIRE_FALSE(routing_info.atLeastOneChildExists());
  routing_info.updateNeighbor(node_2_0);
  REQUIRE(routing_info.atLeastOneChildExists());
  routing_info.updateNeighbor(node_2_1);
  REQUIRE(routing_info.atLeastOneChildExists());

  routing_info.removeNeighbor(node_2_0);
  REQUIRE(routing_info.atLeastOneChildExists());
  routing_info.removeNeighbor(node_2_1);
  REQUIRE_FALSE(routing_info.atLeastOneChildExists());
}

TEST_CASE("RoutingInformationLeaveHelper atLeastOneChildIsFree",
          "[RoutingInformationLeaveHelper][Methods][atLeastOneChildIsFree]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
  minhton::RoutingInformation routing_info(node_1_0, Logger());

  REQUIRE(routing_info.atLeastOneChildIsFree());
  routing_info.updateNeighbor(node_2_0);
  REQUIRE(routing_info.atLeastOneChildIsFree());
  routing_info.updateNeighbor(node_2_1);
  REQUIRE_FALSE(routing_info.atLeastOneChildIsFree());

  routing_info.removeNeighbor(node_2_0);
  REQUIRE(routing_info.atLeastOneChildIsFree());
  routing_info.removeNeighbor(node_2_1);
  REQUIRE(routing_info.atLeastOneChildIsFree());
}

TEST_CASE("RoutingInformationLeaveHelper atLeastOneRTNChildExists",
          "[RoutingInformationLeaveHelper][Methods][atLeastOneRTNChildExists]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_1_1(1, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);
  minhton::RoutingInformation routing_info(node_1_0, Logger());

  routing_info.updateNeighbor(node_2_0);
  routing_info.updateNeighbor(node_2_1);
  routing_info.updateNeighbor(node_1_1);
  REQUIRE_FALSE(routing_info.atLeastOneRoutingTableNeighborChildExists());

  routing_info.updateNeighbor(node_2_2);
  REQUIRE(routing_info.atLeastOneRoutingTableNeighborChildExists());
  routing_info.updateNeighbor(node_2_3);
  REQUIRE(routing_info.atLeastOneRoutingTableNeighborChildExists());

  routing_info.removeNeighbor(node_2_2);
  REQUIRE(routing_info.atLeastOneRoutingTableNeighborChildExists());
  routing_info.removeNeighbor(node_2_3);
  REQUIRE_FALSE(routing_info.atLeastOneRoutingTableNeighborChildExists());
}

TEST_CASE("RoutingInformationLeaveHelper atLeastOneRTNeighborChildIsFree",
          "[RoutingInformationLeaveHelper][Methods][atLeastOneRTNeighborChildIsFree]") {
  uint16_t fanout = 2;

  minhton::NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_1_1(1, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
  minhton::NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);
  minhton::RoutingInformation routing_info(node_1_0, Logger());

  routing_info.updateNeighbor(node_2_0);
  routing_info.updateNeighbor(node_2_1);
  routing_info.updateNeighbor(node_1_1);
  REQUIRE(routing_info.atLeastOneRoutingTableNeighborChildIsFree());

  routing_info.updateNeighbor(node_2_2);
  REQUIRE(routing_info.atLeastOneRoutingTableNeighborChildIsFree());
  routing_info.updateNeighbor(node_2_3);
  REQUIRE_FALSE(routing_info.atLeastOneRoutingTableNeighborChildIsFree());

  routing_info.removeNeighbor(node_2_2);
  REQUIRE(routing_info.atLeastOneRoutingTableNeighborChildIsFree());
  routing_info.removeNeighbor(node_2_3);
  REQUIRE(routing_info.atLeastOneRoutingTableNeighborChildIsFree());
}

TEST_CASE("RoutingInformation setPosition", "[RoutingInformation][Methods][setPosition]") {
  uint16_t fanout = 2;
  minhton::NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
  minhton::RoutingInformation routing_info(node_2_0, Logger());

  routing_info.setParent(NodeInfo(1, 0, fanout, "2.3.4.5", 4000));
  routing_info.setAdjacentLeft(NodeInfo(3, 0, fanout, "2.2.4.5", 4000));
  routing_info.setAdjacentRight(NodeInfo(1, 0, fanout, "2.2.4.6", 4000));
  routing_info.setChild(NodeInfo(3, 0, fanout, "4.4.4.4", 5000), 0);
  routing_info.updateRoutingTableNeighbor(NodeInfo(2, 1, fanout, "5.6.7.8", 2000));

  REQUIRE(routing_info.getParent().isInitialized());
  REQUIRE(routing_info.getAdjacentLeft().isInitialized());
  REQUIRE(routing_info.getAdjacentRight().isInitialized());
  REQUIRE(routing_info.getInitializedRoutingTableNeighborsAndChildren().size() == 1);
  REQUIRE(routing_info.getInitializedChildren().size() == 1);

  minhton::LogicalNodeInfo peer_2_3(2, 1, fanout);
  routing_info.setPosition(peer_2_3);

  REQUIRE(!routing_info.getParent().isInitialized());
  REQUIRE(routing_info.getParent().getLogicalNodeInfo().isInitialized());
  REQUIRE(!routing_info.getAdjacentLeft().getLogicalNodeInfo().isInitialized());
  REQUIRE(!routing_info.getAdjacentRight().getLogicalNodeInfo().isInitialized());
  REQUIRE(!routing_info.getAdjacentLeft().getPhysicalNodeInfo().isInitialized());
  REQUIRE(!routing_info.getAdjacentRight().getPhysicalNodeInfo().isInitialized());
  REQUIRE(routing_info.getInitializedRoutingTableNeighborsAndChildren().size() == 0);
  REQUIRE(routing_info.getInitializedChildren().size() == 0);
  REQUIRE(routing_info.getChild(0).getLogicalNodeInfo().isInitialized());
  REQUIRE(routing_info.getChild(1).getLogicalNodeInfo().isInitialized());

  routing_info.setParent(NodeInfo(1, 0, fanout, "2.3.4.5", 4000));
  routing_info.setAdjacentLeft(NodeInfo(1, 0, fanout, "2.2.4.5", 4000));
  routing_info.setAdjacentRight(NodeInfo(3, 3, fanout, "2.2.4.6", 4000));

  routing_info.resetPosition(0);

  REQUIRE(!routing_info.getParent().isInitialized());
  REQUIRE(!routing_info.getParent().getLogicalNodeInfo().isInitialized());
  REQUIRE(!routing_info.getAdjacentLeft().getLogicalNodeInfo().isInitialized());
  REQUIRE(!routing_info.getAdjacentRight().getLogicalNodeInfo().isInitialized());
  REQUIRE(!routing_info.getAdjacentLeft().getPhysicalNodeInfo().isInitialized());
  REQUIRE(!routing_info.getAdjacentRight().getPhysicalNodeInfo().isInitialized());
  REQUIRE(routing_info.getInitializedRoutingTableNeighborsAndChildren().size() == 0);
  REQUIRE(routing_info.getInitializedChildren().size() == 0);
}
