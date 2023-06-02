// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "algorithms/esearch/find_query.h"

#include <catch2/catch_test_macros.hpp>

using namespace minhton;

TEST_CASE("FindQuery Constructor", "[FindQuery][Init]") {
  FindQuery q1;
  REQUIRE(q1.getBooleanExpression() == nullptr);
  REQUIRE(q1.getScope() == FindQuery::FindQueryScope::kAll);
  REQUIRE(q1.getInquireOutdatedAttributes());
  REQUIRE(q1.getInquireUnknownAttributes());

  q1.setRequestingNode(NodeInfo(3, 4, (uint16_t)7));
  REQUIRE(q1.getRequestingNode().getFanout() == 7);

  q1.setValidityThreshold(1234);
  REQUIRE(q1.getValidityThreshold() == 1234);

  auto expr1 = std::make_shared<PresenceExpression>("huhu");
  q1.setBooleanExpression(expr1);
  auto keys1 = q1.getBooleanExpression()->getRelevantKeys();
  REQUIRE(keys1.size() == 1);
  REQUIRE(keys1[0] == "huhu");

  // FindQuery q2("( HAS sugar )", "some");
  // REQUIRE(q2.getScope() == FindQuery::FindQueryScope::kSome);
  // REQUIRE(q2.getInquireOutdatedAttributes());
  // REQUIRE(q2.getInquireUnknownAttributes());

  // auto keys2 = q2.getBooleanExpression()->getRelevantKeys();
  // REQUIRE(keys2.size() == 2);
  // REQUIRE(((keys2[0] == "energy") || (keys2[0] == "sugar")));
  // REQUIRE(((keys2[1] == "energy") || (keys2[1] == "sugar")));

  // q2.setInquireOutdatedAttributes(false);
  // REQUIRE(q2.getInquireOutdatedAttributes());

  // q2.setInquireUnknownAttributes(false);
  // REQUIRE(q2.getInquireUnknownAttributes());
}

// TEST_CASE("FindQuery Serialize/Deserialize", "[FindQuery][Serialize]") {

//     std::string s1 = "( HAS cake )";
//     std::string s2 = "( ( HAS sugar ) OR ( HAS cookies ) )";
//     std::string s3 = "( ( HAS sugar ) AND ( HAS cookies ) )";
//     std::string s4 = "( ( NOT ( cakesize < 42 ) ) AND ( HAS cookies ) )";

//     // FindQuery q1(s1, "some");

// }
