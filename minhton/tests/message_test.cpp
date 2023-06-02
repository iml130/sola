// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "message/message.h"

#include <catch2/catch_test_macros.hpp>

#include "message/types_all.h"

using namespace minhton;

TEST_CASE("MessageVariant JoinAcceptAck", "[MessageVariant][JoinAcceptAck]") {
  NodeInfo node1(1, 1, 3);
  NodeInfo node2(1, 1, 3, "1.2.3.4", 2000);

  MinhtonMessageHeader header(node1, node1);
  REQUIRE_THROWS(MessageJoinAcceptAck(header));

  header.setSender(node2);
  header.setTarget(node2);

  MessageJoinAcceptAck message(header);
  REQUIRE(message.getHeader().getMessageType() == MessageType::kJoinAcceptAck);
}

TEST_CASE("MessageVariant JoinAccept", "[MessageVariant][JoinAccept]") {
  NodeInfo node1(32, 4, 2);
  NodeInfo node2(32, 4, 2, "1.2.3.4", 2000);

  MinhtonMessageHeader header(node2, node2);

  // Invalid fanout
  REQUIRE_THROWS(MessageJoinAccept(header, 1, node2, node2, {}));

  MessageJoinAccept message(header, 2, node2, node2, {node2, node2});

  REQUIRE(message.getHeader().getMessageType() == MessageType::kJoinAccept);
  REQUIRE(message.getFanout() == 2);
  REQUIRE(message.getAdjacentLeft().getLevel() == 32);
  REQUIRE(message.getAdjacentRight().getLevel() == 32);
  REQUIRE(message.getRoutingTableNeighbors().at(0).getLevel() == 32);

  // at least one of adj left and right has to be initialized
  REQUIRE_THROWS(MessageJoinAccept(header, 2, node2, node2, {node1, node2}));

  REQUIRE_NOTHROW(MessageJoinAccept(header, 2, node2, node1, {node2, node2}));

  REQUIRE_THROWS(MessageJoinAccept(header, 2, node1, node1, {node2, node2}));
}

TEST_CASE("MessageVariant Join", "[MessageVariant][Join]") {
  NodeInfo node1(32, 4, 2);
  NodeInfo node2(32, 4, 2, "1.2.3.4", 2000);

  MinhtonMessageHeader header(node1, node1);
  REQUIRE_THROWS(MessageJoin(header, node1));
  REQUIRE_THROWS(MessageJoin(header, node2));

  header.setSender(node2);
  header.setTarget(node2);
  MessageJoin message(header, node2);

  REQUIRE(message.getEnteringNode().getLevel() == 32);
  REQUIRE(message.getSender().getLevel() == 32);
  REQUIRE(message.getTarget().getLevel() == 32);
  REQUIRE(message.getHeader().getMessageType() == MessageType::kJoin);
}

TEST_CASE("MessageVariant Empty", "[MessageVariant][Empty]") {
  NodeInfo node1(0, 0, 3, "1.2.3.4", 2000);
  MinhtonMessageHeader header(node1, node1);
  MessageEmpty message(header);
  REQUIRE(message.getHeader().getMessageType() == MessageType::kEmpty);
}

TEST_CASE("MessageVariant SearchExact", "[MessageVariant][SearchExact]") {
  NodeInfo node1(32, 4, 2);
  NodeInfo node2(32, 4, 2, "1.2.3.4", 2000);

  MinhtonMessageHeader join_header(node2, node2);
  auto test_query = std::make_shared<MessageSEVariant>(MessageJoin(join_header, node2));

  MinhtonMessageHeader header(node2, node2);
  MessageSearchExact message(header, node2, test_query);
  REQUIRE(message.getHeader().getMessageType() == MessageType::kSearchExact);

  MinhtonMessageHeader query_header = std::visit(
      [](auto &&msg) -> MinhtonMessageHeader { return msg.getHeader(); }, *message.getQuery());
  REQUIRE(query_header.getMessageType() == MessageType::kJoin);

  REQUIRE(message.getDestinationNode().getLevel() == 32);
  REQUIRE(message.getSender().getLevel() == 32);
  REQUIRE(message.getTarget().getLevel() == 32);

  header = MinhtonMessageHeader(node1, node2);
  REQUIRE_THROWS(MessageSearchExact(header, node2, test_query));

  header = MinhtonMessageHeader(node2, node2);
  REQUIRE_NOTHROW(MessageSearchExact(header, node1, test_query));
}

TEST_CASE("MessageVariant BootstrapDiscover", "[MessageVariant][BootstrapDiscover]") {
  MinhtonMessageHeader header(NodeInfo(2, 2, 3, "0.0.0.0", 1024),
                              NodeInfo(2, 2, 3, "0.0.0.0", 1024));
  MessageBootstrapDiscover message(header);
  REQUIRE(message.getHeader().getRefEventId() == 0);
  REQUIRE(message.getHeader().getMessageType() == MessageType::kBootstrapDiscover);

  message = MessageBootstrapDiscover(header, "TestDiscoveryMessage");
  REQUIRE(message.getDiscoveryMessage() == "TestDiscoveryMessage");

  header.setSender(NodeInfo(1, 2, 3));
  REQUIRE(header.getSender().getFanout() == 3);
  REQUIRE_FALSE(header.validate());

  header.setSender(NodeInfo(1, 2, 3, "1.2.3.4", 2001));
  REQUIRE_NOTHROW(message = MessageBootstrapDiscover(header));
  REQUIRE(message.getSender().getPort() == 2001);
}

TEST_CASE("MessageVariant BootstrapResponse", "[MessageVariant][BootstrapResponse]") {
  NodeInfo node1(5, 1, 2);
  NodeInfo node2(6, 1, 2, "1.2.3.4", 2000);

  MinhtonMessageHeader header(node2, node2);
  MessageBootstrapResponse message(header, node2);
  REQUIRE(message.getHeader().getRefEventId() == 0);
  REQUIRE(message.getHeader().getMessageType() == MessageType::kBootstrapResponse);

  header = MinhtonMessageHeader(node2, node2, 42);
  message = MessageBootstrapResponse(header, node2);
  REQUIRE(message.getHeader().getRefEventId() == 42);
  REQUIRE(message.getHeader().getMessageType() == MessageType::kBootstrapResponse);

  REQUIRE(message.getSender().getNumber() == 1);
  REQUIRE(message.getTarget().getLevel() == 6);
  REQUIRE(message.getNodeToJoin().getFanout() == 2);

  header = MinhtonMessageHeader(node1, node2, 42);
  REQUIRE_THROWS(MessageBootstrapResponse(header, node2));
}

TEST_CASE("MessageVariant FindReplacement", "[MessageVariant][FindReplacement]") {
  NodeInfo node1(5, 1, 2);
  NodeInfo node2(6, 1, 2, "1.2.3.4", 2000);

  MinhtonMessageHeader header(node2, node2);
  MessageFindReplacement msg(header, node2);

  REQUIRE(msg.getHeader().getRefEventId() == 0);
  REQUIRE(msg.getHeader().getMessageType() == MessageType::kFindReplacement);

  REQUIRE(msg.getSender().getNumber() == 1);
  REQUIRE(msg.getTarget().getLevel() == 6);
  REQUIRE(msg.getNodeToReplace().getFanout() == 2);

  header = MinhtonMessageHeader(node1, node2, 42);
  REQUIRE_THROWS(MessageFindReplacement(header, node2));

  header = MinhtonMessageHeader(node2, node2, 42);
  REQUIRE_THROWS(MessageFindReplacement(header, node1));
}

TEST_CASE("MessageVariant InformAboutNeighbors", "[MessageVariant][InformAboutNeighbors]") {
  // MessageInformAboutNeighbors msg;

  // REQUIRE(msg.getHeader().getRefEventId() == 0);
  // REQUIRE(msg.getMessageType() == MessageType::kInformAboutNeighbors);

  // NodeInfo node1(5, 1, 2);
  // NodeInfo node2(6, 1, 2, "1.2.3.4", 2000);

  // msg.setSender(node2);
  // msg.setTarget(node2);

  // REQUIRE_FALSE(msg.validate());

  // msg.addRequestedNeighbor(node1);

  // REQUIRE(msg.getSender().getLevel() == 6);
  // REQUIRE(msg.getTarget().getNumber() == 1);
  // REQUIRE(msg.getRequestedNeighbors()[0].getFanout() == 2);

  // REQUIRE(msg.validate());
}

TEST_CASE("MessageVariant RemoveNeighbor", "[MessageVariant][RemoveNeighbor]") {
  NodeInfo node1(3, 1, 2, "1.2.3.4", 2000);
  NodeInfo node2(6, 1, 2, "1.2.3.4", 2000);

  MinhtonMessageHeader header(node1, node2);
  MessageRemoveNeighbor msg(header, node2);

  REQUIRE(msg.getHeader().getRefEventId() == 0);
  REQUIRE(msg.getHeader().getMessageType() == MessageType::kRemoveNeighbor);

  REQUIRE(msg.getSender().getLevel() == 3);
  REQUIRE(msg.getTarget().getNumber() == 1);
  REQUIRE(msg.getRemovedPositionNode().getFanout() == 2);
}

TEST_CASE("MessageVariant GetNeighbors", "[MessageVariant][GetNeighbors]") {
  NodeInfo node1(5, 1, 2);
  NodeInfo node2(6, 1, 2, "1.2.3.4", 2013);

  MinhtonMessageHeader header(node1, node1);
  REQUIRE_THROWS(MessageGetNeighbors(header, NodeInfo(), {}));

  header = MinhtonMessageHeader(node2, node2);
  REQUIRE_THROWS(MessageGetNeighbors(header, node1, {}));

  std::vector<NeighborRelationship> rels = {NeighborRelationship::kAdjacentLeft};

  MessageGetNeighbors msg(header, node1, rels);

  REQUIRE(msg.getHeader().getRefEventId() == 0);
  REQUIRE(msg.getHeader().getMessageType() == MessageType::kGetNeighbors);
  REQUIRE(msg.getRelationships()[0] == NeighborRelationship::kAdjacentLeft);
  REQUIRE(msg.getSender().getAddress() == "1.2.3.4");
  REQUIRE(msg.getTarget().getPort() == 2013);
}

TEST_CASE("MessageVariant UpdateNeighbors", "[MessageVariant][UpdateNeighbors]") {
  NodeInfo node1(5, 13, 2);
  NodeInfo node2(7, 2, 5, "1.2.3.4", 2013);
  NodeInfo node3(1, 2, 3, "1.2.3.5", 2003);

  MinhtonMessageHeader header(node2, node2);

  REQUIRE_THROWS(MessageUpdateNeighbors(header, {}));

  MessageUpdateNeighbors msg(header, {std::make_tuple(node3, NeighborRelationship::kParent)});

  REQUIRE(msg.getHeader().getRefEventId() == 0);
  REQUIRE(msg.getHeader().getMessageType() == MessageType::kUpdateNeighbors);
  REQUIRE(std::get<0>(msg.getNeighborsToUpdate()[0]).getLevel() == 1);
  REQUIRE(std::get<1>(msg.getNeighborsToUpdate()[0]) == NeighborRelationship::kParent);
}

TEST_CASE("MessageVariant ReplacementOffer", "[MessageVariant][ReplacementOffer]") {
  NodeInfo node(6, 1, 2, "1.2.3.4", 2013);

  MinhtonMessageHeader header(node, node);
  MessageReplacementOffer msg(header);

  REQUIRE(msg.getHeader().getRefEventId() == 0);
  REQUIRE(msg.getHeader().getMessageType() == MessageType::kReplacementOffer);

  REQUIRE(msg.getSender().getAddress() == "1.2.3.4");
  REQUIRE(msg.getTarget().getLevel() == 6);
}

TEST_CASE("MessageVariant ReplacementAck", "[MessageVariant][ReplacementAck]") {
  NodeInfo node1(5, 1, 2);
  NodeInfo node2(6, 1, 2, "1.2.3.4", 2013);

  MinhtonMessageHeader header(node2, node2, 99);

  REQUIRE_THROWS(MessageReplacementAck(header, {node1, node2},
                                       MessageReplacementAck::LockedStates{true, true, false}));

  MessageReplacementAck msg(header, {node2, node2},
                            MessageReplacementAck::LockedStates{true, true, false});

  REQUIRE(msg.getHeader().getRefEventId() == 99);
  REQUIRE(msg.getHeader().getMessageType() == MessageType::kReplacementAck);

  auto [locked, locked_right, locked_left] = msg.getLockedStates();
  REQUIRE(locked);
  REQUIRE(locked_right);
  REQUIRE_FALSE(locked_left);

  REQUIRE(msg.getSender().getFanout() == 2);
  REQUIRE(msg.getTarget().getLevel() == 6);

  REQUIRE(msg.getNeighbors()[0].getFanout() == 2);
}

TEST_CASE("MessageVariant ReplacementUpdate", "[MessageVariant][ReplacementUpdate]") {
  NodeInfo node1(5, 1, 2);
  NodeInfo node2(6, 1, 2, "1.2.3.4", 2013);

  MinhtonMessageHeader header(node2, node2, 99);

  REQUIRE_THROWS(MessageReplacementUpdate(header, node1, node1, node2.getPeerInfo()));
  REQUIRE_THROWS(MessageReplacementUpdate(header, node2, node1, node2.getPeerInfo()));

  MessageReplacementUpdate msg(header, node2, node2, node2.getPeerInfo());

  REQUIRE(msg.getHeader().getRefEventId() == 99);
  REQUIRE(msg.getHeader().getMessageType() == MessageType::kReplacementUpdate);
  REQUIRE(msg.getSender().getLevel() == 6);
  REQUIRE(msg.getTarget().getLevel() == 6);
  REQUIRE(msg.getRemovedPositionNode().getLevel() == 6);
  REQUIRE(msg.getReplacedPositionNode().getLevel() == 6);
}
