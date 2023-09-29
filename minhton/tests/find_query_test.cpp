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
}
