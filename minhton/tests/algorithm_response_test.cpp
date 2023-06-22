// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

#include "algorithms/misc/response_algorithm_general.h"

using namespace minhton;

TEST_CASE("ResponseAlgorithmGeneral processUpdateNeighbors",
          "[ResponseAlgorithmGeneral][processUpdateNeighbors]") {
  uint16_t fanout = 2;

  NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);
  NodeInfo node_1_0(1, 0, fanout, "1.2.3.5", 2000);
  NodeInfo node_1_1(1, 1, fanout, "1.2.3.6", 2000);

  NodeInfo node_2_0(2, 0, fanout, "1.2.3.7", 2000);
  NodeInfo node_2_1(2, 1, fanout, "1.2.3.8", 2000);
  NodeInfo node_2_2(2, 2, fanout, "1.2.3.9", 2000);
  NodeInfo node_2_3(2, 3, fanout, "1.2.3.10", 2000);

  NodeInfo node_3_2(3, 2, fanout, "1.2.3.42", 1234);
  NodeInfo node_3_4(3, 4, fanout, "1.3.3.10", 2000);
  NodeInfo node_3_5(3, 5, fanout, "1.4.3.10", 2000);
  NodeInfo node_3_6(3, 6, fanout, "1.4.5.10", 2000);
  NodeInfo node_3_7(3, 7, fanout, "1.5.3.10", 2000);

  auto access = std::make_shared<AccessContainer>();
  auto routing_info = std::make_shared<RoutingInformation>(node_2_3, Logger());
  access->routing_info = routing_info;

  ResponseAlgorithmGeneral response_algo(access);

  // we are 2:3
  routing_info->setParent(node_1_1);
  routing_info->setAdjacentLeft(node_1_1);
  routing_info->updateRoutingTableNeighbor(node_2_1);
  routing_info->updateRoutingTableNeighbor(node_2_2);
  routing_info->updateRoutingTableNeighborChild(node_3_4);
  routing_info->updateRoutingTableNeighborChild(node_3_5);

  // update 3:6 as adjacent

  REQUIRE(!routing_info->getChild(0).isInitialized());
  REQUIRE(routing_info->getAdjacentLeft() == node_1_1);

  MinhtonMessageHeader header(node_2_3, node_3_6);
  MessageUpdateNeighbors msg1(header,
                              {std::make_tuple(node_3_6, NeighborRelationship::kAdjacentLeft)});
  REQUIRE_NOTHROW(response_algo.process(msg1));

  REQUIRE(!routing_info->getChild(0).isInitialized());
  REQUIRE(routing_info->getAdjacentLeft() == node_3_6);

  // update 3:2 as routing table neighbor child

  REQUIRE(!routing_info->getRoutingTableNeighborChildren()[0].isInitialized());
  REQUIRE(routing_info->getRoutingTableNeighborChildren()[0].getLogicalNodeInfo() ==
          node_3_2.getLogicalNodeInfo());

  MessageUpdateNeighbors msg3(
      header, {std::make_tuple(node_3_2, NeighborRelationship::kRoutingTableNeighborChild)});
  REQUIRE_NOTHROW(response_algo.process(msg3));

  REQUIRE(routing_info->getRoutingTableNeighborChildren()[0] == node_3_2);

  // update 2:1 as routing table neighbor

  REQUIRE(routing_info->getRoutingTableNeighbors()[0] == node_2_1);
  node_2_1.setAddress("6.6.6.6");

  MessageUpdateNeighbors msg4(
      header, {std::make_tuple(node_2_1, NeighborRelationship::kRoutingTableNeighbor)});
  REQUIRE_NOTHROW(response_algo.process(msg4));

  REQUIRE(routing_info->getRoutingTableNeighbors()[0] == node_2_1);
}

TEST_CASE("ResponseAlgorithmGeneral processGetNeighbors",
          "[ResponseAlgorithmGeneral][processGetNeighbors]") {
  uint16_t fanout = 2;

  NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);
  NodeInfo node_1_0(1, 0, fanout, "1.2.3.5", 2000);
  NodeInfo node_1_1(1, 1, fanout, "1.2.3.6", 2000);

  NodeInfo node_2_0(2, 0, fanout, "1.2.3.7", 2000);
  NodeInfo node_2_1(2, 1, fanout, "1.2.3.8", 2000);
  NodeInfo node_2_2(2, 2, fanout, "1.2.3.9", 2000);
  NodeInfo node_2_3(2, 3, fanout, "1.2.3.10", 2000);

  NodeInfo node_3_2(3, 2, fanout);
  NodeInfo node_3_4(3, 4, fanout, "1.3.3.10", 2000);
  NodeInfo node_3_5(3, 5, fanout, "1.4.3.10", 2000);
  NodeInfo node_3_6(3, 6, fanout, "1.4.5.10", 2000);
  NodeInfo node_3_7(3, 7, fanout, "1.5.3.10", 2000);

  auto access = std::make_shared<AccessContainer>();
  auto routing_info = std::make_shared<RoutingInformation>(node_2_3, Logger());
  access->routing_info = routing_info;

  NodeInfo sent_msg_sender;
  NodeInfo sent_msg_target;
  std::vector<NodeInfo> sent_msg_requested_neighbors;
  std::function<uint32_t(const MessageVariant &)> save_target =
      [&](const MessageVariant &msg) mutable {
        auto sent_msg = std::get_if<MessageInformAboutNeighbors>(&msg);

        sent_msg_sender = sent_msg->getSender();
        sent_msg_target = sent_msg->getTarget();
        sent_msg_requested_neighbors = sent_msg->getRequestedNeighbors();

        return 1;
      };
  access->send = save_target;

  ResponseAlgorithmGeneral response_algo(access);

  routing_info->setParent(node_1_1);
  routing_info->setAdjacentLeft(node_3_6);
  routing_info->setAdjacentRight(node_3_7);
  routing_info->updateNeighbor(node_3_6);
  routing_info->updateNeighbor(node_3_7);
  routing_info->updateRoutingTableNeighbor(node_2_1);
  routing_info->updateRoutingTableNeighbor(node_2_2);
  routing_info->updateRoutingTableNeighborChild(node_3_4);
  routing_info->updateRoutingTableNeighborChild(node_3_5);

  MinhtonMessageHeader header(node_3_7, node_2_3);
  MessageGetNeighbors msg1(header, node_1_1, {NeighborRelationship::kAdjacentLeft});
  REQUIRE_NOTHROW(response_algo.process(msg1));

  REQUIRE(sent_msg_target == node_1_1);
  REQUIRE(sent_msg_sender == node_2_3);
  REQUIRE(sent_msg_requested_neighbors.size() == 1);
  REQUIRE(sent_msg_requested_neighbors[0] == node_3_6);

  MessageGetNeighbors msg2(header, node_0_0, {NeighborRelationship::kAdjacentRight});
  REQUIRE_NOTHROW(response_algo.process(msg2));

  REQUIRE(sent_msg_target == node_0_0);
  REQUIRE(sent_msg_sender == node_2_3);
  REQUIRE(sent_msg_requested_neighbors.size() == 1);
  REQUIRE(sent_msg_requested_neighbors[0] == node_3_7);

  MessageGetNeighbors msg3(header, node_2_0, {NeighborRelationship::kChild});
  REQUIRE_NOTHROW(response_algo.process(msg3));

  REQUIRE(sent_msg_target == node_2_0);
  REQUIRE(sent_msg_sender == node_2_3);
  REQUIRE(sent_msg_requested_neighbors.size() == fanout);
  REQUIRE(((sent_msg_requested_neighbors[0] == node_3_7) ||
           (sent_msg_requested_neighbors[0] == node_3_6)));
  REQUIRE(((sent_msg_requested_neighbors[1] == node_3_7) ||
           (sent_msg_requested_neighbors[1] == node_3_6)));

  routing_info->removeNeighbor(node_3_6);

  MessageGetNeighbors msg4(header, node_2_0, {NeighborRelationship::kChild});
  REQUIRE_NOTHROW(response_algo.process(msg4));

  REQUIRE(sent_msg_target == node_2_0);
  REQUIRE(sent_msg_sender == node_2_3);
  REQUIRE(sent_msg_requested_neighbors.size() == 1);
  REQUIRE(sent_msg_requested_neighbors[0] == node_3_7);

  MinhtonMessageHeader header2(node_0_0, node_2_3);
  MessageGetNeighbors msg5(header2, node_2_2, {NeighborRelationship::kRoutingTableNeighbor});
  REQUIRE_NOTHROW(response_algo.process(msg5));

  REQUIRE(sent_msg_target == node_2_2);
  REQUIRE(sent_msg_sender == node_2_3);
  REQUIRE(sent_msg_requested_neighbors.size() == 2);
  REQUIRE(((sent_msg_requested_neighbors[0] == node_2_1) ||
           (sent_msg_requested_neighbors[0] == node_2_2)));
  REQUIRE(((sent_msg_requested_neighbors[1] == node_2_1) ||
           (sent_msg_requested_neighbors[1] == node_2_2)));

  MessageGetNeighbors msg6(header2, node_2_2, {NeighborRelationship::kRoutingTableNeighborChild});
  REQUIRE_NOTHROW(response_algo.process(msg6));

  REQUIRE(sent_msg_target == node_2_2);
  REQUIRE(sent_msg_sender == node_2_3);
  REQUIRE(sent_msg_requested_neighbors.size() == 2);
  REQUIRE(((sent_msg_requested_neighbors[0] == node_3_4) ||
           (sent_msg_requested_neighbors[0] == node_3_5)));
  REQUIRE(((sent_msg_requested_neighbors[1] == node_3_4) ||
           (sent_msg_requested_neighbors[1] == node_3_5)));

  sent_msg_sender = NodeInfo();
  sent_msg_target = NodeInfo();
  sent_msg_requested_neighbors = std::vector<NodeInfo>();

  routing_info->removeNeighbor(node_3_4);
  routing_info->removeNeighbor(node_3_5);

  MessageGetNeighbors msg7(header2, node_2_2, {NeighborRelationship::kRoutingTableNeighborChild});
  REQUIRE_NOTHROW(response_algo.process(msg7));

  REQUIRE(!sent_msg_sender.isValidPeer());
  REQUIRE(!sent_msg_sender.getNetworkInfo().isInitialized());
  REQUIRE(!sent_msg_target.isValidPeer());
  REQUIRE(!sent_msg_target.getNetworkInfo().isInitialized());
  REQUIRE(sent_msg_requested_neighbors.empty());
}

TEST_CASE("ResponseAlgorithmGeneral processRemoveNeighbor",
          "[ResponseAlgorithmGeneral][processRemoveNeighbor]") {
  uint16_t fanout = 2;

  NodeInfo node_0_0(0, 0, fanout, "1.2.3.4", 2000);
  NodeInfo node_1_0(1, 0, fanout, "1.2.3.5", 2000);
  NodeInfo node_1_1(1, 1, fanout, "1.2.3.6", 2000);

  NodeInfo node_2_0(2, 0, fanout, "1.2.3.7", 2000);
  NodeInfo node_2_1(2, 1, fanout, "1.2.3.8", 2000);
  NodeInfo node_2_2(2, 2, fanout, "1.2.3.9", 2000);
  NodeInfo node_2_3(2, 3, fanout, "1.2.3.10", 2000);

  NodeInfo node_3_2(3, 2, fanout);
  NodeInfo node_3_4(3, 4, fanout, "1.3.3.10", 2000);
  NodeInfo node_3_5(3, 5, fanout, "1.4.3.10", 2000);
  NodeInfo node_3_6(3, 6, fanout, "1.4.5.10", 2000);
  NodeInfo node_3_7(3, 7, fanout, "1.5.3.10", 2000);

  auto access = std::make_shared<AccessContainer>();
  auto routing_info = std::make_shared<RoutingInformation>(node_2_3, Logger());
  access->routing_info = routing_info;

  ResponseAlgorithmGeneral response_algo(access);

  // we are 2:3
  routing_info->setParent(node_1_1);
  routing_info->setAdjacentLeft(node_3_6);
  routing_info->setAdjacentRight(node_3_7);
  routing_info->updateNeighbor(node_3_6);
  routing_info->updateNeighbor(node_3_7);
  routing_info->updateRoutingTableNeighbor(node_2_1);
  routing_info->updateRoutingTableNeighbor(node_2_2);
  routing_info->updateRoutingTableNeighborChild(node_3_4);
  routing_info->updateRoutingTableNeighborChild(node_3_5);

  // remove

  REQUIRE(routing_info->getChild(0).isInitialized());
  REQUIRE(routing_info->getChild(0) == node_3_6);

  MinhtonMessageHeader header(node_0_0, node_1_1);
  MessageRemoveNeighbor msg1(header, node_3_6);
  REQUIRE_NOTHROW(response_algo.process(msg1));

  REQUIRE(!routing_info->getChild(0).isInitialized());
  REQUIRE(routing_info->getChild(0).getLogicalNodeInfo() == node_3_6.getLogicalNodeInfo());

  // remove

  REQUIRE(routing_info->getRoutingTableNeighbors()[0].isInitialized());
  REQUIRE(routing_info->getRoutingTableNeighbors()[0] == node_2_1);

  MessageRemoveNeighbor msg2(header, node_2_1);
  REQUIRE_NOTHROW(response_algo.process(msg2));

  REQUIRE(!routing_info->getRoutingTableNeighbors()[0].isInitialized());
  REQUIRE(routing_info->getRoutingTableNeighbors()[0].getLogicalNodeInfo() ==
          node_2_1.getLogicalNodeInfo());

  // cannot remove

  REQUIRE(routing_info->getParent().isInitialized());
  REQUIRE(routing_info->getParent() == node_1_1);

  MessageRemoveNeighbor msg3(header, node_1_1);
  REQUIRE_THROWS(response_algo.process(msg3));
}
