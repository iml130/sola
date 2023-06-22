// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

#include "algorithms/search_exact/minhton_search_exact_algorithm.h"
#include "message/join.h"

using namespace minhton;

TEST_CASE("MinhtonSearchExactAlgorithm", "[MinhtonSearchExactAlgorithm][performSearchExact]") {
  SECTION("Fanout 3") {
    uint16_t fanout = 3;

    NodeInfo node_2_4(2, 4, fanout, "0.0.0.0", 1024);  // this is us

    auto routing_info = std::make_shared<RoutingInformation>(node_2_4, Logger());

    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;

    // to see what was sent, we can look at target_node
    NodeInfo target_node;
    std::function<uint32_t(const MessageVariant &)> save_target =
        [&](const MessageVariant &msg) mutable {
          target_node =
              std::visit([](auto &&message) -> NodeInfo { return message.getTarget(); }, msg);
          return 1;
        };
    access->send = save_target;

    MinhtonSearchExactAlgorithm se_algo(access);

    NodeInfo node_1_1(1, 1, fanout, "1.2.3.4", 2000);
    routing_info->setParent(node_1_1);

    NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_5(2, 5, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_6(2, 6, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_7(2, 7, fanout, "1.2.3.4", 2000);

    NodeInfo node_3_3(3, 3, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_4(3, 4, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_5(3, 5, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_6(3, 6, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_7(3, 7, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_8(3, 8, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_9(3, 9, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_10(3, 10, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_11(3, 11, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_12(3, 12, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_13(3, 13, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_14(3, 14, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_15(3, 15, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_16(3, 16, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_17(3, 17, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_18(3, 18, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_19(3, 19, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_20(3, 20, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_21(3, 21, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_22(3, 22, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_23(3, 23, fanout, "1.2.3.4", 2000);

    routing_info->updateNeighbor(node_2_1);
    routing_info->updateNeighbor(node_2_2);
    routing_info->updateNeighbor(node_2_3);
    routing_info->updateNeighbor(node_2_5);
    routing_info->updateNeighbor(node_2_6);
    routing_info->updateNeighbor(node_2_7);

    routing_info->updateNeighbor(node_3_3);
    routing_info->updateNeighbor(node_3_4);
    routing_info->updateNeighbor(node_3_5);
    routing_info->updateNeighbor(node_3_6);
    routing_info->updateNeighbor(node_3_7);
    routing_info->updateNeighbor(node_3_8);
    routing_info->updateNeighbor(node_3_9);
    routing_info->updateNeighbor(node_3_10);
    routing_info->updateNeighbor(node_3_11);

    routing_info->updateNeighbor(node_3_12);
    routing_info->updateNeighbor(node_3_13);
    routing_info->updateNeighbor(node_3_14);

    routing_info->setAdjacentLeft(node_3_13);
    routing_info->setAdjacentRight(node_3_14);

    routing_info->updateNeighbor(node_3_15);
    routing_info->updateNeighbor(node_3_16);
    routing_info->updateNeighbor(node_3_17);
    routing_info->updateNeighbor(node_3_18);
    routing_info->updateNeighbor(node_3_19);
    routing_info->updateNeighbor(node_3_20);
    routing_info->updateNeighbor(node_3_21);
    routing_info->updateNeighbor(node_3_22);
    routing_info->updateNeighbor(node_3_23);

    MinhtonMessageHeader header;
    header.setSender(node_2_4);
    header.setTarget(node_2_4);
    auto query = std::make_shared<MessageSEVariant>(
        MessageJoin(header, NodeInfo(1, 0, 3, "0.0.0.0", 10000)));

    // 2:4 to 0:0
    se_algo.performSearchExact(NodeInfo(0, 0, fanout), query);
    REQUIRE(((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 17, fanout)) ||
             (target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 18, fanout))));

    // 2:4 to 1:0
    se_algo.performSearchExact(NodeInfo(1, 0, fanout), query);
    REQUIRE(((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 5, fanout)) ||
             (target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 6, fanout))));

    // 2:4 to 1:1
    se_algo.performSearchExact(NodeInfo(1, 1, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(1, 1, fanout)));

    // 2:4 to 3:23
    se_algo.performSearchExact(NodeInfo(3, 23, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 23, fanout)));

    // 2:4 to 2:6
    se_algo.performSearchExact(NodeInfo(2, 6, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 6, fanout)));  // TODO

    // 2:4 to 2:0
    // TODO here it would make much more sense to send to 2:1,
    // because its a direct neighbor to 2:0
    // 3:3 is not a direct neighbor, but closer horizontally
    se_algo.performSearchExact(NodeInfo(2, 0, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 3, fanout)));
  }

  SECTION("Fanout 4") {
    uint16_t fanout = 4;

    NodeInfo node_1_1(1, 1, fanout, "0.0.0.0", 1024);  // this is us

    auto routing_info = std::make_shared<RoutingInformation>(node_1_1, Logger());

    auto access = std::make_shared<AccessContainer>();
    access->routing_info = routing_info;

    // to see what was sent, we can look at target_node
    NodeInfo target_node(5, 15, fanout);
    std::function<uint32_t(const MessageVariant &)> save_target =
        [&](const MessageVariant &msg) mutable {
          target_node =
              std::visit([](auto &&message) -> NodeInfo { return message.getTarget(); }, msg);
          return 1;
        };
    access->send = save_target;

    MinhtonSearchExactAlgorithm se_algo(access);

    NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);
    NodeInfo node_1_0(1, 0, fanout, "1.2.3.4", 2000);
    NodeInfo node_1_2(1, 2, fanout, "1.2.3.4", 2000);
    NodeInfo node_1_3(1, 3, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_1(2, 1, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_3(2, 3, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_4(2, 4, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_5(2, 5, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_6(2, 6, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_7(2, 7, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_8(2, 8, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_9(2, 9, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_10(2, 10, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_11(2, 11, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_12(2, 12, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_13(2, 13, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_14(2, 14, fanout, "1.2.3.4", 2000);
    NodeInfo node_2_15(2, 15, fanout, "1.2.3.4", 2000);

    NodeInfo node_3_23(3, 23, fanout, "1.2.3.4", 2000);
    NodeInfo node_3_24(3, 24, fanout, "1.2.3.4", 2000);

    routing_info->updateNeighbor(node_0_0);
    routing_info->updateNeighbor(node_1_0);
    routing_info->updateNeighbor(node_1_2);
    routing_info->updateNeighbor(node_1_3);
    routing_info->updateNeighbor(node_2_0);
    routing_info->updateNeighbor(node_2_1);
    routing_info->updateNeighbor(node_2_2);
    routing_info->updateNeighbor(node_2_3);
    routing_info->updateNeighbor(node_2_4);
    routing_info->updateNeighbor(node_2_5);
    routing_info->updateNeighbor(node_2_6);
    routing_info->updateNeighbor(node_2_7);
    routing_info->updateNeighbor(node_2_8);
    routing_info->updateNeighbor(node_2_9);
    routing_info->updateNeighbor(node_2_10);
    routing_info->updateNeighbor(node_2_11);
    routing_info->updateNeighbor(node_2_12);
    routing_info->updateNeighbor(node_2_13);
    routing_info->updateNeighbor(node_2_14);
    routing_info->updateNeighbor(node_2_15);
    routing_info->setAdjacentLeft(node_3_23);
    routing_info->setAdjacentRight(node_3_24);

    MinhtonMessageHeader header;
    header.setSender(node_1_1);
    header.setTarget(node_1_1);
    auto query = std::make_shared<MessageSEVariant>(
        MessageJoin(header, NodeInfo(1, 0, 4, "0.0.0.0", 10000)));

    // direct forwards

    // 1:1 to 0:0
    se_algo.performSearchExact(NodeInfo(0, 0, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(0, 0, fanout)));

    // 1:1 to 2:6
    se_algo.performSearchExact(NodeInfo(2, 6, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 6, fanout)));

    // 1:1 to 3:23
    se_algo.performSearchExact(NodeInfo(3, 23, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 23, fanout)));

    // 1:1 to 3:24
    se_algo.performSearchExact(NodeInfo(3, 24, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 24, fanout)));

    // 1:1 to 1:0
    se_algo.performSearchExact(NodeInfo(1, 0, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(1, 0, fanout)));

    // 1:1 to 1:2
    se_algo.performSearchExact(NodeInfo(1, 2, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(1, 2, fanout)));

    // 1:1 to 1:3
    se_algo.performSearchExact(NodeInfo(1, 3, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(1, 3, fanout)));

    // non direct forwards

    // 1:1 to 3:0 -> over 2:0
    se_algo.performSearchExact(NodeInfo(3, 0, fanout), query);
    REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 0, fanout)));

    // 1:1 to 3:3 -> over 2:0 or 2:1
    se_algo.performSearchExact(NodeInfo(3, 3, fanout), query);
    REQUIRE(((target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 0, fanout)) ||
             (target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 1, fanout))));

    // 1:1 to 3:4 -> over 2:0 or 2:1
    se_algo.performSearchExact(NodeInfo(3, 4, fanout), query);
    REQUIRE(((target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 0, fanout)) ||
             (target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 1, fanout))));

    // 1:1 to 3:31 -> over 2:7, 2:8 or 0:0
    se_algo.performSearchExact(NodeInfo(3, 31, fanout), query);
    REQUIRE(((target_node.getLogicalNodeInfo() == LogicalNodeInfo(0, 0, fanout)) ||
             (target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 7, fanout)) ||
             (target_node.getLogicalNodeInfo() == LogicalNodeInfo(2, 8, fanout))));
  }

  // SECTION("Fanout 2 - 1") {
  //   uint16_t fanout = 2;

  //   NodeInfo node_3_0(3, 0, fanout);  // this is us

  //   auto routing_info = std::make_shared<RoutingInformation>(node_3_0, Logger());

  //   auto access = std::make_shared<AccessContainer>();
  //   access->routing_info = routing_info;

  //   // to see what was sent, we can look at target_node
  //   NodeInfo target_node;
  //   std::function<uint32_t(const MessageVariant &)> save_target =
  //       [&](const MessageVariant &msg) mutable {
  //         target_node =
  //             std::visit([](auto &&message) -> NodeInfo { return message.getTarget(); }, msg);
  //         return 1;
  //       };
  //   access->send = save_target;

  //   MinhtonSearchExactAlgorithm se_algo(access);

  //   NodeInfo node_3_1(3, 1, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_3_2(3, 2, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_3_4(3, 4, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_2_0(2, 0, fanout, "1.2.3.4", 2000);

  //   routing_info->setAdjacentRight(node_2_0);
  //   routing_info->updateNeighbor(node_3_1);
  //   routing_info->updateNeighbor(node_3_2);
  //   routing_info->updateNeighbor(node_3_4);
  //   routing_info->updateNeighbor(node_2_0);

  //   auto query = std::make_shared<MessageSEVariant>(MessageJoin());
  //   query->setSender(node_3_0);

  //   // 3:0 to 3:5
  //   se_algo.performSearchExact(NodeInfo(3, 5, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 4, fanout)));

  //   // 3:0 to 3:6
  //   se_algo.performSearchExact(NodeInfo(3, 6, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 4, fanout)));

  //   // 3:0 to 3:7
  //   se_algo.performSearchExact(NodeInfo(3, 7, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 4, fanout)));

  //   // 3:0 to 2:2
  //   se_algo.performSearchExact(NodeInfo(2, 2, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 4, fanout)));

  //   // 3:0 to 1:1
  //   se_algo.performSearchExact(NodeInfo(1, 1, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 4, fanout)));

  //   // 3:0 to 4:15
  //   se_algo.performSearchExact(NodeInfo(4, 15, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 4, fanout)));
  // }

  // SECTION("Fanout 2 - 2") {
  //   uint16_t fanout = 2;

  //   NodeInfo node_3_4(3, 4, fanout);  // this is us

  //   auto routing_info = std::make_shared<RoutingInformation>(node_3_4, Logger());

  //   auto access = std::make_shared<AccessContainer>();
  //   access->routing_info = routing_info;

  //   // to see what was sent, we can look at target_node
  //   NodeInfo target_node;
  //   std::function<uint32_t(const MessageVariant &)> save_target =
  //       [&](const MessageVariant &msg) mutable {
  //         target_node =
  //             std::visit([](auto &&message) -> NodeInfo { return message.getTarget(); }, msg);
  //         return 1;
  //       };
  //   access->send = save_target;

  //   MinhtonSearchExactAlgorithm se_algo(access);

  //   NodeInfo node_3_0(3, 0, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_3_2(3, 2, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_3_3(3, 3, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_3_5(3, 5, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_3_6(3, 6, fanout, "1.2.3.4", 2000);

  //   NodeInfo node_2_2(2, 2, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_4_6(4, 6, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_4_7(4, 7, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_4_11(4, 11, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);

  //   routing_info->updateNeighbor(node_3_0);
  //   routing_info->updateNeighbor(node_3_2);
  //   routing_info->updateNeighbor(node_3_3);
  //   routing_info->updateNeighbor(node_3_5);
  //   routing_info->updateNeighbor(node_3_6);

  //   routing_info->updateNeighbor(node_4_6);
  //   routing_info->updateNeighbor(node_4_7);
  //   routing_info->updateNeighbor(node_4_11);

  //   routing_info->setAdjacentLeft(node_0_0);
  //   routing_info->setAdjacentRight(node_2_2);
  //   routing_info->setParent(node_2_2);

  //   auto query = std::make_shared<MessageSEVariant>(MessageJoin());
  //   query->setSender(node_3_4);

  //   // 3:4 to 1:1
  //   se_algo.performSearchExact(NodeInfo(1, 1, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 5, fanout)));

  //   // 3:4 to 3:6
  //   se_algo.performSearchExact(NodeInfo(3, 6, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 6, fanout)));

  //   // 3:4 to 3:7
  //   se_algo.performSearchExact(NodeInfo(3, 7, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 6, fanout)));

  //   // 3:4 to 4:15
  //   se_algo.performSearchExact(NodeInfo(4, 15, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 6, fanout)));

  //   // 3:4 to 4:14
  //   se_algo.performSearchExact(NodeInfo(4, 14, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 6, fanout)));

  //   // 3:4 to 2:3
  //   se_algo.performSearchExact(NodeInfo(2, 3, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 6, fanout)));

  //   // 3:4 to 4:11
  //   // we know 4:11, but according to the paper you cannot go over rtn children
  //   se_algo.performSearchExact(NodeInfo(4, 11, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 5, fanout)));

  //   // 3:4 to 3:2
  //   se_algo.performSearchExact(NodeInfo(3, 2, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 2, fanout)));

  //   // 3:4 to 3:0
  //   se_algo.performSearchExact(NodeInfo(3, 0, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 0, fanout)));

  //   // 3:4 to 3:3
  //   se_algo.performSearchExact(NodeInfo(3, 3, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 3, fanout)));
  // }

  // SECTION("Fanout 2 - 3") {
  //   uint16_t fanout = 2;

  //   NodeInfo node_4_11(4, 11, fanout);  // this is us

  //   auto routing_info = std::make_shared<RoutingInformation>(node_4_11, Logger());

  //   auto access = std::make_shared<AccessContainer>();
  //   access->routing_info = routing_info;

  //   // to see what was sent, we can look at target_node
  //   NodeInfo target_node;
  //   std::function<uint32_t(const MessageVariant &)> save_target =
  //       [&](const MessageVariant &msg) mutable {
  //         target_node =
  //             std::visit([](auto &&message) -> NodeInfo { return message.getTarget(); }, msg);
  //         return 1;
  //       };
  //   access->send = save_target;

  //   MinhtonSearchExactAlgorithm se_algo(access);

  //   NodeInfo node_4_15(4, 15, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_4_7(4, 7, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_3_5(3, 5, fanout, "1.2.3.4", 2000);
  //   NodeInfo node_1_1(1, 1, fanout, "1.2.3.4", 2000);

  //   routing_info->updateNeighbor(node_4_7);
  //   routing_info->updateNeighbor(node_4_15);
  //   routing_info->setAdjacentLeft(node_3_5);
  //   routing_info->setParent(node_3_5);
  //   routing_info->setAdjacentRight(node_1_1);

  //   auto query = std::make_shared<MessageSEVariant>(MessageJoin());
  //   query->setSender(node_4_11);

  //   // 4:11 to 1:1
  //   se_algo.performSearchExact(NodeInfo(1, 1, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(1, 1, fanout)));

  //   // 4:11 to 4:7
  //   se_algo.performSearchExact(NodeInfo(4, 7, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(4, 7, fanout)));

  //   // 4:11 to 4:15
  //   se_algo.performSearchExact(NodeInfo(4, 15, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(4, 15, fanout)));

  //   // 4:11 to 3:5
  //   se_algo.performSearchExact(NodeInfo(3, 5, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 5, fanout)));

  //   // 4:11 to 3:6
  //   se_algo.performSearchExact(NodeInfo(3, 6, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(1, 1, fanout)));

  //   // 4:11 to 3:4
  //   se_algo.performSearchExact(NodeInfo(3, 4, fanout), query);
  //   REQUIRE((target_node.getLogicalNodeInfo() == LogicalNodeInfo(3, 5, fanout)));
  // }
}
