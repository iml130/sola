// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "algorithms/esearch/dsn_handler.h"

#include <catch2/catch_test_macros.hpp>

using namespace minhton;

TEST_CASE("DSNHandler Constructor", "[DSNHandler][Constructor]") {
  SECTION("Fanout 2") {
    uint16_t fanout = 2;

    NodeInfo node_3_1(3, 1, fanout, "22.2.3.4", 2000);

    NodeInfo node_4_0(4, 0, fanout, "17.2.37.5", 2040);
    NodeInfo node_4_1(4, 1, fanout, "1.2.3.4", 2000);
    NodeInfo node_4_2(4, 2, fanout, "1.24.3.4", 2000);  // us
    NodeInfo node_4_3(4, 3, fanout, "1.25.3.4", 2040);
    NodeInfo node_4_4(4, 4, fanout, "1.32.3.4", 2030);

    NodeInfo node_5_0(5, 0, fanout, "13.2.3.4", 2000);
    NodeInfo node_5_1(5, 1, fanout, "1.3.3.4", 2000);
    NodeInfo node_5_2(5, 2, fanout, "1.2.3.4", 2300);
    NodeInfo node_5_3(5, 3, fanout, "1.2.5.4", 2400);
    NodeInfo node_5_4(5, 4, fanout, "1.6.3.4", 2500);
    NodeInfo node_5_5(5, 5, fanout, "1.2.3.4", 2600);
    NodeInfo node_5_6(5, 6, fanout, "1.2.3.8", 2700);
    NodeInfo node_5_7(5, 7, fanout, "1.6.3.4", 2800);
    NodeInfo node_5_8(5, 8, fanout, "1.2.3.4", 2900);
    NodeInfo node_5_9(5, 9, fanout, "1.2.3.99", 2050);

    auto routing_info = std::make_shared<RoutingInformation>(node_4_2, Logger());

    routing_info->setParent(node_3_1);
    routing_info->setAdjacentLeft(node_5_4);
    routing_info->setAdjacentRight(node_5_5);

    routing_info->updateNeighbor(node_4_0);
    routing_info->updateNeighbor(node_4_1);
    routing_info->updateNeighbor(node_4_3);
    routing_info->updateNeighbor(node_4_4);

    routing_info->updateNeighbor(node_5_0);
    routing_info->updateNeighbor(node_5_1);
    routing_info->updateNeighbor(node_5_2);
    routing_info->updateNeighbor(node_5_3);
    routing_info->updateNeighbor(node_5_4);
    routing_info->updateNeighbor(node_5_5);
    routing_info->updateNeighbor(node_5_6);
    routing_info->updateNeighbor(node_5_7);
    routing_info->updateNeighbor(node_5_8);
    routing_info->updateNeighbor(node_5_9);

    std::function<void(const NodeInfo &node)> empty_callback =
        [](const NodeInfo &) { /* intentionally empty*/ };
    DSNHandler handler(routing_info, empty_callback);

    REQUIRE(handler.isActive());

    auto cover_data = handler.getCoverData();
    REQUIRE(cover_data.size() == getCoverArea(4, 2, 2).size());

    auto cover_nodes = {node_4_0, node_4_1, node_4_3, node_5_0, node_5_1, node_5_2,
                        node_5_3, node_5_4, node_5_5, node_5_6, node_5_7};

    for (auto const &cover_node : cover_nodes) {
      auto cover_node_it = cover_data.find(cover_node.getLogicalNodeInfo());
      REQUIRE(cover_node_it != cover_data.end());
      REQUIRE(cover_node_it->second.getPhysicalNodeInfo() == cover_node.getPhysicalNodeInfo());
      REQUIRE(cover_node_it->second.getData().empty());
    }

    REQUIRE(cover_data.size() == getCoverArea(4, 2, 2).size());
  }

  SECTION("Fanout 3") {
    uint16_t fanout = 3;

    NodeInfo node_3_3(3, 3, fanout, "1.6.3.4", 2500);  // parent

    NodeInfo node_4_6(4, 6, fanout, "1.6.3.4", 2500);
    NodeInfo node_4_7(4, 7, fanout, "1.6.3.4", 2500);
    NodeInfo node_4_8(4, 8, fanout, "1.6.3.4", 2500);

    NodeInfo node_4_9(4, 9, fanout, "1.6.3.4", 2500);  // us

    NodeInfo node_4_10(4, 10, fanout, "1.6.3.4", 2500);
    NodeInfo node_4_11(4, 11, fanout, "1.6.3.4", 2500);
    NodeInfo node_4_12(4, 12, fanout, "1.6.3.4", 2500);

    NodeInfo node_5_18(5, 18, fanout, "1.6.3.4", 2500);  // children of 4:6
    NodeInfo node_5_19(5, 19, fanout, "1.6.3.4", 2500);
    NodeInfo node_5_20(5, 20, fanout, "1.6.3.4", 2500);
    NodeInfo node_5_21(5, 21, fanout, "1.6.3.4", 2300);  // children of 4:7
    NodeInfo node_5_22(5, 22, fanout, "1.6.3.4", 2509);
    NodeInfo node_5_23(5, 23, fanout, "1.6.3.4", 2590);
    NodeInfo node_5_24(5, 24, fanout, "1.6.3.5", 2500);  // children of 4:8
    NodeInfo node_5_25(5, 25, fanout, "1.6.5.4", 2590);
    NodeInfo node_5_26(5, 26, fanout, "1.6.3.4", 2500);
    NodeInfo node_5_27(5, 27, fanout, "1.6.3.6", 2570);  // children of 4:9 - us
    NodeInfo node_5_28(5, 28, fanout, "1.6.3.7", 2450);
    NodeInfo node_5_29(5, 29, fanout, "1.6.3.8", 2200);
    NodeInfo node_5_30(5, 30, fanout, "12.6.3.4", 2507);  // children of 4:10
    NodeInfo node_5_31(5, 31, fanout, "13.6.3.4", 2505);
    NodeInfo node_5_32(5, 32, fanout, "14.6.3.4", 2503);
    NodeInfo node_5_33(5, 33, fanout, "15.6.3.4", 2590);  // children of 4:11
    NodeInfo node_5_34(5, 34, fanout, "16.6.3.4", 2580);
    NodeInfo node_5_35(5, 35, fanout, "17.6.3.4", 2570);
    NodeInfo node_5_36(5, 36, fanout, "19.6.3.4", 2520);  // children of 4:12
    NodeInfo node_5_37(5, 37, fanout, "1.96.3.4", 2530);
    NodeInfo node_5_38(5, 38, fanout, "1.6.93.4", 2540);

    auto routing_info = std::make_shared<RoutingInformation>(node_4_9, Logger());

    routing_info->setParent(node_3_3);
    routing_info->setAdjacentLeft(node_5_28);
    routing_info->setAdjacentRight(node_5_29);

    routing_info->updateNeighbor(node_4_6);
    routing_info->updateNeighbor(node_4_7);
    routing_info->updateNeighbor(node_4_8);
    routing_info->updateNeighbor(node_4_10);
    routing_info->updateNeighbor(node_4_11);
    routing_info->updateNeighbor(node_4_12);

    routing_info->updateNeighbor(node_5_18);
    routing_info->updateNeighbor(node_5_19);
    routing_info->updateNeighbor(node_5_20);
    routing_info->updateNeighbor(node_5_21);
    routing_info->updateNeighbor(node_5_22);
    routing_info->updateNeighbor(node_5_23);
    routing_info->updateNeighbor(node_5_24);
    routing_info->updateNeighbor(node_5_25);
    routing_info->updateNeighbor(node_5_26);
    routing_info->updateNeighbor(node_5_27);
    routing_info->updateNeighbor(node_5_28);
    routing_info->updateNeighbor(node_5_29);
    routing_info->updateNeighbor(node_5_30);
    routing_info->updateNeighbor(node_5_31);
    routing_info->updateNeighbor(node_5_32);
    routing_info->updateNeighbor(node_5_33);
    routing_info->updateNeighbor(node_5_34);
    routing_info->updateNeighbor(node_5_35);
    routing_info->updateNeighbor(node_5_36);
    routing_info->updateNeighbor(node_5_37);
    routing_info->updateNeighbor(node_5_38);

    std::function<void(const NodeInfo &node)> empty_callback =
        [](const NodeInfo &) { /* intentionally empty*/ };
    DSNHandler handler(routing_info, empty_callback);

    auto cover_data = handler.getCoverData();

    REQUIRE(handler.isActive());

    auto cover_nodes = {// node_4_6,
                        node_4_7, node_4_8, node_4_10, node_4_11,
                        // node_5_18,
                        // node_5_19,
                        // node_5_20,
                        node_5_21, node_5_22, node_5_23, node_5_24, node_5_25, node_5_26, node_5_27,
                        node_5_28, node_5_29, node_5_30, node_5_31, node_5_32, node_5_33, node_5_34,
                        node_5_35};

    for (auto const &cover_node : cover_nodes) {
      auto cover_node_it = cover_data.find(cover_node.getLogicalNodeInfo());
      REQUIRE(cover_node_it != cover_data.end());
      REQUIRE(cover_node_it->second.getPhysicalNodeInfo() == cover_node.getPhysicalNodeInfo());
      REQUIRE(cover_node_it->second.getData().empty());
    }

    REQUIRE(cover_data.size() == getCoverArea(4, 9, 3).size());
  }

  SECTION("Fanout 3 - Incomplete") {
    uint16_t fanout = 3;

    NodeInfo node_3_3(3, 3, fanout, "1.6.3.4", 2500);  // parent

    NodeInfo node_4_6(4, 6, fanout, "1.6.3.4", 2500);
    NodeInfo node_4_7(4, 7, fanout, "1.6.3.4", 2500);
    NodeInfo node_4_8(4, 8, fanout, "1.6.3.4", 2500);

    NodeInfo node_4_9(4, 9, fanout, "1.6.3.4", 2500);  // us

    NodeInfo node_4_10(4, 10, fanout, "1.6.3.4", 2500);
    NodeInfo node_4_11(4, 11, fanout, "1.6.3.4", 2500);
    NodeInfo node_4_12(4, 12, fanout, "1.6.3.4", 2500);

    NodeInfo node_5_18(5, 18, fanout, "1.6.3.4", 2500);  // children of 4:6
    NodeInfo node_5_19(5, 19, fanout, "1.6.3.4", 2500);
    NodeInfo node_5_20(5, 20, fanout, "1.6.3.4", 2500);
    NodeInfo node_5_21(5, 21, fanout, "1.6.3.4", 2300);  // children of 4:7
    NodeInfo node_5_22(5, 22, fanout, "1.6.3.4", 2509);
    NodeInfo node_5_23(5, 23, fanout, "1.6.3.4", 2590);
    NodeInfo node_5_24(5, 24, fanout, "1.6.3.5", 2500);  // children of 4:8
    NodeInfo node_5_25(5, 25, fanout, "1.6.5.4", 2590);
    NodeInfo node_5_26(5, 26, fanout, "1.6.3.4", 2500);
    NodeInfo node_5_27(5, 27, fanout, "1.6.3.6", 2570);  // children of 4:9 - us
    NodeInfo node_5_28(5, 28, fanout, "1.6.3.7", 2450);

    auto routing_info = std::make_shared<RoutingInformation>(node_4_9, Logger());

    routing_info->setParent(node_3_3);
    routing_info->setAdjacentLeft(node_5_28);
    routing_info->setAdjacentRight(node_4_10);

    routing_info->updateNeighbor(node_4_6);
    routing_info->updateNeighbor(node_4_7);
    routing_info->updateNeighbor(node_4_8);
    routing_info->updateNeighbor(node_4_10);
    routing_info->updateNeighbor(node_4_11);
    routing_info->updateNeighbor(node_4_12);

    routing_info->updateNeighbor(node_5_18);
    routing_info->updateNeighbor(node_5_19);
    routing_info->updateNeighbor(node_5_20);
    routing_info->updateNeighbor(node_5_21);
    routing_info->updateNeighbor(node_5_22);
    routing_info->updateNeighbor(node_5_23);
    routing_info->updateNeighbor(node_5_24);
    routing_info->updateNeighbor(node_5_25);
    routing_info->updateNeighbor(node_5_26);
    routing_info->updateNeighbor(node_5_27);
    routing_info->updateNeighbor(node_5_28);

    std::function<void(const NodeInfo &node)> empty_callback =
        [](const NodeInfo &) { /* intentionally empty*/ };
    DSNHandler handler(routing_info, empty_callback);

    auto cover_data = handler.getCoverData();

    REQUIRE(handler.isActive());

    auto cover_nodes = {// node_4_6,
                        node_4_7, node_4_8, node_4_10, node_4_11,
                        // node_5_18,
                        // node_5_19,
                        // node_5_20,
                        node_5_21, node_5_22, node_5_23, node_5_24, node_5_25, node_5_26, node_5_27,
                        node_5_28};

    for (auto const &cover_node : cover_nodes) {
      auto cover_node_it = cover_data.find(cover_node.getLogicalNodeInfo());
      REQUIRE(cover_node_it != cover_data.end());
      REQUIRE(cover_node_it->second.getPhysicalNodeInfo() == cover_node.getPhysicalNodeInfo());
      REQUIRE(cover_node_it->second.getData().empty());
    }

    REQUIRE(cover_data.size() < getCoverArea(4, 9, 3).size());
  }
}

TEST_CASE("DSNHandler onNeighborChangeNotification", "[DSNHandler][Constructor]") {
  uint16_t fanout = 2;

  NodeInfo node_3_1(3, 1, fanout, "22.2.3.4", 2000);

  NodeInfo node_4_0(4, 0, fanout, "17.2.37.5", 2040);
  NodeInfo node_4_1(4, 1, fanout, "1.2.3.4", 2000);
  NodeInfo node_4_2(4, 2, fanout, "1.24.3.4", 2000);  // us
  NodeInfo node_4_3(4, 3, fanout, "1.25.3.4", 2040);
  NodeInfo node_4_4(4, 4, fanout, "1.32.3.4", 2030);

  NodeInfo node_5_0(5, 0, fanout, "13.2.3.4", 2000);
  NodeInfo node_5_1(5, 1, fanout, "1.3.3.4", 2000);
  NodeInfo node_5_2(5, 2, fanout, "1.2.3.4", 2300);
  NodeInfo node_5_3(5, 3, fanout, "1.2.5.4", 2400);
  NodeInfo node_5_4(5, 4, fanout, "1.6.3.4", 2500);
  NodeInfo node_5_5(5, 5, fanout, "1.2.3.4", 2600);
  NodeInfo node_5_6(5, 6, fanout, "1.2.3.8", 2700);
  NodeInfo node_5_7(5, 7, fanout, "1.6.3.4", 2800);
  NodeInfo node_5_8(5, 8, fanout, "1.2.3.4", 2900);
  NodeInfo node_5_9(5, 9, fanout, "1.2.3.99", 2050);

  auto routing_info = std::make_shared<RoutingInformation>(node_4_2, Logger());

  routing_info->updateNeighbor(node_4_0);
  routing_info->updateNeighbor(node_4_1);
  routing_info->updateNeighbor(node_4_3);
  routing_info->updateNeighbor(node_4_4);

  routing_info->updateNeighbor(node_5_0);
  routing_info->updateNeighbor(node_5_1);
  routing_info->updateNeighbor(node_5_2);
  routing_info->updateNeighbor(node_5_3);
  routing_info->updateNeighbor(node_5_4);

  std::function<void(const NodeInfo &node)> empty_callback =
      [](const NodeInfo &) { /* intentionally empty*/ };
  DSNHandler handler(routing_info, empty_callback);
  REQUIRE(handler.isActive());

  auto cover_data1 = handler.getCoverData();

  auto init_cover_nodes = {
      node_4_0, node_4_1, node_4_3, node_4_4, node_5_0, node_5_1, node_5_2, node_5_3, node_5_4,
  };

  for (auto const &cover_node : init_cover_nodes) {
    auto cover_node_it = cover_data1.find(cover_node.getLogicalNodeInfo());
    REQUIRE(cover_node_it != cover_data1.end());
    REQUIRE(cover_node_it->second.getPhysicalNodeInfo() == cover_node.getPhysicalNodeInfo());
    REQUIRE(cover_node_it->second.getData().empty());
  }

  routing_info->updateNeighbor(node_5_6);
  REQUIRE(handler.isActive());

  auto cover_data2 = handler.getCoverData();
  auto node_5_6_it = cover_data2.find(node_5_6.getLogicalNodeInfo());
  REQUIRE(node_5_6_it != cover_data2.end());
  REQUIRE(node_5_6_it->second.getPhysicalNodeInfo() == node_5_6.getPhysicalNodeInfo());
  REQUIRE(node_5_6_it->second.getData().empty());
  REQUIRE(cover_data1.size() + 1 == cover_data2.size());

  routing_info->updateNeighbor(node_5_6);
  REQUIRE(handler.isActive());

  routing_info->updateNeighbor(node_5_7);
  REQUIRE(handler.isActive());

  auto cover_data3 = handler.getCoverData();
  auto node_5_7_it = cover_data3.find(node_5_7.getLogicalNodeInfo());
  REQUIRE(node_5_7_it != cover_data3.end());
  REQUIRE(node_5_7_it->second.getPhysicalNodeInfo() == node_5_7.getPhysicalNodeInfo());
  REQUIRE(node_5_7_it->second.getData().empty());
  REQUIRE(cover_data2.size() + 1 == cover_data3.size());

  node_5_7.setPhysicalNodeInfo(PhysicalNodeInfo("8.8.8.8", 1111));
  routing_info->updateNeighbor(node_5_7);
  REQUIRE(handler.isActive());

  auto cover_data4 = handler.getCoverData();
  auto node_5_7_it2 = cover_data4.find(node_5_7.getLogicalNodeInfo());
  REQUIRE(node_5_7_it2 != cover_data4.end());
  REQUIRE(node_5_7_it2->second.getPhysicalNodeInfo() == node_5_7.getPhysicalNodeInfo());
  REQUIRE(node_5_7_it2->second.getData().empty());
  REQUIRE(cover_data3.size() == cover_data4.size());

  routing_info->removeNeighbor(node_4_3);
  REQUIRE(handler.isActive());

  auto cover_data5 = handler.getCoverData();
  auto node_4_3_it = cover_data5.find(node_4_3.getLogicalNodeInfo());
  REQUIRE(node_4_3_it == cover_data5.end());
  REQUIRE(cover_data4.size() - 1 == cover_data5.size());

  routing_info->updateNeighbor(NodeInfo(4, 15, fanout, "1.2.3.4", 4444));
  REQUIRE(handler.isActive());
  auto cover_data6 = handler.getCoverData();
  REQUIRE(cover_data5.size() == cover_data6.size());
}

TEST_CASE("DSNHandler changing positions", "[DSNHandler][changing positions]") {
  uint16_t fanout = 2;

  NodeInfo node_3_1(3, 1, fanout, "22.2.3.4", 2000);

  NodeInfo node_4_0(4, 0, fanout, "17.2.37.5", 2040);
  NodeInfo node_4_1(4, 1, fanout, "1.2.3.4", 2000);
  NodeInfo node_4_2(4, 2, fanout, "1.24.3.4", 2000);  // us
  NodeInfo node_4_3(4, 3, fanout, "1.25.3.4", 2040);
  NodeInfo node_4_4(4, 4, fanout, "1.32.3.4", 2030);

  NodeInfo node_5_0(5, 0, fanout, "13.2.3.4", 2000);
  NodeInfo node_5_1(5, 1, fanout, "1.3.3.4", 2000);
  NodeInfo node_5_2(5, 2, fanout, "1.2.3.4", 2300);
  NodeInfo node_5_3(5, 3, fanout, "1.2.5.4", 2400);
  NodeInfo node_5_4(5, 4, fanout, "1.6.3.4", 2500);
  NodeInfo node_5_5(5, 5, fanout, "1.2.3.4", 2600);
  NodeInfo node_5_6(5, 6, fanout, "1.2.3.8", 2700);
  NodeInfo node_5_7(5, 7, fanout, "1.6.3.4", 2800);
  NodeInfo node_5_8(5, 8, fanout, "1.2.3.4", 2900);
  NodeInfo node_5_9(5, 9, fanout, "1.2.3.99", 2050);

  auto routing_info = std::make_shared<RoutingInformation>(node_4_2, Logger());

  routing_info->setParent(node_3_1);
  routing_info->setAdjacentLeft(node_5_4);
  routing_info->setAdjacentRight(node_5_5);

  routing_info->updateNeighbor(node_4_0);
  routing_info->updateNeighbor(node_4_1);
  routing_info->updateNeighbor(node_4_3);
  routing_info->updateNeighbor(node_4_4);

  routing_info->updateNeighbor(node_5_0);
  routing_info->updateNeighbor(node_5_1);
  routing_info->updateNeighbor(node_5_2);
  routing_info->updateNeighbor(node_5_3);
  routing_info->updateNeighbor(node_5_4);
  routing_info->updateNeighbor(node_5_5);
  routing_info->updateNeighbor(node_5_6);
  routing_info->updateNeighbor(node_5_7);
  routing_info->updateNeighbor(node_5_8);
  routing_info->updateNeighbor(node_5_9);

  std::function<void(const NodeInfo &node)> empty_callback =
      [](const NodeInfo &) { /* intentionally empty*/ };
  DSNHandler handler(routing_info, empty_callback);

  REQUIRE(handler.isActive());

  std::string my_test_string = "hihihi";
  std::unordered_map<NodeData::Key, NodeData::ValueAndType> node_5_2_updates = {
      {"a01", {10, NodeData::ValueType::kValueDynamic}},
      {"a02", {my_test_string, NodeData::ValueType::kValueDynamic}},
      {"a03", {3.14159f, NodeData::ValueType::kValueDynamic}}};
  handler.updateInquiredOrSubscribedAttributeValues(node_5_2, node_5_2_updates, 1000);

  auto cover_data = handler.getCoverData();
  auto node_5_2_it = cover_data.find(node_5_2.getLogicalNodeInfo());

  auto a01tuple = node_5_2_it->second.getValueAndTimestamp("a01");
  auto a01_pval = *std::get_if<int>(&std::get<0>(a01tuple));
  auto a01_timestamp = std::get<1>(a01tuple);
  REQUIRE(a01_pval == 10);
  REQUIRE(a01_timestamp == 1000);

  auto a02tuple = node_5_2_it->second.getValueAndTimestamp("a02");
  auto a02_pval = *std::get_if<std::string>(&std::get<0>(a02tuple));
  auto a02_timestamp = std::get<1>(a02tuple);
  REQUIRE(a02_pval == "hihihi");
  REQUIRE(a02_timestamp == 1000);

  auto a03tuple = node_5_2_it->second.getValueAndTimestamp("a03");
  auto a03_pval = *std::get_if<float>(&std::get<0>(a03tuple));
  auto a03_timestamp = std::get<1>(a03tuple);
  REQUIRE(a03_pval == 3.14159f);
  REQUIRE(a03_timestamp == 1000);

  REQUIRE(node_5_2_it->second.getUpdateTimestamps("a01").front() == 1000);
  REQUIRE(node_5_2_it->second.getUpdateTimestamps("a02").front() == 1000);
  REQUIRE(node_5_2_it->second.getUpdateTimestamps("a03").front() == 1000);

  std::unordered_map<NodeData::Key, NodeData::ValueAndType> node_5_3_updates = {
      {"a01", {11, NodeData::ValueType::kValueDynamic}}};
  handler.updateInquiredOrSubscribedAttributeValues(node_5_3, node_5_3_updates, 1100);

  auto cover_data2 = handler.getCoverData();
  auto node_5_3_it = cover_data2.find(node_5_3.getLogicalNodeInfo());

  auto a01tuple2 = node_5_3_it->second.getValueAndTimestamp("a01");
  auto a01_pval2 = *std::get_if<int>(&std::get<0>(a01tuple2));
  auto a01_timestamp2 = std::get<1>(a01tuple2);
  REQUIRE(a01_pval2 == 11);
  REQUIRE(a01_timestamp2 == 1100);

  std::string my_test_string2 = "i need coffee";
  std::unordered_map<NodeData::Key, NodeData::ValueAndType> node_5_3_updates2 = {
      {"a01", {my_test_string2, NodeData::ValueType::kValueDynamic}}};
  handler.updateInquiredOrSubscribedAttributeValues(node_5_3, node_5_3_updates2, 1200);

  auto cover_data3 = handler.getCoverData();
  auto node_5_3_it2 = cover_data3.find(node_5_3.getLogicalNodeInfo());

  auto a01tuple3 = node_5_3_it2->second.getValueAndTimestamp("a01");
  auto a01_pval3 = *std::get_if<std::string>(&std::get<0>(a01tuple3));
  auto a01_timestamp3 = std::get<1>(a01tuple3);
  REQUIRE(a01_pval3 == "i need coffee");
  REQUIRE(a01_timestamp3 == 1200);

  REQUIRE(node_5_3_it2->second.getUpdateTimestamps("a01").front() == 1100);
  REQUIRE(node_5_3_it2->second.getUpdateTimestamps("a01").back() == 1200);
}

TEST_CASE("DSNHandler getTrueNodes and getUndecidedNodesAndMissingKeys",
          "[DSNHandler][getTrueNodes, Undecided]") {
  uint16_t fanout = 2;

  NodeInfo node_3_1(3, 1, fanout, "22.2.3.4", 2000);

  NodeInfo node_4_0(4, 0, fanout, "17.2.37.5", 2040);
  NodeInfo node_4_1(4, 1, fanout, "1.2.3.4", 2000);
  NodeInfo node_4_2(4, 2, fanout, "1.24.3.4", 2000);  // us
  NodeInfo node_4_3(4, 3, fanout, "1.25.3.4", 2040);
  NodeInfo node_4_4(4, 4, fanout, "1.32.3.4", 2030);

  NodeInfo node_5_0(5, 0, fanout, "13.2.3.4", 2000);
  NodeInfo node_5_1(5, 1, fanout, "1.3.3.4", 2000);
  NodeInfo node_5_2(5, 2, fanout, "1.2.3.4", 2300);
  NodeInfo node_5_3(5, 3, fanout, "1.2.5.4", 2400);
  NodeInfo node_5_4(5, 4, fanout, "1.6.3.4", 2500);
  NodeInfo node_5_5(5, 5, fanout, "1.2.3.4", 2600);
  NodeInfo node_5_6(5, 6, fanout, "1.2.3.8", 2700);
  NodeInfo node_5_7(5, 7, fanout, "1.6.3.4", 2800);
  NodeInfo node_5_8(5, 8, fanout, "1.2.3.4", 2900);
  NodeInfo node_5_9(5, 9, fanout, "1.2.3.99", 2050);

  auto routing_info = std::make_shared<RoutingInformation>(node_4_2, Logger());

  routing_info->setParent(node_3_1);
  routing_info->setAdjacentLeft(node_5_4);
  routing_info->setAdjacentRight(node_5_5);

  routing_info->updateNeighbor(node_4_0);
  routing_info->updateNeighbor(node_4_1);
  routing_info->updateNeighbor(node_4_3);
  routing_info->updateNeighbor(node_4_4);

  routing_info->updateNeighbor(node_5_0);
  routing_info->updateNeighbor(node_5_1);
  routing_info->updateNeighbor(node_5_2);
  routing_info->updateNeighbor(node_5_3);
  routing_info->updateNeighbor(node_5_4);
  routing_info->updateNeighbor(node_5_5);
  routing_info->updateNeighbor(node_5_6);
  routing_info->updateNeighbor(node_5_7);
  routing_info->updateNeighbor(node_5_8);
  routing_info->updateNeighbor(node_5_9);

  std::function<void(const NodeInfo &node)> empty_callback =
      [](const NodeInfo &) { /* intentionally empty*/ };
  DSNHandler handler(routing_info, empty_callback);
  REQUIRE(handler.isActive());

  FindQuery query1;
  auto sub_expr1 = std::make_shared<PresenceExpression>("topicA");
  auto sub_expr2 =
      std::make_shared<NumericComparisonExpression<int>>("weight", ComparisonTypes::kGreater, 100);
  auto expr1 = std::make_shared<OrExpression>(sub_expr1, sub_expr2);
  query1.setScope(FindQuery::FindQueryScope::kAll);
  query1.setBooleanExpression(expr1);
  query1.setValidityThreshold(350);

  std::unordered_map<NodeData::Key, NodeData::ValueAndType> has_topicA = {
      {"topicA", {true, NodeData::ValueType::kValueDynamic}}};
  handler.updateInquiredOrSubscribedAttributeValues(node_4_1, has_topicA, 1000);
  handler.updateInquiredOrSubscribedAttributeValues(node_4_0, has_topicA, 1000);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_1, has_topicA, 1100);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_2, has_topicA, 1100);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_3, has_topicA, 1100);

  std::unordered_map<NodeData::Key, NodeData::ValueAndType> has_not_topicA = {
      {"topicA", {false, NodeData::ValueType::kValueDynamic}}};
  handler.updateInquiredOrSubscribedAttributeValues(node_5_0, has_not_topicA, 1100);

  std::unordered_map<NodeData::Key, NodeData::ValueAndType> weight_over_100 = {
      {"weight", {101, NodeData::ValueType::kValueDynamic}}};
  handler.updateInquiredOrSubscribedAttributeValues(node_5_1, weight_over_100, 1000);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_4, weight_over_100, 1200);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_5, weight_over_100, 1300);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_6, weight_over_100, 1000);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_9, weight_over_100, 1400);

  std::unordered_map<NodeData::Key, NodeData::ValueAndType> weight_equal_to_100 = {
      {"weight", {100, NodeData::ValueType::kValueDynamic}}};
  handler.updateInquiredOrSubscribedAttributeValues(node_5_3, weight_equal_to_100, 1400);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_7, weight_equal_to_100, 1400);
  handler.updateInquiredOrSubscribedAttributeValues(node_5_8, weight_equal_to_100, 1400);

  auto true_nodes_1 = handler.getTrueNodes(query1, 1401);

  REQUIRE(true_nodes_1.size() == 6);
  REQUIRE(std::find(true_nodes_1.begin(), true_nodes_1.end(), node_5_1) != true_nodes_1.end());
  REQUIRE(std::find(true_nodes_1.begin(), true_nodes_1.end(), node_5_2) != true_nodes_1.end());
  REQUIRE(std::find(true_nodes_1.begin(), true_nodes_1.end(), node_5_3) != true_nodes_1.end());
  REQUIRE(std::find(true_nodes_1.begin(), true_nodes_1.end(), node_5_4) != true_nodes_1.end());
  REQUIRE(std::find(true_nodes_1.begin(), true_nodes_1.end(), node_5_5) != true_nodes_1.end());
  REQUIRE(std::find(true_nodes_1.begin(), true_nodes_1.end(), node_5_9) != true_nodes_1.end());

  std::vector<std::tuple<NodeInfo, std::vector<std::string>>> undec_nodes_missing_keys_1 =
      handler.getUndecidedNodesAndMissingKeys(query1, false, 1401);

  auto tuple_4_0_it =
      std::find_if(undec_nodes_missing_keys_1.begin(), undec_nodes_missing_keys_1.end(),
                   [&](std::tuple<NodeInfo, std::vector<std::string>> tup) {
                     return std::get<0>(tup) == node_4_0;
                   });
  REQUIRE(tuple_4_0_it != undec_nodes_missing_keys_1.end());
  auto missing_keys_4_0 = std::get<1>(*tuple_4_0_it);
  REQUIRE(missing_keys_4_0.size() == 2);
  REQUIRE(((missing_keys_4_0[0] == "weight") || (missing_keys_4_0[0] == "topicA")));
  REQUIRE(((missing_keys_4_0[1] == "weight") || (missing_keys_4_0[1] == "topicA")));

  auto tuple_4_1_it =
      std::find_if(undec_nodes_missing_keys_1.begin(), undec_nodes_missing_keys_1.end(),
                   [&](std::tuple<NodeInfo, std::vector<std::string>> tup) {
                     return std::get<0>(tup) == node_4_1;
                   });
  REQUIRE(tuple_4_1_it != undec_nodes_missing_keys_1.end());
  auto missing_keys_4_1 = std::get<1>(*tuple_4_1_it);
  REQUIRE(missing_keys_4_1.size() == 2);
  REQUIRE(((missing_keys_4_1[0] == "weight") || (missing_keys_4_1[0] == "topicA")));
  REQUIRE(((missing_keys_4_1[1] == "weight") || (missing_keys_4_1[1] == "topicA")));

  auto tuple_5_6_it =
      std::find_if(undec_nodes_missing_keys_1.begin(), undec_nodes_missing_keys_1.end(),
                   [&](std::tuple<NodeInfo, std::vector<std::string>> tup) {
                     return std::get<0>(tup) == node_5_6;
                   });
  REQUIRE(tuple_5_6_it != undec_nodes_missing_keys_1.end());
  auto missing_keys_5_6 = std::get<1>(*tuple_5_6_it);
  REQUIRE(missing_keys_5_6.size() == 2);
  REQUIRE(((missing_keys_5_6[0] == "weight") || (missing_keys_5_6[0] == "topicA")));
  REQUIRE(((missing_keys_5_6[1] == "weight") || (missing_keys_5_6[1] == "topicA")));

  auto tuple_4_3_it =
      std::find_if(undec_nodes_missing_keys_1.begin(), undec_nodes_missing_keys_1.end(),
                   [&](std::tuple<NodeInfo, std::vector<std::string>> tup) {
                     return std::get<0>(tup) == node_4_3;
                   });
  REQUIRE(tuple_4_3_it != undec_nodes_missing_keys_1.end());
  auto missing_keys_4_3 = std::get<1>(*tuple_4_3_it);
  REQUIRE(missing_keys_4_3.size() == 2);
  REQUIRE(((missing_keys_4_3[0] == "weight") || (missing_keys_4_3[0] == "topicA")));
  REQUIRE(((missing_keys_4_3[1] == "weight") || (missing_keys_4_3[1] == "topicA")));

  auto tuple_4_4_it =
      std::find_if(undec_nodes_missing_keys_1.begin(), undec_nodes_missing_keys_1.end(),
                   [&](std::tuple<NodeInfo, std::vector<std::string>> tup) {
                     return std::get<0>(tup) == node_4_4;
                   });
  REQUIRE(tuple_4_4_it != undec_nodes_missing_keys_1.end());
  auto missing_keys_4_4 = std::get<1>(*tuple_4_4_it);
  REQUIRE(missing_keys_4_4.size() == 2);
  REQUIRE(((missing_keys_4_4[0] == "weight") || (missing_keys_4_4[0] == "topicA")));
  REQUIRE(((missing_keys_4_4[1] == "weight") || (missing_keys_4_4[1] == "topicA")));

  auto tuple_5_0_it =
      std::find_if(undec_nodes_missing_keys_1.begin(), undec_nodes_missing_keys_1.end(),
                   [&](std::tuple<NodeInfo, std::vector<std::string>> tup) {
                     return std::get<0>(tup) == node_5_0;
                   });
  REQUIRE(tuple_5_0_it != undec_nodes_missing_keys_1.end());
  auto missing_keys_5_0 = std::get<1>(*tuple_5_0_it);
  REQUIRE(missing_keys_5_0.size() == 1);
  REQUIRE(missing_keys_5_0[0] == "weight");

  auto tuple_5_7_it =
      std::find_if(undec_nodes_missing_keys_1.begin(), undec_nodes_missing_keys_1.end(),
                   [&](std::tuple<NodeInfo, std::vector<std::string>> tup) {
                     return std::get<0>(tup) == node_5_7;
                   });
  REQUIRE(tuple_5_7_it != undec_nodes_missing_keys_1.end());
  auto missing_keys_5_7 = std::get<1>(*tuple_5_7_it);
  REQUIRE(missing_keys_5_7.size() == 1);
  REQUIRE(missing_keys_5_7[0] == "topicA");

  auto tuple_5_8_it =
      std::find_if(undec_nodes_missing_keys_1.begin(), undec_nodes_missing_keys_1.end(),
                   [&](std::tuple<NodeInfo, std::vector<std::string>> tup) {
                     return std::get<0>(tup) == node_5_8;
                   });
  REQUIRE(tuple_5_8_it != undec_nodes_missing_keys_1.end());
  auto missing_keys_5_8 = std::get<1>(*tuple_5_8_it);
  REQUIRE(missing_keys_5_8.size() == 1);
  REQUIRE(missing_keys_5_8[0] == "topicA");

  REQUIRE(undec_nodes_missing_keys_1.size() == 8);

  auto undec_nodes_missing_keys_2 = handler.getUndecidedNodesAndMissingKeys(query1, true, 1401);
  REQUIRE(undec_nodes_missing_keys_2.empty());

  query1.setValidityThreshold(450);

  auto true_nodes_2 = handler.getTrueNodes(query1, 1401);

  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_4_0) != true_nodes_2.end());
  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_4_1) != true_nodes_2.end());
  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_5_1) != true_nodes_2.end());
  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_5_2) != true_nodes_2.end());
  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_5_3) != true_nodes_2.end());
  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_5_4) != true_nodes_2.end());
  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_5_5) != true_nodes_2.end());
  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_5_6) != true_nodes_2.end());
  REQUIRE(std::find(true_nodes_2.begin(), true_nodes_2.end(), node_5_9) != true_nodes_2.end());
  REQUIRE(true_nodes_2.size() == 9);

  auto undec_nodes_missing_keys_3 = handler.getUndecidedNodesAndMissingKeys(query1, false, 1401);

  REQUIRE(undec_nodes_missing_keys_3.size() == 5);
}
