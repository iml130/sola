// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

#include "exception/algorithm_exception.h"
#include "message/types_all.h"
#include "utils/serializer_cereal.h"

using namespace minhton;

// Compares the node infos \p n1 and \p n2 by level, number, address and port
void compare_node_info(const NodeInfo &n1, const NodeInfo &n2) {
  REQUIRE(n1.getLevel() == n2.getLevel());
  REQUIRE(n1.getNumber() == n2.getNumber());
  REQUIRE(n1.getAddress() == n2.getAddress());
  REQUIRE(n1.getPort() == n2.getPort());
}

// Checks for a specific test case if the message inside a search exact message (the query) is
// correct. Simulates the message passing from the MINHTON structure
void check_se_repl_ack_query(const MessageVariant &msg, const NodeInfo &destination_node,
                             const std::vector<minhton::NodeInfo> &neighbors) {
  auto se_msg = std::get<MessageSearchExact>(msg);

  // Message specific attributes
  auto query_var = std::get<MessageReplacementAck>(*se_msg.getQuery());
  assert(query_var.getHeader().getMessageType() == MessageType::kReplacementAck);
  compare_node_info(se_msg.getDestinationNode(), destination_node);
  compare_node_info(query_var.getNeighbors()[0], neighbors[0]);
  compare_node_info(query_var.getNeighbors()[3], neighbors[3]);
}

// Serialized and deserialized message and checks general message attributes
template <class T> T serialize_deserialize_message_and_check(const MessageVariant &msg) {
  serializer::SerializerCereal serializer;
  std::string serialized_msg = serializer.serialize(msg);
  auto deserialized_msg_variant = serializer.deserialize(serialized_msg);

  T deserialized_msg = std::get<T>(deserialized_msg_variant);
  T actual_msg = std::get<T>(msg);

  // Check general attributes
  REQUIRE(deserialized_msg.getHeader().getMessageType() == actual_msg.getHeader().getMessageType());
  REQUIRE(deserialized_msg.getHeader().getEventId() == actual_msg.getHeader().getEventId());
  REQUIRE(deserialized_msg.getHeader().getRefEventId() == actual_msg.getHeader().getRefEventId());
  compare_node_info(deserialized_msg.getSender(), actual_msg.getSender());
  compare_node_info(deserialized_msg.getTarget(), actual_msg.getTarget());

  return deserialized_msg;
}

TEST_CASE("JsonSerializer Serialize/Deserialize", "[JsonSerializer][Serialize/Deserialize]") {
  SECTION("Serialize/Deserialize BootstrapDiscover", "Serialize/Deserialize BootstrapDiscover") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 2, 3, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(2, 2, 3, "127.0.0.2", 4321));
    MessageBootstrapDiscover msg(header, "TestMessage");
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageBootstrapDiscover>(msg);

    // Message specific attributes
    REQUIRE(deserialized_msg.getDiscoveryMessage() == msg.getDiscoveryMessage());
  }

  SECTION("Serialize/Deserialize BootstrapResponse", "Serialize/Deserialize BootstrapResponse") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(2, 4, 4, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(2, 8, 4, "127.0.0.2", 4321));
    MessageBootstrapResponse msg(header, NodeInfo(2, 1, 4, "127.0.0.4", 1234));
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageBootstrapResponse>(msg);

    // Message specific attributes
    compare_node_info(deserialized_msg.getNodeToJoin(), msg.getNodeToJoin());
  }

  SECTION("Serialize/Deserialize Empty", "Serialize/Deserialize Empty") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 2, 4, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(2, 9, 4, "127.0.0.2", 4321));
    MessageEmpty msg(header);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageEmpty>(msg);
  }

  SECTION("Serialize/Deserialize FindReplacement", "Serialize/Deserialize FindReplacement") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 0, 4, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(1, 3, 4, "127.0.0.2", 4321));
    NodeInfo node_info(2, 13, 4, "127.0.0.4", 4321);
    MessageFindReplacement msg(header, node_info);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageFindReplacement>(msg);

    // Message specific attributes
    compare_node_info(deserialized_msg.getNodeToReplace(), msg.getNodeToReplace());
    REQUIRE(deserialized_msg.getSender().getFanout() == msg.getSender().getFanout());
    REQUIRE(deserialized_msg.getSearchProgress() == msg.getSearchProgress());
  }

  SECTION("Serialize/Deserialize FindReplacement 2", "Serialize/Deserialize FindReplacement") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 6, 7, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(1, 0, 7, "127.0.0.2", 4321));
    NodeInfo node_info(2, 17, 7, "127.0.0.7", 1234);
    MessageFindReplacement msg(header, node_info, SearchProgress::kSearchLeft);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageFindReplacement>(msg);

    // Message specific attributes
    compare_node_info(deserialized_msg.getNodeToReplace(), msg.getNodeToReplace());
    REQUIRE(deserialized_msg.getNodeToReplace().getFanout() == msg.getNodeToReplace().getFanout());
    REQUIRE(deserialized_msg.getSearchProgress() == msg.getSearchProgress());
  }

  SECTION("Serialize/Deserialize GetNeighbors", "Serialize/Deserialize GetNeighbors") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(2, 10, 4, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(3, 13, 4, "127.0.0.2", 4321));
    std::vector<NeighborRelationship> relationships = {kParent, kAdjacentLeft, kChild};
    MessageGetNeighbors msg(header, NodeInfo(), relationships);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageGetNeighbors>(msg);

    // Message specific attributes
    REQUIRE(deserialized_msg.getRelationships() == msg.getRelationships());
  }

  SECTION("Serialize/Deserialize InformAboutNeighbors",
          "Serialize/Deserialize InformAboutNeighbors") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(2, 0, 4, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(2, 1, 4, "127.0.0.2", 4321));
    std::vector<NodeInfo> requested_neighbors = {NodeInfo(2, 4, 4), NodeInfo(2, 6, 4)};
    MessageInformAboutNeighbors msg(header, requested_neighbors);
    auto deserialized_msg =
        serialize_deserialize_message_and_check<MessageInformAboutNeighbors>(msg);

    // Message specific attributes
    for (size_t i = 0; i < msg.getRequestedNeighbors().size(); i++) {
      compare_node_info(deserialized_msg.getRequestedNeighbors()[i],
                        msg.getRequestedNeighbors()[i]);
    }
  }

  SECTION("Serialize/Deserialize Join", "Serialize/Deserialize Join") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 1, 2, "127.0.0.1", 2000));
    header.setTarget(NodeInfo(2, 1, 2, "127.0.0.2", 2001));
    MessageJoin msg(header, NodeInfo(2, 2, 2, "127.0.0.3", 2002));
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageJoin>(msg);

    // Message specific attributes
    compare_node_info(deserialized_msg.getEnteringNode(), msg.getEnteringNode());
  }

  SECTION("Serialize/Deserialize JoinAccept", "Serialize/Deserialize JoinAccept") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 2, 3, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(2, 2, 3, "127.0.0.2", 2001));
    uint16_t fanout = 3;
    NodeInfo adj_left(1, 1, 3, "127.0.0.13", 4321);
    NodeInfo adj_right(1, 0, 3, "127.0.0.3", 4321);
    std::vector<NodeInfo> neighbors = {NodeInfo(1, 2, 3, "127.0.0.12", 4321),
                                       NodeInfo(3, 3, 3, "127.0.0.33", 4321)};
    MessageJoinAccept msg(header, fanout, adj_left, adj_right, neighbors);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageJoinAccept>(msg);

    // Message specific attributes
    REQUIRE(deserialized_msg.getFanout() == msg.getFanout());
    compare_node_info(deserialized_msg.getAdjacentLeft(), msg.getAdjacentLeft());
    compare_node_info(deserialized_msg.getAdjacentRight(), msg.getAdjacentRight());
    for (size_t i = 0; i < msg.getRoutingTableNeighbors().size(); i++) {
      compare_node_info(deserialized_msg.getRoutingTableNeighbors()[i],
                        msg.getRoutingTableNeighbors()[i]);
    }
  }

  SECTION("Serialize/Deserialize JoinAcceptAck", "Serialize/Deserialize JoinAcceptAck") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(4, 4, 4, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(5, 4, 4, "127.0.0.2", 2001));
    MessageJoinAcceptAck msg(header);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageJoinAcceptAck>(msg);
  }

  SECTION("Serialize/Deserialize RemoveNeighbor", "Serialize/Deserialize RemoveNeighbor") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 1, 2, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(1, 0, 2, "127.0.0.2", 2001));
    MessageRemoveNeighbor msg(header, NodeInfo(2, 0, 2, "127.0.0.20", 1220));
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageRemoveNeighbor>(msg);

    // Message specific attributes
    compare_node_info(deserialized_msg.getRemovedPositionNode(), msg.getRemovedPositionNode());
  }

  SECTION("Serialize/Deserialize ReplacementAck", "Serialize/Deserialize ReplacementAck") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 1, 5, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(4, 5, 5, "127.0.0.2", 2001));
    std::vector<NodeInfo> neighbors = {NodeInfo(2, 2, 5, "127.0.0.22", 4321),
                                       NodeInfo(2, 3, 5, "127.0.0.23", 4321)};
    MessageReplacementAck::LockedStates locked_states{true, false, true};
    MessageReplacementAck msg(header, neighbors, locked_states);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageReplacementAck>(msg);

    // Message specific attributes
    for (size_t i = 0; i < msg.getNeighbors().size(); i++) {
      compare_node_info(deserialized_msg.getNeighbors()[i], msg.getNeighbors()[i]);
    }
  }

  SECTION("Serialize/Deserialize ReplacementOffer", "Serialize/Deserialize ReplacementOffer") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 1, 2, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(1, 0, 2, "127.0.0.2", 2001));
    MessageReplacementOffer msg(header);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageReplacementOffer>(msg);
  }

  SECTION("Serialize/Deserialize ReplacementUpdate", "Serialize/Deserialize ReplacementUpdate") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(3, 2, 5, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(3, 1, 5, "127.0.0.2", 2001));
    NodeInfo rem_pos_node(1, 2, 5, "127.0.0.12", 4321);
    NodeInfo repl_pos_node(1, 1, 5, "127.0.0.11", 4321);
    PeerInfo new_peer(3, 2, 5);
    MessageReplacementUpdate msg(header, rem_pos_node, repl_pos_node, new_peer);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageReplacementUpdate>(msg);

    // Message specific attributes
    compare_node_info(deserialized_msg.getRemovedPositionNode(), msg.getRemovedPositionNode());
    compare_node_info(deserialized_msg.getReplacedPositionNode(), msg.getReplacedPositionNode());
  }

  SECTION("Serialize/Deserialize SearchExact", "Serialize/Deserialize SearchExact") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(4, 5, 6, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(1, 2, 6, "127.0.0.2", 2001));
    NodeInfo dest_node(4, 4, 6, "127.0.0.44", 1044);

    MinhtonMessageHeader query_header;
    query_header.setSender(NodeInfo(4, 5, 6, "127.0.0.45", 1234));
    query_header.setTarget(NodeInfo(1, 2, 6));
    std::vector<minhton::NodeInfo> neighbors = {
        NodeInfo(4, 0, 6, "127.0.0.40", 4321), NodeInfo(4, 1, 6, "127.0.0.41", 4321),
        NodeInfo(4, 2, 6, "127.0.0.42", 4321), NodeInfo(4, 3, 6, "127.0.0.43", 4321)};
    MessageReplacementAck::LockedStates locked_states{false, true, false};
    auto query_msg = MessageReplacementAck(query_header, neighbors, locked_states);

    MessageSearchExact msg(header, dest_node, std::make_shared<MessageSEVariant>(query_msg));
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageSearchExact>(msg);

    // Message specific attributes
    check_se_repl_ack_query(deserialized_msg, msg.getDestinationNode(), neighbors);
  }

  SECTION("Serialize/Deserialize UpdateNeighbors", "Serialize/Deserialize UpdateNeighbors") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 4, 7, "127.0.0.1", 1234));
    header.setTarget(NodeInfo(1, 5, 7, "127.0.0.2", 2001));
    std::vector<std::tuple<minhton::NodeInfo, minhton::NeighborRelationship>> relationship;
    relationship.push_back(std::make_tuple<>(NodeInfo(1, 3, 7), kParent));
    MessageUpdateNeighbors msg(header, relationship);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageUpdateNeighbors>(msg);

    // Message specific attributes
    for (size_t i = 0; i < relationship.size(); i++) {
      // Node Info
      compare_node_info(std::get<0>(deserialized_msg.getNeighborsToUpdate()[i]),
                        std::get<0>(msg.getNeighborsToUpdate()[i]));
      // Neighbor Relationship
      REQUIRE(std::get<1>(deserialized_msg.getNeighborsToUpdate()[i]) ==
              std::get<1>(msg.getNeighborsToUpdate()[i]));
    }
  }

  SECTION("Serialize/Deserialize MessageSignOffParentRequest",
          "Serialize/Deserialize MessageSignOffParentRequest") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 1, 2, "127.0.0.1", 2000));
    header.setTarget(NodeInfo(2, 1, 2, "127.0.0.2", 2001));
    MessageSignoffParentRequest msg(header);
    auto deserialized_msg =
        serialize_deserialize_message_and_check<MessageSignoffParentRequest>(msg);

    // Message specific attributes
    compare_node_info(deserialized_msg.getTarget(), msg.getTarget());
  }

  SECTION("Serialize/Deserialize MessageFindQueryRequest",
          "Serialize/Deserialize MessageFindQueryRequest") {
    MinhtonMessageHeader header;
    header.setSender(NodeInfo(1, 1, 2, "127.0.0.1", 2000));
    header.setTarget(NodeInfo(2, 1, 2, "127.0.0.2", 2001));
    minhton::FindQuery query;
    query.setBooleanExpression(std::make_shared<EmptyExpression>(EmptyExpression()));
    query.setScope(minhton::FindQuery::FindQueryScope::kAll);
    query.setValidityThreshold(16);

    auto interval = std::make_pair<uint32_t, uint32_t>(256, 1);
    MessageFindQueryRequest msg(
        header, query, MessageFindQueryRequest::ForwardingDirection::kDirectionRight, interval);
    auto deserialized_msg = serialize_deserialize_message_and_check<MessageFindQueryRequest>(msg);

    // Message specific attributes
    compare_node_info(deserialized_msg.getTarget(), msg.getTarget());
    REQUIRE(deserialized_msg.getForwardingDirection() == msg.getForwardingDirection());
    REQUIRE(deserialized_msg.getInterval().first == msg.getInterval().first);
    REQUIRE(deserialized_msg.getInterval().second == msg.getInterval().second);
  }
}
