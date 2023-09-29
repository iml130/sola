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

    auto adj_left = MinhtonLeaveAlgorithmForTest::getAdjacentLeftFromVector(node_3_0, neighbors);
    auto adj_right = MinhtonLeaveAlgorithmForTest::getAdjacentRightFromVector(node_3_0, neighbors);

    REQUIRE_FALSE(adj_left.isInitialized());
    REQUIRE(adj_right.isInitialized());
    REQUIRE(adj_right.getLogicalNodeInfo() == node_2_0.getLogicalNodeInfo());
  }

  SECTION("Case 2") {
    std::vector<minhton::NodeInfo> neighbors = {node_3_6, node_3_5, node_3_3, node_2_3};

    auto adj_left = MinhtonLeaveAlgorithmForTest::getAdjacentLeftFromVector(node_3_7, neighbors);
    auto adj_right = MinhtonLeaveAlgorithmForTest::getAdjacentRightFromVector(node_3_7, neighbors);

    REQUIRE_FALSE(adj_right.isInitialized());
    REQUIRE(adj_left.isInitialized());
    REQUIRE(adj_left.getLogicalNodeInfo() == node_2_3.getLogicalNodeInfo());
  }

  SECTION("Case 3") {
    std::vector<minhton::NodeInfo> neighbors = {node_3_2, node_0_0, node_1_0,
                                                node_2_0, node_2_2, node_2_3};

    auto adj_left = MinhtonLeaveAlgorithmForTest::getAdjacentLeftFromVector(node_2_1, neighbors);
    auto adj_right = MinhtonLeaveAlgorithmForTest::getAdjacentRightFromVector(node_2_1, neighbors);

    REQUIRE(adj_right.isInitialized());
    REQUIRE(adj_right.getLogicalNodeInfo() == node_0_0.getLogicalNodeInfo());
    REQUIRE(adj_left.isInitialized());
    REQUIRE(adj_left.getLogicalNodeInfo() == node_3_2.getLogicalNodeInfo());
  }
}
