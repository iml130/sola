// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "utils/procedure_info.h"

#include <catch2/catch_test_macros.hpp>

#include "core/node_info.h"
#include "exception/algorithm_exception.h"
using namespace minhton;

TEST_CASE("ProcedureInfo", "[ProcedureInfo]") {
  uint16_t fanout = 5;
  ProcedureInfo info;

  minhton::NodeInfo node_a(1, 2, fanout);
  minhton::NodeInfo node_b(1, 3, fanout);
  minhton::NodeInfo node_c(1, 4, fanout);

  REQUIRE_FALSE(info.hasKey(ProcedureKey::kAcceptChildProcedure));
  REQUIRE_FALSE(info.hasKey(ProcedureKey::kFindReplacementProcedure));

  auto v1 = {node_a, node_b};
  info.save(ProcedureKey::kAcceptChildProcedure, v1);
  REQUIRE(info.load(ProcedureKey::kAcceptChildProcedure)[0] == node_a);
  REQUIRE(info.load(ProcedureKey::kAcceptChildProcedure)[1] == node_b);

  auto v2 = {node_b, node_c, node_a};
  info.update(ProcedureKey::kAcceptChildProcedure, v2);
  REQUIRE(info.load(ProcedureKey::kAcceptChildProcedure).size() == 3);
  REQUIRE_THROWS_AS(info.update(ProcedureKey::kFindReplacementProcedure, v2), AlgorithmException);

  REQUIRE(info.hasKey(ProcedureKey::kAcceptChildProcedure));
  REQUIRE_FALSE(info.hasKey(ProcedureKey::kFindReplacementProcedure));
  REQUIRE_THROWS_AS(info.save(ProcedureKey::kAcceptChildProcedure, v1), AlgorithmException);

  REQUIRE_THROWS_AS(info.remove(ProcedureKey::kFindReplacementProcedure), AlgorithmException);
  REQUIRE_NOTHROW(info.remove(ProcedureKey::kAcceptChildProcedure));
  REQUIRE_THROWS_AS(info.load(ProcedureKey::kAcceptChildProcedure), AlgorithmException);

  REQUIRE_FALSE(info.hasKey(ProcedureKey::kAcceptChildProcedure));
  REQUIRE_FALSE(info.hasKey(ProcedureKey::kFindReplacementProcedure));
}
