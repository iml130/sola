// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

#include "algorithms/join/minhton_join_algorithm.h"
#include "exception/algorithm_exception.h"
#include "message/join.h"

using namespace minhton;

class MinhtonJoinAlgorithmForTest : public MinhtonJoinAlgorithm {
public:
  explicit MinhtonJoinAlgorithmForTest(std::shared_ptr<AccessContainer> access)
      : MinhtonJoinAlgorithm(access){};

  using MinhtonJoinAlgorithm::calcAdjacentLeftOfNewChild;
  using MinhtonJoinAlgorithm::calcAdjacentRightOfNewChild;
  using MinhtonJoinAlgorithm::calcOurNewAdjacentLeft;
  using MinhtonJoinAlgorithm::calcOurNewAdjacentRight;
  using MinhtonJoinAlgorithm::getCloserAdjacent;
  using MinhtonJoinAlgorithm::getRoutingTableNeighborsForNewChild;
  using MinhtonJoinAlgorithm::mustSendUpdateLeft;
  using MinhtonJoinAlgorithm::mustSendUpdateRight;
  using MinhtonJoinAlgorithm::performSendUpdateNeighborMessagesAboutEnteringNode;
};

TEST_CASE("JoinAlgorithmGeneral calcAdjacents Fanout 2",
          "[MinhtonSearchExactAlgorithm][calcAdjacents][Fanout][2]") {
  uint16_t fanout = 2;

  SECTION("Scenario 2.1 - on Root, Fanout 2") {
    /// Scenario 1
    ///
    ///   A
    ///  / \ 
    /// x   B
    ///
    /// We are A and our current adjacent right does not exist.
    /// We accept B as a child. B's new adjacent left is A.
    /// A's new adjacent right is B. B's adjacent right does not exist.

    minhton::NodeInfo node_a = minhton::NodeInfo(0, 0, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_b = minhton::NodeInfo(1, 1, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_x = minhton::NodeInfo(1, 0, fanout, "127.0.0.1", 1234);

    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    // build routing information of A
    routing_info->setChild(node_x, 0);
    routing_info->setAdjacentLeft(node_x);

    // adjacent left of B is A
    minhton::NodeInfo adj_left_of_b = join_algo.calcAdjacentLeftOfNewChild(node_b);
    REQUIRE(adj_left_of_b.isInitialized());
    REQUIRE(adj_left_of_b.getLogicalNodeInfo() == node_a.getLogicalNodeInfo());

    // adjacent right of B does not exist
    minhton::NodeInfo adj_right_of_b = join_algo.calcAdjacentRightOfNewChild(node_b);
    REQUIRE_FALSE(adj_right_of_b.isInitialized());

    // we dont have to send update left or right messages
    REQUIRE_FALSE(join_algo.mustSendUpdateLeft(adj_right_of_b));
    REQUIRE_FALSE(join_algo.mustSendUpdateRight(adj_left_of_b));

    // A's new adjacent right is B
    REQUIRE(join_algo.calcOurNewAdjacentRight(node_b, adj_left_of_b).getLogicalNodeInfo() ==
            node_b.getLogicalNodeInfo());

    // A's adjacent left does not change
    REQUIRE(join_algo.calcOurNewAdjacentLeft(node_b, adj_right_of_b).getLogicalNodeInfo() ==
            node_x.getLogicalNodeInfo());
  }

  SECTION("Scenario 2.2, Fanout 2") {
    /// Scenario 2
    ///
    ///            x
    ///           / \ 
    ///         /     \ 
    ///       /         \ 
    ///      B           x
    ///     / \         / \ 
    ///    /   \       /   \ 
    ///   x     A     x     x
    ///  / \   / \   / \   / \ 
    /// x   x C   x x   x x   x
    ///
    /// We are A and our current adjacent right is B
    /// We accept C as a child. C's new adjacent left is A.
    /// A's new adjacent right is C.

    minhton::NodeInfo node_a = minhton::NodeInfo(2, 1, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_b = minhton::NodeInfo(1, 0, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_c = minhton::NodeInfo(3, 2, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_x = minhton::NodeInfo(3, 3, fanout, "127.0.0.1", 1234);

    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    // build routing information of A
    routing_info->setParent(node_b);
    routing_info->setAdjacentLeft(node_b);
    routing_info->setAdjacentRight(node_x);
    routing_info->setChild(node_x, 1);

    // adjacent left of C is B
    minhton::NodeInfo adj_left_of_c = join_algo.calcAdjacentLeftOfNewChild(node_c);
    REQUIRE(adj_left_of_c.getLogicalNodeInfo() == node_b.getLogicalNodeInfo());

    // adjacent right of C is A
    minhton::NodeInfo adj_right_of_c = join_algo.calcAdjacentRightOfNewChild(node_c);
    REQUIRE(adj_right_of_c.getLogicalNodeInfo() == node_a.getLogicalNodeInfo());

    // we have to send an update right to B, because C is in between B and A now
    // update left and update right cannot be both true simultaneously
    REQUIRE(join_algo.mustSendUpdateRight(adj_left_of_c));
    REQUIRE_FALSE(join_algo.mustSendUpdateLeft(adj_right_of_c));

    // A's new adjacent left is C
    REQUIRE(join_algo.calcOurNewAdjacentLeft(node_c, adj_right_of_c).getLogicalNodeInfo() ==
            node_c.getLogicalNodeInfo());

    // A's adjacent right does not change
    REQUIRE(join_algo.calcOurNewAdjacentRight(node_c, adj_left_of_c).getLogicalNodeInfo() ==
            node_x.getLogicalNodeInfo());
  }
}

TEST_CASE("JoinAlgorithmGeneral calcAdjacents Fanout 3",
          "[JoinAlgorithmGeneral][calcAdjacents][Fanout][3]") {
  uint16_t fanout = 3;

  SECTION("Scenario 3.1") {
    minhton::NodeInfo node_a = minhton::NodeInfo(0, 0, fanout, "127.0.0.1", 1235);  // us
    minhton::NodeInfo node_b = minhton::NodeInfo(1, 0, fanout, "127.0.0.1", 1235);  // entering node
    minhton::NodeInfo node_x = minhton::NodeInfo(1, 1, fanout, "127.0.0.1", 1235);
    minhton::NodeInfo node_y = minhton::NodeInfo(1, 2, fanout, "127.0.0.1", 1235);

    // build routing information of A

    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    routing_info->setAdjacentLeft(node_x);
    routing_info->setAdjacentRight(node_y);
    routing_info->setChild(node_x, 1);
    routing_info->setChild(node_y, 2);

    // B has no left adjacent
    minhton::NodeInfo adj_left_of_b = join_algo.calcAdjacentLeftOfNewChild(node_b);
    REQUIRE_FALSE(adj_left_of_b.isInitialized());

    // adj right of B is X
    minhton::NodeInfo adj_right_of_b = join_algo.calcAdjacentRightOfNewChild(node_b);
    REQUIRE(adj_right_of_b.getLogicalNodeInfo() == node_x.getLogicalNodeInfo());

    REQUIRE_FALSE(join_algo.mustSendUpdateRight(adj_left_of_b));
    REQUIRE(join_algo.mustSendUpdateLeft(adj_right_of_b));

    minhton::NodeInfo adj_left_of_a = join_algo.calcOurNewAdjacentLeft(node_b, adj_right_of_b);
    REQUIRE(adj_left_of_a.getLogicalNodeInfo() == node_x.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_a = join_algo.calcOurNewAdjacentRight(node_b, adj_left_of_b);
    REQUIRE(adj_right_of_a.getLogicalNodeInfo() == node_y.getLogicalNodeInfo());
  }

  SECTION("Scenario 3.2") {
    minhton::NodeInfo node_a = minhton::NodeInfo(1, 2, fanout, "127.0.0.1", 1234);  // us
    minhton::NodeInfo node_y = minhton::NodeInfo(2, 6, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_b = minhton::NodeInfo(2, 7, fanout, "127.0.0.1", 1234);  // entering node
    minhton::NodeInfo node_z = minhton::NodeInfo(2, 8, fanout, "127.0.0.1", 1234);

    // build routing information of A
    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    routing_info->setAdjacentLeft(node_y);
    routing_info->setAdjacentRight(node_z);
    routing_info->setChild(node_y, 0);
    routing_info->setChild(node_z, 2);

    minhton::NodeInfo adj_left_of_b = join_algo.calcAdjacentLeftOfNewChild(node_b);
    REQUIRE(adj_left_of_b.getLogicalNodeInfo() == node_y.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_b = join_algo.calcAdjacentRightOfNewChild(node_b);
    REQUIRE(adj_right_of_b.getLogicalNodeInfo() == node_a.getLogicalNodeInfo());

    REQUIRE(join_algo.mustSendUpdateRight(adj_left_of_b));
    REQUIRE_FALSE(join_algo.mustSendUpdateLeft(adj_right_of_b));

    minhton::NodeInfo adj_left_of_a = join_algo.calcOurNewAdjacentLeft(node_b, adj_right_of_b);
    REQUIRE(adj_left_of_a.getLogicalNodeInfo() == node_b.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_a = join_algo.calcOurNewAdjacentRight(node_b, adj_left_of_b);
    REQUIRE(adj_right_of_a.getLogicalNodeInfo() == node_z.getLogicalNodeInfo());
  }

  SECTION("Scenario 3.3") {
    minhton::NodeInfo node_p = minhton::NodeInfo(0, 0, fanout, "127.0.0.1", 1234);  // parent of us
    minhton::NodeInfo node_a = minhton::NodeInfo(1, 1, fanout, "127.0.0.1", 1234);  // us
    minhton::NodeInfo node_x = minhton::NodeInfo(2, 3, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_y = minhton::NodeInfo(2, 4, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_b = minhton::NodeInfo(2, 5, fanout, "127.0.0.1", 1234);  // entering node

    // build routing information of A
    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    routing_info->setParent(node_p);
    routing_info->setAdjacentLeft(node_y);
    routing_info->setAdjacentRight(node_p);
    routing_info->setChild(node_x, 0);
    routing_info->setChild(node_y, 1);

    minhton::NodeInfo adj_left_of_b = join_algo.calcAdjacentLeftOfNewChild(node_b);
    REQUIRE(adj_left_of_b.getLogicalNodeInfo() == node_a.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_b = join_algo.calcAdjacentRightOfNewChild(node_b);
    REQUIRE(adj_right_of_b.getLogicalNodeInfo() == node_p.getLogicalNodeInfo());

    REQUIRE_FALSE(join_algo.mustSendUpdateRight(adj_left_of_b));
    REQUIRE(join_algo.mustSendUpdateLeft(adj_right_of_b));

    minhton::NodeInfo adj_left_of_a = join_algo.calcOurNewAdjacentLeft(node_b, adj_right_of_b);
    REQUIRE(adj_left_of_a.getLogicalNodeInfo() == node_y.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_a = join_algo.calcOurNewAdjacentRight(node_b, adj_left_of_b);
    REQUIRE(adj_right_of_a.getLogicalNodeInfo() == node_b.getLogicalNodeInfo());
  }
}

TEST_CASE("JoinAlgorithmGeneral calcAdjacents Fanout 4",
          "[JoinAlgorithmGeneral][calcAdjacents][Fanout][4]") {
  uint16_t fanout = 4;

  SECTION("Scenario 4.1") {
    minhton::NodeInfo node_a = minhton::NodeInfo(0, 0, fanout, "127.0.0.1", 1234);  // us
    minhton::NodeInfo node_x = minhton::NodeInfo(1, 0, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_y = minhton::NodeInfo(1, 1, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_b = minhton::NodeInfo(1, 2, fanout, "127.0.0.1", 1234);  // entering node
    minhton::NodeInfo node_z = minhton::NodeInfo(1, 3, fanout, "127.0.0.1", 1234);

    // build routing information of A
    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);
    routing_info->setAdjacentLeft(node_y);
    routing_info->setAdjacentRight(node_z);
    routing_info->setChild(node_x, 0);
    routing_info->setChild(node_y, 1);
    routing_info->setChild(node_z, 3);

    minhton::NodeInfo adj_left_of_b = join_algo.calcAdjacentLeftOfNewChild(node_b);
    REQUIRE(adj_left_of_b.getLogicalNodeInfo() == node_a.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_b = join_algo.calcAdjacentRightOfNewChild(node_b);
    REQUIRE(adj_right_of_b.isInitialized());
    REQUIRE(adj_right_of_b.getLogicalNodeInfo() == node_z.getLogicalNodeInfo());

    REQUIRE_FALSE(join_algo.mustSendUpdateRight(adj_left_of_b));
    REQUIRE(join_algo.mustSendUpdateLeft(adj_right_of_b));

    minhton::NodeInfo adj_left_of_a = join_algo.calcOurNewAdjacentLeft(node_b, adj_right_of_b);
    REQUIRE(adj_left_of_a.getLogicalNodeInfo() == node_y.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_a = join_algo.calcOurNewAdjacentRight(node_b, adj_left_of_b);
    REQUIRE(adj_right_of_a.getLogicalNodeInfo() == node_b.getLogicalNodeInfo());
  }

  SECTION("Scenario 4.2") {
    minhton::NodeInfo node_p = minhton::NodeInfo(0, 0, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_a = minhton::NodeInfo(1, 0, fanout, "127.0.0.1", 1234);  // us
    minhton::NodeInfo node_c = minhton::NodeInfo(1, 1, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_x0 = minhton::NodeInfo(2, 0, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_x1 = minhton::NodeInfo(2, 1, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_x2 = minhton::NodeInfo(2, 2, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_b = minhton::NodeInfo(2, 3, fanout, "127.0.0.1", 1234);  // entering node
    minhton::NodeInfo node_y0 = minhton::NodeInfo(2, 4, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_y1 = minhton::NodeInfo(2, 5, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_y2 = minhton::NodeInfo(2, 6, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_y3 = minhton::NodeInfo(2, 7, fanout, "127.0.0.1", 1234);

    // build routing information of A
    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);
    routing_info->setParent(node_p);
    routing_info->setAdjacentLeft(node_x1);
    routing_info->setAdjacentRight(node_x2);
    routing_info->setChild(node_x0, 0);
    routing_info->setChild(node_x1, 1);
    routing_info->setChild(node_x2, 2);

    routing_info->updateRoutingTableNeighbor(node_c);
    routing_info->updateRoutingTableNeighborChild(node_y0);
    routing_info->updateRoutingTableNeighborChild(node_y1);
    routing_info->updateRoutingTableNeighborChild(node_y2);
    routing_info->updateRoutingTableNeighborChild(node_y3);

    minhton::NodeInfo adj_left_of_b = join_algo.calcAdjacentLeftOfNewChild(node_b);
    REQUIRE(adj_left_of_b.getLogicalNodeInfo() == node_x2.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_b = join_algo.calcAdjacentRightOfNewChild(node_b);
    REQUIRE(adj_right_of_b.getLogicalNodeInfo() == node_y0.getLogicalNodeInfo());

    REQUIRE(join_algo.mustSendUpdateRight(adj_left_of_b));
    REQUIRE(join_algo.mustSendUpdateLeft(adj_right_of_b));

    minhton::NodeInfo adj_left_of_a = join_algo.calcOurNewAdjacentLeft(node_b, adj_right_of_b);
    REQUIRE(adj_left_of_a.getLogicalNodeInfo() == node_x1.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_a = join_algo.calcOurNewAdjacentRight(node_b, adj_left_of_b);
    REQUIRE(adj_right_of_a.getLogicalNodeInfo() == node_x2.getLogicalNodeInfo());
  }
}

TEST_CASE("JoinAlgorithmGeneral calcAdjacents Fanout 5",
          "[JoinAlgorithmGeneral][calcAdjacents][Fanout][5]") {
  uint16_t fanout = 5;

  SECTION("Scenario 5.1") {
    minhton::NodeInfo node_a = minhton::NodeInfo(0, 0, fanout, "127.0.0.1", 1234);  // us
    minhton::NodeInfo node_x = minhton::NodeInfo(1, 0, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_b = minhton::NodeInfo(1, 1, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_y = minhton::NodeInfo(1, 2, fanout, "127.0.0.1", 1234);  // entering node
    minhton::NodeInfo node_w = minhton::NodeInfo(1, 3, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_v = minhton::NodeInfo(1, 4, fanout, "127.0.0.1", 1234);

    // build routing information of A
    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);
    routing_info->setAdjacentLeft(node_y);
    routing_info->setAdjacentRight(node_w);
    routing_info->setChild(node_x, 0);
    routing_info->setChild(node_y, 2);
    routing_info->setChild(node_w, 3);
    routing_info->setChild(node_v, 4);

    minhton::NodeInfo adj_left_of_b = join_algo.calcAdjacentLeftOfNewChild(node_b);
    REQUIRE(adj_left_of_b.getLogicalNodeInfo() == node_x.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_b = join_algo.calcAdjacentRightOfNewChild(node_b);
    REQUIRE(adj_right_of_b.getLogicalNodeInfo() == node_y.getLogicalNodeInfo());

    REQUIRE(join_algo.mustSendUpdateRight(adj_left_of_b));
    REQUIRE(join_algo.mustSendUpdateLeft(adj_right_of_b));

    minhton::NodeInfo adj_left_of_a = join_algo.calcOurNewAdjacentLeft(node_b, adj_right_of_b);
    REQUIRE(adj_left_of_a.getLogicalNodeInfo() == node_y.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_a = join_algo.calcOurNewAdjacentRight(node_b, adj_left_of_b);
    REQUIRE(adj_right_of_a.getLogicalNodeInfo() == node_w.getLogicalNodeInfo());
  }

  SECTION("Scenario 5.2") {
    minhton::NodeInfo node_p = minhton::NodeInfo(0, 0, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_c = minhton::NodeInfo(1, 0, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_a = minhton::NodeInfo(1, 1, fanout, "127.0.0.1", 1234);  // us
    minhton::NodeInfo node_d = minhton::NodeInfo(1, 2, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_b = minhton::NodeInfo(2, 5, fanout, "127.0.0.1", 1234);  // entering node
    minhton::NodeInfo node_x1 = minhton::NodeInfo(2, 6, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_x2 = minhton::NodeInfo(2, 7, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_x3 = minhton::NodeInfo(2, 8, fanout, "127.0.0.1", 1234);
    minhton::NodeInfo node_x4 = minhton::NodeInfo(2, 9, fanout, "127.0.0.1", 1234);

    auto routing_info = std::make_shared<RoutingInformation>(node_a, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    routing_info->setParent(node_p);
    routing_info->setAdjacentLeft(node_x2);
    routing_info->setAdjacentRight(node_x3);
    routing_info->setChild(node_x1, 1);
    routing_info->setChild(node_x2, 2);
    routing_info->setChild(node_x3, 3);
    routing_info->setChild(node_x4, 4);

    routing_info->updateRoutingTableNeighbor(node_c);
    routing_info->updateRoutingTableNeighbor(node_d);

    minhton::NodeInfo adj_left_of_b = join_algo.calcAdjacentLeftOfNewChild(node_b);
    REQUIRE(adj_left_of_b.getLogicalNodeInfo() == node_c.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_b = join_algo.calcAdjacentRightOfNewChild(node_b);
    REQUIRE(adj_right_of_b.getLogicalNodeInfo() == node_x1.getLogicalNodeInfo());

    REQUIRE(join_algo.mustSendUpdateRight(adj_left_of_b));
    REQUIRE(join_algo.mustSendUpdateLeft(adj_right_of_b));

    minhton::NodeInfo adj_left_of_a = join_algo.calcOurNewAdjacentLeft(node_b, adj_right_of_b);
    REQUIRE(adj_left_of_a.getLogicalNodeInfo() == node_x2.getLogicalNodeInfo());

    minhton::NodeInfo adj_right_of_a = join_algo.calcOurNewAdjacentRight(node_b, adj_left_of_b);
    REQUIRE(adj_right_of_a.getLogicalNodeInfo() == node_x3.getLogicalNodeInfo());
  }
}

TEST_CASE("JoinAlgorithmGeneral getCloserAdjacent",
          "[JoinAlgorithmGeneral][Methods][getCloserAdjacent]") {
  SECTION("Fanout 3") {
    uint16_t fanout = 3;

    minhton::NodeInfo node_1_1(1, 1, fanout, "1.2.3.4", 2000);

    minhton::NodeInfo node_3_15(3, 15, fanout, "1.2.3.4", 2000);

    minhton::NodeInfo node_4_44(4, 44, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_4_45(4, 45, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_4_46(4, 46, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_4_47(4, 47, fanout, "1.2.3.4", 2000);

    auto routing_info = std::make_shared<RoutingInformation>(node_3_15, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    routing_info->setChild(node_4_46, 1);
    routing_info->setChild(node_4_47, 2);

    auto closer_adj1 = join_algo.getCloserAdjacent(node_4_45, node_4_44);
    REQUIRE(closer_adj1.getLogicalNodeInfo() == node_1_1.getLogicalNodeInfo());

    auto closer_adj2 = join_algo.getCloserAdjacent(node_4_45, node_1_1);
    REQUIRE_FALSE(closer_adj2.isInitialized());

    auto closer_adj3 = join_algo.getCloserAdjacent(node_4_46, node_1_1);
    REQUIRE_FALSE(closer_adj3.isInitialized());
  }

  SECTION("Fanout 4") {
    uint16_t fanout = 4;

    minhton::NodeInfo node_1_2(1, 2, fanout, "1.2.3.4", 2000);

    minhton::NodeInfo node_3_39(3, 39, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_3_40(3, 40, fanout, "1.2.3.4", 2000);

    minhton::NodeInfo node_4_156(4, 156, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_4_157(4, 157, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_4_158(4, 158, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_4_159(4, 159, fanout, "1.2.3.4", 2000);

    auto routing_info = std::make_shared<RoutingInformation>(node_3_39, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    routing_info->setChild(node_4_156, 0);
    routing_info->setChild(node_4_157, 1);
    routing_info->setChild(node_4_158, 2);

    auto closer_adj1 = join_algo.getCloserAdjacent(node_4_158, node_1_2);
    REQUIRE_FALSE(closer_adj1.isInitialized());

    auto closer_adj2 = join_algo.getCloserAdjacent(node_4_159, node_3_40);
    REQUIRE(closer_adj2.getLogicalNodeInfo() == node_1_2.getLogicalNodeInfo());

    routing_info->setChild(node_4_159, 3);
    auto closer_adj3 = join_algo.getCloserAdjacent(node_4_158, node_1_2);
    REQUIRE(closer_adj3.getLogicalNodeInfo() == node_4_159.getLogicalNodeInfo());
  }

  SECTION("Fanout 5") {
    uint16_t fanout = 5;

    minhton::NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);

    minhton::NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);

    minhton::NodeInfo node_3_14(3, 14, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_3_15(3, 15, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_3_16(3, 16, fanout, "1.2.3.4", 2000);
    minhton::NodeInfo node_3_17(3, 17, fanout, "1.2.3.4", 2000);

    auto routing_info = std::make_shared<RoutingInformation>(node_2_3, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    routing_info->setChild(node_3_16, 1);
    routing_info->setChild(node_3_17, 2);

    auto closer_adj1 = join_algo.getCloserAdjacent(node_3_16, node_1_0);
    REQUIRE_FALSE(closer_adj1.isInitialized());

    auto closer_adj2 = join_algo.getCloserAdjacent(node_3_15, node_3_14);
    REQUIRE(closer_adj2.getLogicalNodeInfo() == node_1_0.getLogicalNodeInfo());
  }
}

// this method might change due to the fanout 10 problem
// these are the old tests
//
// TEST_CASE("JoinAlgorithmGeneral calcNewChildPosition Fanout 3",
//           "[JoinAlgorithmGeneral][Method][calcNewChildPosition][Fanout][3]") {
//   uint16_t fanout = 3;

//   SECTION("Middle of Level 1, Fanout 3") {
//     minhton::NodeInfo node = minhton::NodeInfo(1, 1, fanout);
//     minhton::RoutingInformation routing_info = minhton::RoutingInformation(node);

//     // node (1:1) has no children, so new_child_1 should be (2:5), because its closest to (0:0)
//     minhton::NodeInfo new_child_1 = routing_info.calcNewChildPosition();
//     new_child_1.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_1.getLogicalNodeInfo().getLevel() == 2);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getNumber() == 5);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_1.getLogicalNodeInfo().isInitialized());

//     // adding child (2:5) to (1:1) -> (2:4) is next closest to (0:0)
//     routing_info.setChild(new_child_1, 2);
//     minhton::NodeInfo new_child_2 = routing_info.calcNewChildPosition();
//     new_child_2.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_2.getLogicalNodeInfo().getLevel() == 2);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getNumber() == 4);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_2.getLogicalNodeInfo().isInitialized());

//     // adding child (2:4) to (1:1) -> (2:3) is last free position
//     routing_info.setChild(new_child_2, 1);
//     minhton::NodeInfo new_child_3 = routing_info.calcNewChildPosition();
//     new_child_3.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_3.getLogicalNodeInfo().getLevel() == 2);
//     REQUIRE(new_child_3.getLogicalNodeInfo().getNumber() == 3);
//     REQUIRE(new_child_3.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_3.getLogicalNodeInfo().isInitialized());

//     // adding child (2:3) -> no free position left
//     routing_info.setChild(new_child_3, 0);
//     minhton::NodeInfo new_child_4 = routing_info.calcNewChildPosition();
//     REQUIRE(new_child_4.getLogicalNodeInfo().isInitialized() == false);
//     REQUIRE(new_child_4.getPhysicalNodeInfo().isInitialized() == false);
//   }

//   SECTION("Right Edge of Level 2, Fanout 3") {
//     minhton::NodeInfo node = minhton::NodeInfo(2, 8, fanout);
//     minhton::RoutingInformation routing_info = minhton::RoutingInformation(node);

//     // node (2:8) has no children, so new_child_1 should be (3:24), because its closest to
//     (0:0) minhton::NodeInfo new_child_1 = routing_info.calcNewChildPosition();
//     new_child_1.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_1.getLogicalNodeInfo().getLevel() == 3);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getNumber() == 24);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_1.getLogicalNodeInfo().isInitialized());

//     // adding child (3:24) to (2:8) -> (3:25) is next closest to (0:0)
//     routing_info.setChild(new_child_1, 0);
//     minhton::NodeInfo new_child_2 = routing_info.calcNewChildPosition();
//     new_child_2.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_2.getLogicalNodeInfo().getLevel() == 3);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getNumber() == 25);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_2.getLogicalNodeInfo().isInitialized());

//     // adding child (3:25) to (2:8) -> (3:26) is last free position
//     routing_info.setChild(new_child_2, 1);
//     minhton::NodeInfo new_child_3 = routing_info.calcNewChildPosition();
//     new_child_3.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_3.getLogicalNodeInfo().getLevel() == 3);
//     REQUIRE(new_child_3.getLogicalNodeInfo().getNumber() == 26);
//     REQUIRE(new_child_3.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_3.getLogicalNodeInfo().isInitialized());

//     // adding child (2:3) -> no free position left
//     routing_info.setChild(new_child_3, 2);
//     minhton::NodeInfo new_child_4 = routing_info.calcNewChildPosition();
//     REQUIRE(new_child_4.getLogicalNodeInfo().isInitialized() == false);
//     REQUIRE(new_child_4.getPhysicalNodeInfo().isInitialized() == false);
//   }

//   SECTION("Left Edge of Level 4, Fanout 3") {
//     minhton::NodeInfo node = minhton::NodeInfo(4, 0, fanout);
//     minhton::RoutingInformation routing_info = minhton::RoutingInformation(node);

//     // node (4:0) has no children, so new_child_1 should be (5:2), because its closest to (0:0)
//     minhton::NodeInfo new_child_1 = routing_info.calcNewChildPosition();
//     new_child_1.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_1.getLogicalNodeInfo().getLevel() == 5);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getNumber() == 2);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_1.getLogicalNodeInfo().isInitialized());

//     // adding child (5:2) to (4:0) -> (5:1) is next closest to (0:0)
//     routing_info.setChild(new_child_1, 2);
//     minhton::NodeInfo new_child_2 = routing_info.calcNewChildPosition();
//     new_child_2.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_2.getLogicalNodeInfo().getLevel() == 5);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getNumber() == 1);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_2.getLogicalNodeInfo().isInitialized());

//     // adding child (5:1) to (4:0) -> (5:0) is last free position
//     routing_info.setChild(new_child_2, 1);
//     minhton::NodeInfo new_child_3 = routing_info.calcNewChildPosition();
//     new_child_3.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_3.getLogicalNodeInfo().getLevel() == 5);
//     REQUIRE(new_child_3.getLogicalNodeInfo().getNumber() == 0);
//     REQUIRE(new_child_3.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_3.getLogicalNodeInfo().isInitialized());

//     // adding child (4:0) -> no free position left
//     routing_info.setChild(new_child_3, 0);
//     minhton::NodeInfo new_child_4 = routing_info.calcNewChildPosition();
//     REQUIRE(new_child_4.getLogicalNodeInfo().isInitialized() == false);
//     REQUIRE(new_child_4.getPhysicalNodeInfo().isInitialized() == false);
//   }
// }

// TEST_CASE("JoinAlgorithmGeneral calcNewChildPosition Fanout 2",
//           "[JoinAlgorithmGeneral][Method][calcNewChildPosition][Fanout][2]") {
//   uint16_t fanout = 2;

//   SECTION("Middle of Level 2, Fanout 2") {
//     minhton::NodeInfo node = minhton::NodeInfo(2, 1, fanout);
//     minhton::RoutingInformation routing_info = minhton::RoutingInformation(node);

//     // node (2:1) has no children, so new_child_1 should be (3:3), because its closest to (0:0)
//     minhton::NodeInfo new_child_1 = routing_info.calcNewChildPosition();
//     new_child_1.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_1.isInitialized());
//     REQUIRE(new_child_1.getLogicalNodeInfo().getLevel() == 3);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getNumber() == 3);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getFanout() == fanout);

//     routing_info.setChild(new_child_1, new_child_1.getNumber() % fanout);
//     REQUIRE(routing_info.getChildren()[0].isInitialized() == false);
//     REQUIRE(routing_info.getChildren()[1].isInitialized());

//     // adding child (3:3) to (2:1) -> (3:2) is only free position
//     minhton::NodeInfo new_child_2 = routing_info.calcNewChildPosition();
//     new_child_2.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_2.isInitialized());
//     REQUIRE(new_child_2.getLogicalNodeInfo().getLevel() == 3);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getNumber() == 2);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getFanout() == fanout);

//     routing_info.setChild(new_child_2, new_child_2.getNumber() % fanout);
//     REQUIRE(routing_info.getChildren()[0].isInitialized());
//     REQUIRE(routing_info.getChildren()[1].isInitialized());

//     // adding child (3:2) -> no free position left
//     minhton::NodeInfo new_child_3 = routing_info.calcNewChildPosition();
//     REQUIRE(new_child_3.getLogicalNodeInfo().isInitialized() == false);
//     REQUIRE(new_child_3.getPhysicalNodeInfo().isInitialized() == false);
//   }

//   SECTION("Left Edge of Level 3, Fanout 2") {
//     minhton::NodeInfo node = minhton::NodeInfo(3, 0, fanout);
//     minhton::RoutingInformation routing_info = minhton::RoutingInformation(node);

//     // node (2:1) has no children, so new_child_1_1 should be (3:3), because its closest to
//     (0:0) minhton::NodeInfo new_child_1 = routing_info.calcNewChildPosition();
//     new_child_1.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_1.getLogicalNodeInfo().getLevel() == 4);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getNumber() == 1);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_1.getLogicalNodeInfo().isInitialized());

//     // adding child (4:1) to (3:0) -> (4:1) is only free position
//     routing_info.setChild(new_child_1, 1);
//     minhton::NodeInfo new_child_2 = routing_info.calcNewChildPosition();
//     new_child_2.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_2.getLogicalNodeInfo().getLevel() == 4);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getNumber() == 0);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_2.getLogicalNodeInfo().isInitialized());

//     // no free position left
//     routing_info.setChild(new_child_2, 0);
//     minhton::NodeInfo new_child_3 = routing_info.calcNewChildPosition();
//     REQUIRE(new_child_3.getLogicalNodeInfo().isInitialized() == false);
//     REQUIRE(new_child_3.getPhysicalNodeInfo().isInitialized() == false);
//   }

//   SECTION("Right Edge of Level 4, Fanout 2") {
//     minhton::NodeInfo node = minhton::NodeInfo(4, 15, fanout);
//     minhton::RoutingInformation routing_info = minhton::RoutingInformation(node);

//     // node (4:15) has no children, so new_child_1 should be (5:30), because its closest to
//     (0:0) minhton::NodeInfo new_child_1 = routing_info.calcNewChildPosition();
//     new_child_1.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_1.getLogicalNodeInfo().getLevel() == 5);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getNumber() == 30);
//     REQUIRE(new_child_1.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_1.getLogicalNodeInfo().isInitialized());

//     // adding child (5:30) to (4:15) -> (5:31) is only free position
//     routing_info.setChild(new_child_1, 0);
//     minhton::NodeInfo new_child_2 = routing_info.calcNewChildPosition();
//     new_child_2.setPhysicalNodeInfo(minhton::PhysicalNodeInfo("127.0.0.1", 1234));
//     REQUIRE(new_child_2.getLogicalNodeInfo().getLevel() == 5);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getNumber() == 31);
//     REQUIRE(new_child_2.getLogicalNodeInfo().getFanout() == fanout);
//     REQUIRE(new_child_2.getLogicalNodeInfo().isInitialized());

//     // no free position left
//     routing_info.setChild(new_child_2, 1);
//     minhton::NodeInfo new_child_3 = routing_info.calcNewChildPosition();
//     REQUIRE(new_child_3.getLogicalNodeInfo().isInitialized() == false);
//     REQUIRE(new_child_3.getPhysicalNodeInfo().isInitialized() == false);
//   }
// }

TEST_CASE("JoinAlgorithmGeneral getRoutingTableNeighborsForNewChild",
          "[JoinAlgorithmGeneral][Methods][getRoutingTableNeighborsForNewChild]") {
  SECTION("Fanout 2") {
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

    auto routing_info = std::make_shared<RoutingInformation>(node_2_1, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;
    MinhtonJoinAlgorithmForTest join_algo(access);

    routing_info->updateNeighbor(node_2_0);
    routing_info->updateNeighbor(node_2_2);
    routing_info->updateNeighbor(node_2_3);
    routing_info->updateNeighbor(node_3_0);
    routing_info->updateNeighbor(node_3_1);
    routing_info->updateNeighbor(node_3_2);
    routing_info->updateNeighbor(node_3_3);
    routing_info->updateNeighbor(node_3_4);
    routing_info->updateNeighbor(node_3_5);
    routing_info->updateNeighbor(node_3_6);
    routing_info->updateNeighbor(node_3_7);

    // entering node is 3:3
    auto neighbors = join_algo.getRoutingTableNeighborsForNewChild(node_3_3);

    REQUIRE(neighbors.size() == 5);

    bool inside = false;
    for (auto const &n : neighbors) {
      if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 1, fanout)) {
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
      if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 4, fanout)) {
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
      if (n.getLogicalNodeInfo() == minhton::LogicalNodeInfo(3, 7, fanout)) {
        inside = true;
        break;
      }
    }
    REQUIRE(inside);
  }
}

TEST_CASE("JoinAlgorithmGeneral initiateJoin", "[JoinAlgorithmGeneral][initiateJoin]") {
  NodeInfo node;
  node.setPhysicalNodeInfo(PhysicalNodeInfo("123.0.0.42", 2123));

  auto routing_info = std::make_shared<RoutingInformation>(node, Logger());
  auto access = std::make_shared<AccessContainer>();
  access->routing_info = routing_info;

  NodeInfo sent_msg_sender;
  NodeInfo sent_msg_target;
  NodeInfo sent_msg_entering_node;
  std::function<uint32_t(const MessageVariant &)> save_target =
      [&](const MessageVariant &msg) mutable {
        auto sent_msg = std::get_if<MessageJoin>(&msg);

        sent_msg_sender = sent_msg->getSender();
        sent_msg_target = sent_msg->getTarget();
        sent_msg_entering_node = sent_msg->getEnteringNode();

        return 1;
      };
  access->send = save_target;

  // to see what timeout was set
  TimeoutType set_timeout_type;
  std::function<void(TimeoutType)> set_timeout = [&](TimeoutType type) mutable {
    set_timeout_type = type;
    return 1;
  };
  access->set_timeout = set_timeout;

  MinhtonJoinAlgorithmForTest join_algo(access);

  PhysicalNodeInfo target_p_node_info = PhysicalNodeInfo("1.2.3.4", 2020);
  join_algo.initiateJoin(target_p_node_info);

  REQUIRE(sent_msg_sender == node);
  REQUIRE(sent_msg_target.getPhysicalNodeInfo() == target_p_node_info);
  REQUIRE(sent_msg_entering_node.getPhysicalNodeInfo() == node.getPhysicalNodeInfo());

  // join accept response timeout must have been set
  REQUIRE(set_timeout_type == TimeoutType::kJoinAcceptResponseTimeout);
}

TEST_CASE("JoinAlgorithmGeneral processJoinAccept", "[JoinAlgorithmGeneral][processJoinAccept]") {
  SECTION("Fanout 2") {
    NodeInfo node;
    node.setPhysicalNodeInfo(PhysicalNodeInfo("123.0.0.42", 2123));

    auto routing_info = std::make_shared<RoutingInformation>(node, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;

    // to see what was sent, we can look at target_node
    NodeInfo sent_msg_sender;
    NodeInfo sent_msg_target;
    uint64_t ref_event_id;
    std::function<uint32_t(const MessageVariant &)> save_target =
        [&](const MessageVariant &msg) mutable {
          auto sent_msg = std::get_if<MessageJoinAcceptAck>(&msg);

          sent_msg_sender = sent_msg->getSender();
          sent_msg_target = sent_msg->getTarget();
          ref_event_id = sent_msg->getHeader().getRefEventId();
          return 1;
        };
    access->send = save_target;

    // to see what timeout was set
    TimeoutType cancelled_timeout_type;
    std::function<void(TimeoutType)> cancel_timeout = [&](TimeoutType type) mutable {
      cancelled_timeout_type = type;
      return 1;
    };
    access->cancel_timeout = cancel_timeout;

    MinhtonJoinAlgorithmForTest join_algo(access);
    uint64_t event_id = 42;
    uint16_t fanout = 2;

    NodeInfo parent(1, 0, fanout, "1.2.3.5", 2000);
    NodeInfo new_node(2, 1, fanout);
    new_node.setPhysicalNodeInfo(node.getPhysicalNodeInfo());

    NodeInfo adjacent_right(0, 0, fanout, "1.2.3.6", 2000);
    NodeInfo rt_neighbor_1(2, 0, fanout, "1.2.3.7", 2000);
    NodeInfo rt_neighbor_2(2, 2, fanout, "1.2.3.8", 2000);
    NodeInfo rt_neighbor_3(2, 3, fanout, "1.2.3.9", 2000);

    MinhtonMessageHeader header(parent, new_node, event_id);
    MessageJoinAccept message_join_accept(header, fanout, parent, adjacent_right,
                                          {rt_neighbor_1, rt_neighbor_3, rt_neighbor_2});

    // must call processJoinAccept of JoinAlgorithmGeneral
    join_algo.process(message_join_accept);

    // timeout must have been cancelled
    REQUIRE(cancelled_timeout_type == TimeoutType::kJoinAcceptResponseTimeout);

    // checking the send join accept ack message
    REQUIRE(ref_event_id == event_id);
    REQUIRE(sent_msg_sender == new_node);
    REQUIRE(sent_msg_target == parent);

    // checking initialized routing info of new node 2:1
    REQUIRE(routing_info->getSelfNodeInfo() == new_node);
    REQUIRE(routing_info->getParent() == parent);
    REQUIRE(routing_info->getAdjacentLeft() == parent);
    REQUIRE(routing_info->getAdjacentRight() == adjacent_right);

    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors().size() == 3);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[0] == rt_neighbor_1);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[1] == rt_neighbor_2);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[2] == rt_neighbor_3);

    // no routing table neighbor children and no children
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighborChildren().size() == 0);
    REQUIRE(routing_info->getInitializedChildren().size() == 0);
  }

  SECTION("Fanout 5") {
    NodeInfo node;
    node.setPhysicalNodeInfo(PhysicalNodeInfo("12.3.0.42", 2123));

    auto routing_info = std::make_shared<RoutingInformation>(node, Logger());
    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;

    NodeInfo sent_msg_sender;
    NodeInfo sent_msg_target;
    uint64_t ref_event_id;
    std::function<uint32_t(const MessageVariant &)> save_target =
        [&](const MessageVariant &msg) mutable {
          auto sent_msg = std::get_if<MessageJoinAcceptAck>(&msg);

          sent_msg_sender = sent_msg->getSender();
          sent_msg_target = sent_msg->getTarget();
          ref_event_id = sent_msg->getHeader().getRefEventId();
          return 1;
        };
    access->send = save_target;

    // to see what timeout was set
    TimeoutType cancelled_timeout_type;
    std::function<void(TimeoutType)> cancel_timeout = [&](TimeoutType type) mutable {
      cancelled_timeout_type = type;
      return 1;
    };
    access->cancel_timeout = cancel_timeout;

    MinhtonJoinAlgorithmForTest join_algo(access);
    uint16_t fanout = 5;
    uint64_t event_id = 43;

    NodeInfo parent(1, 3, fanout, "1.2.3.5", 2000);
    NodeInfo new_node(2, 16, fanout);
    new_node.setPhysicalNodeInfo(node.getPhysicalNodeInfo());

    NodeInfo adjacent_left(2, 15, fanout, "1.2.3.6", 2000);
    NodeInfo adjacent_right(2, 17, fanout, "1.2.3.6", 2000);

    NodeInfo rt_neighbor_1(2, 6, fanout, "1.2.3.7", 2000);
    NodeInfo rt_neighbor_2(2, 11, fanout, "1.2.3.8", 2000);
    NodeInfo rt_neighbor_3(2, 12, fanout, "1.2.3.9", 2000);
    NodeInfo rt_neighbor_4(2, 13, fanout, "1.2.3.10", 2000);
    NodeInfo rt_neighbor_5(2, 14, fanout, "1.2.3.11", 2000);

    MinhtonMessageHeader header(parent, new_node, event_id);
    MessageJoinAccept message_join_accept(
        header, fanout, adjacent_left, adjacent_right,
        {adjacent_left, adjacent_right, rt_neighbor_4, rt_neighbor_5, rt_neighbor_1, rt_neighbor_3,
         rt_neighbor_2});

    // must call processJoinAccept of JoinAlgorithmGeneral
    join_algo.process(message_join_accept);

    // timeout must have been cancelled
    REQUIRE(cancelled_timeout_type == TimeoutType::kJoinAcceptResponseTimeout);

    // checking the send join accept ack message
    REQUIRE(ref_event_id == event_id);
    REQUIRE(sent_msg_sender == new_node);
    REQUIRE(sent_msg_target == parent);

    // checking initialized routing info of new node 2:1
    REQUIRE(routing_info->getSelfNodeInfo() == new_node);
    REQUIRE(routing_info->getParent() == parent);
    REQUIRE(routing_info->getAdjacentLeft() == adjacent_left);
    REQUIRE(routing_info->getAdjacentRight() == adjacent_right);

    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors().size() == 7);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[0] == rt_neighbor_1);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[1] == rt_neighbor_2);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[2] == rt_neighbor_3);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[3] == rt_neighbor_4);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[4] == rt_neighbor_5);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[5] == adjacent_left);
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighbors()[6] == adjacent_right);

    // no routing table neighbor children and no children
    REQUIRE(routing_info->getAllInitializedRoutingTableNeighborChildren().size() == 0);
    REQUIRE(routing_info->getInitializedChildren().size() == 0);
  }
}

TEST_CASE("JoinAlgorithmGeneral processJoinAcceptAck",
          "[JoinAlgorithmGeneral][processJoinAcceptAck]") {
  SECTION("Procedure Key not set") {
    uint16_t fanout = 2;
    uint64_t event_id = 1234;

    NodeInfo node(2, 1, fanout, "12.3.4.5", 2134);
    NodeInfo parent(1, 0, fanout, "1.2.3.5", 2000);

    auto access = std::make_shared<AccessContainer>();
    auto routing_info = std::make_shared<RoutingInformation>(node, Logger());
    auto procedure_info = std::make_shared<ProcedureInfo>();
    access->routing_info = routing_info;
    access->procedure_info = procedure_info;

    MinhtonJoinAlgorithmForTest join_algo(access);

    MinhtonMessageHeader header(node, parent, event_id);
    MessageJoinAcceptAck message_join_accept_ack(header);

    // must call processJoinAcceptAck of JoinAlgorithmGeneral
    REQUIRE_THROWS_AS(join_algo.process(message_join_accept_ack), AlgorithmException);
  }

  SECTION("Fanout 2") {
    uint16_t fanout = 2;

    NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);
    NodeInfo node_1_0(1, 0, fanout, "1.2.3.5", 2000);
    NodeInfo node_1_1(1, 1, fanout, "1.2.3.6", 2000);
    NodeInfo node_2_0(2, 0, fanout, "1.2.3.7", 2000);
    NodeInfo node_2_1(2, 1, fanout, "1.2.3.8", 2000);
    NodeInfo node_2_2(2, 2, fanout, "1.2.3.9", 2000);
    NodeInfo node_2_3(2, 3, fanout, "1.2.3.10", 2000);

    auto access = std::make_shared<AccessContainer>();
    auto routing_info = std::make_shared<RoutingInformation>(node_1_0, Logger());
    auto procedure_info = std::make_shared<ProcedureInfo>();
    access->routing_info = routing_info;
    access->procedure_info = procedure_info;

    // to see what was sent, we can look at target_node
    std::vector<std::vector<NodeInfo>> update_neighbor_msg_contents;
    std::function<uint32_t(const MessageVariant &)> save_target =
        [&](const MessageVariant &msg) mutable {
          try {
            auto sent_msg = std::get_if<MessageUpdateNeighbors>(&msg);

            std::vector<NodeInfo> current = {sent_msg->getSender(), sent_msg->getTarget()};
            for (auto const &node_and_rel : sent_msg->getNeighborsToUpdate()) {
              current.push_back(std::get<0>(node_and_rel));
            }

            update_neighbor_msg_contents.push_back(current);

          } catch (const std::bad_cast &e) {
          };

          return 1;
        };
    access->send = save_target;

    // to see what timeout was set
    TimeoutType cancelled_timeout_type;
    std::function<void(TimeoutType)> cancel_timeout = [&](TimeoutType type) mutable {
      cancelled_timeout_type = type;
      return 1;
    };
    access->cancel_timeout = cancel_timeout;

    MinhtonJoinAlgorithmForTest join_algo(access);
    uint64_t event_id = 1234;

    // we are 1:0
    routing_info->setParent(node_0_0);
    routing_info->setAdjacentLeft(node_2_0);
    routing_info->updateNeighbor(node_2_0);
    routing_info->setAdjacentRight(node_0_0);
    routing_info->updateRoutingTableNeighbor(node_1_1);
    routing_info->updateRoutingTableNeighborChild(node_2_2);
    routing_info->updateRoutingTableNeighborChild(node_2_3);

    MinhtonMessageHeader header(node_2_1, node_1_0);
    MessageJoinAcceptAck message_join_accept_ack(header);

    auto state_nodes = {node_2_1, node_1_0, node_0_0};
    access->procedure_info->save(ProcedureKey::kAcceptChildProcedure, state_nodes);
    REQUIRE(access->procedure_info->hasKey(ProcedureKey::kAcceptChildProcedure));
    REQUIRE_NOTHROW(access->procedure_info->load(ProcedureKey::kAcceptChildProcedure));

    REQUIRE_NOTHROW(access->procedure_info->saveEventId(ProcedureKey::kJoinProcedure, event_id));
    REQUIRE(access->procedure_info->hasKey(ProcedureKey::kJoinProcedure));
    REQUIRE_NOTHROW(access->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));

    REQUIRE(routing_info->getChild(0) == node_2_0);
    REQUIRE(!routing_info->getChild(1).isInitialized());
    REQUIRE(routing_info->getAdjacentLeft() == node_2_0);
    REQUIRE(routing_info->getAdjacentRight() == node_0_0);

    access->wait_for_acks = [](uint32_t /*number*/, std::function<void()> cb) { cb(); };
    // must call processJoinAcceptAck of JoinAlgorithmGeneral
    REQUIRE_NOTHROW(join_algo.process(message_join_accept_ack));

    REQUIRE(cancelled_timeout_type == TimeoutType::kJoinAcceptAckResponseTimeout);

    // updated our childs
    REQUIRE(routing_info->getChild(1).isInitialized());
    REQUIRE(routing_info->getChild(1) == node_2_1);

    // updated our adjacents
    REQUIRE(routing_info->getAdjacentRight().isInitialized());
    REQUIRE(routing_info->getAdjacentRight() == node_2_1);

    // updated procedure infos
    REQUIRE(!access->procedure_info->hasKey(ProcedureKey::kAcceptChildProcedure));
    REQUIRE(!access->procedure_info->hasKey(ProcedureKey::kJoinProcedure));

    // send update adj right
    auto adj_left_update =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_1_0.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_0_0.getPhysicalNodeInfo();
                     });
    REQUIRE(adj_left_update != update_neighbor_msg_contents.end());

    // not send update left, because we are the left
    auto adj_right_update =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_1_0.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_1_0.getPhysicalNodeInfo();
                     });
    REQUIRE(adj_right_update == update_neighbor_msg_contents.end());

    // sent other update neighbor messages
    auto rt_update =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_1_0.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_1_1.getPhysicalNodeInfo();
                     });
    REQUIRE(rt_update != update_neighbor_msg_contents.end());
  }

  SECTION("Fanout 5") {
    uint16_t fanout = 5;

    NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);
    NodeInfo node_1_0(1, 0, fanout, "1.2.3.5", 2000);
    NodeInfo node_1_1(1, 1, fanout, "1.2.3.6", 2000);
    NodeInfo node_1_2(1, 2, fanout, "1.2.3.7", 2000);
    NodeInfo node_1_3(1, 3, fanout, "1.2.3.8", 2000);
    NodeInfo node_1_4(1, 4, fanout, "1.2.3.9", 2000);
    NodeInfo node_2_14(2, 14, fanout, "1.2.3.10", 2000);
    NodeInfo node_2_15(2, 15, fanout, "1.2.3.11", 2000);
    NodeInfo node_2_16(2, 16, fanout, "1.2.3.12", 2000);
    NodeInfo node_2_17(2, 17, fanout, "1.2.3.13", 2000);
    NodeInfo node_2_18(2, 18, fanout, "1.2.3.14", 2000);

    auto access = std::make_shared<AccessContainer>();
    auto routing_info = std::make_shared<RoutingInformation>(node_1_3, Logger());
    auto procedure_info = std::make_shared<ProcedureInfo>();
    access->routing_info = routing_info;
    access->procedure_info = procedure_info;

    std::vector<std::vector<NodeInfo>> update_neighbor_msg_contents;
    std::function<uint32_t(const MessageVariant &)> save_target =
        [&](const MessageVariant &msg) mutable {
          try {
            auto sent_msg = std::get_if<MessageUpdateNeighbors>(&msg);

            std::vector<NodeInfo> current = {sent_msg->getSender(), sent_msg->getTarget()};
            for (auto const &node_and_rel : sent_msg->getNeighborsToUpdate()) {
              current.push_back(std::get<0>(node_and_rel));
            }

            update_neighbor_msg_contents.push_back(current);

          } catch (const std::bad_cast &e) {
          };

          return 1;
        };
    access->send = save_target;

    // to see what timeout was set
    TimeoutType cancelled_timeout_type;
    std::function<void(TimeoutType)> cancel_timeout = [&](TimeoutType type) mutable {
      cancelled_timeout_type = type;
      return 1;
    };
    access->cancel_timeout = cancel_timeout;

    MinhtonJoinAlgorithmForTest join_algo(access);
    uint64_t event_id = 1235;

    // we are 1:0
    routing_info->setParent(node_0_0);
    routing_info->setAdjacentLeft(node_2_17);
    routing_info->setAdjacentRight(node_2_18);
    routing_info->updateNeighbor(node_1_0);
    routing_info->updateNeighbor(node_1_1);
    routing_info->updateNeighbor(node_1_2);
    routing_info->updateNeighbor(node_1_4);
    routing_info->updateNeighbor(node_2_14);
    routing_info->updateNeighbor(node_2_15);
    routing_info->updateNeighbor(node_2_17);
    routing_info->updateNeighbor(node_2_18);

    MinhtonMessageHeader header(node_2_16, node_1_3);
    MessageJoinAcceptAck message_join_accept_ack(header);

    auto state_nodes = {node_2_16, node_2_15, node_2_17};
    access->procedure_info->save(ProcedureKey::kAcceptChildProcedure, state_nodes);
    REQUIRE(access->procedure_info->hasKey(ProcedureKey::kAcceptChildProcedure));
    REQUIRE_NOTHROW(access->procedure_info->load(ProcedureKey::kAcceptChildProcedure));

    REQUIRE_NOTHROW(access->procedure_info->saveEventId(ProcedureKey::kJoinProcedure, event_id));
    REQUIRE(access->procedure_info->hasKey(ProcedureKey::kJoinProcedure));
    REQUIRE_NOTHROW(access->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));

    REQUIRE(routing_info->getChild(0) == node_2_15);
    REQUIRE(routing_info->getChild(2) == node_2_17);
    REQUIRE(!routing_info->getChild(1).isInitialized());
    REQUIRE(routing_info->getAdjacentLeft() == node_2_17);
    REQUIRE(routing_info->getAdjacentRight() == node_2_18);

    access->wait_for_acks = [](uint32_t /*number*/, std::function<void()> cb) { cb(); };
    // must call processJoinAcceptAck of JoinAlgorithmGeneral
    REQUIRE_NOTHROW(join_algo.process(message_join_accept_ack));

    REQUIRE(cancelled_timeout_type == TimeoutType::kJoinAcceptAckResponseTimeout);

    // updated our childs
    REQUIRE(routing_info->getChild(1).isInitialized());
    REQUIRE(routing_info->getChild(1) == node_2_16);

    REQUIRE(routing_info->getChild(0) == node_2_15);
    REQUIRE(routing_info->getChild(2) == node_2_17);

    // our adjacents unchanged
    REQUIRE(routing_info->getAdjacentLeft() == node_2_17);
    REQUIRE(routing_info->getAdjacentRight() == node_2_18);

    // updated procedure infos
    REQUIRE(!access->procedure_info->hasKey(ProcedureKey::kAcceptChildProcedure));
    REQUIRE(!access->procedure_info->hasKey(ProcedureKey::kJoinProcedure));

    // send update adj right
    auto adj_left_update =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_1_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_2_15.getPhysicalNodeInfo();
                     });
    REQUIRE(adj_left_update != update_neighbor_msg_contents.end());

    auto adj_right_update =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_1_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_2_17.getPhysicalNodeInfo();
                     });
    REQUIRE(adj_right_update != update_neighbor_msg_contents.end());

    // sent other update neighbor messages
    auto rt_update =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_1_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_1_1.getPhysicalNodeInfo();
                     });
    REQUIRE(rt_update != update_neighbor_msg_contents.end());
  }
}

TEST_CASE("MinhtonJoinAlgorithm performSendUpdateNeighbor",
          "[MinhtonJoinAlgorithm][performSendUpdateNeighbor]") {
  SECTION("Fanout 2") {
    uint16_t fanout = 2;

    NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);
    NodeInfo node_1_0(1, 0, fanout, "1.2.3.5", 2000);
    NodeInfo node_1_1(1, 1, fanout, "1.2.3.6", 2000);

    NodeInfo node_2_0(2, 0, fanout, "1.2.3.7", 2000);
    NodeInfo node_2_1(2, 1, fanout, "1.2.3.8", 2000);
    NodeInfo node_2_2(2, 2, fanout, "1.2.3.9", 2000);
    NodeInfo node_2_3(2, 3, fanout, "1.2.3.10", 2000);

    NodeInfo node_3_2(3, 2, fanout);  // does not exist -> not sending update message to
    NodeInfo node_3_4(3, 4, fanout, "1.3.3.10", 2000);
    NodeInfo node_3_5(3, 5, fanout, "1.4.3.10", 2000);
    NodeInfo node_3_6(3, 6, fanout, "1.4.5.10", 2000);
    NodeInfo node_3_7(3, 7, fanout, "1.5.3.10", 2000);

    auto access = std::make_shared<AccessContainer>();
    auto routing_info = std::make_shared<RoutingInformation>(node_2_3, Logger());
    auto procedure_info = std::make_shared<ProcedureInfo>();
    access->routing_info = routing_info;
    access->procedure_info = procedure_info;

    // to see what was sent, we can look at target_node
    std::vector<std::vector<NodeInfo>> update_neighbor_msg_contents;
    std::function<uint32_t(const MessageVariant &)> save_target =
        [&](const MessageVariant &msg) mutable {
          try {
            auto sent_msg = std::get_if<MessageUpdateNeighbors>(&msg);

            std::vector<NodeInfo> current = {sent_msg->getSender(), sent_msg->getTarget()};
            for (auto const &node_and_rel : sent_msg->getNeighborsToUpdate()) {
              current.push_back(std::get<0>(node_and_rel));
            }

            update_neighbor_msg_contents.push_back(current);

          } catch (const std::bad_cast &e) {
          };

          return 1;
        };
    access->send = save_target;

    MinhtonJoinAlgorithmForTest join_algo(access);
    uint64_t event_id = 1234;

    // we are 1:0
    routing_info->setParent(node_1_1);
    routing_info->setAdjacentLeft(node_3_6);
    routing_info->setAdjacentRight(node_3_7);
    routing_info->updateNeighbor(node_3_6);
    routing_info->updateNeighbor(node_3_7);
    routing_info->updateRoutingTableNeighbor(node_2_1);
    routing_info->updateRoutingTableNeighbor(node_2_2);
    routing_info->updateRoutingTableNeighborChild(node_3_4);
    routing_info->updateRoutingTableNeighborChild(node_3_5);

    REQUIRE_NOTHROW(access->procedure_info->saveEventId(ProcedureKey::kJoinProcedure, event_id));
    REQUIRE(access->procedure_info->hasKey(ProcedureKey::kJoinProcedure));
    REQUIRE_NOTHROW(access->procedure_info->loadEventId(ProcedureKey::kJoinProcedure));

    REQUIRE_NOTHROW(join_algo.performSendUpdateNeighborMessagesAboutEnteringNode(node_3_6));

    auto update_2_1 =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_2_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_2_1.getPhysicalNodeInfo();
                     });
    REQUIRE(update_2_1 != update_neighbor_msg_contents.end());

    auto update_2_2 =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_2_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_2_1.getPhysicalNodeInfo();
                     });
    REQUIRE(update_2_2 != update_neighbor_msg_contents.end());

    auto update_3_4 =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_2_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_3_4.getPhysicalNodeInfo();
                     });
    REQUIRE(update_3_4 != update_neighbor_msg_contents.end());

    auto update_3_5 =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_2_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_3_5.getPhysicalNodeInfo();
                     });
    REQUIRE(update_3_5 != update_neighbor_msg_contents.end());

    auto update_3_7 =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_2_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_3_7.getPhysicalNodeInfo();
                     });
    REQUIRE(update_3_7 != update_neighbor_msg_contents.end());

    auto not_update_3_2 =
        std::find_if(begin(update_neighbor_msg_contents), end(update_neighbor_msg_contents),
                     [&](std::vector<NodeInfo> nodes) {
                       return nodes[0].getPhysicalNodeInfo() == node_2_3.getPhysicalNodeInfo() &&
                              nodes[1].getPhysicalNodeInfo() == node_3_2.getPhysicalNodeInfo();
                     });
    REQUIRE(not_update_3_2 == update_neighbor_msg_contents.end());
  }
}
