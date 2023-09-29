// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "core/routing_calculations.h"

#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "core/constants.h"
using namespace minhton;
TEST_CASE("RoutingCalculations Sweep Direction", "[RoutingCalculations][SweepDirection]") {
  REQUIRE(getFillLevelRightToLeft(0));
  REQUIRE_FALSE(getFillLevelRightToLeft(1));
  REQUIRE(getFillLevelRightToLeft(2));
  REQUIRE_FALSE(getFillLevelRightToLeft(3));
  REQUIRE(getFillLevelRightToLeft(4));

  // Boundary Check
  REQUIRE(getFillLevelRightToLeft(300));
  REQUIRE_FALSE(getFillLevelRightToLeft(301));
  REQUIRE_FALSE(getFillLevelRightToLeft(65535));
}

TEST_CASE("RoutingCalculations calcParent", "[RoutingCalculations][calcParent]") {
  REQUIRE(calcParent(1, 0, 2) == std::make_tuple(0, 0));
  REQUIRE(calcParent(1, 1, 2) == std::make_tuple(0, 0));

  REQUIRE(calcParent(2, 0, 2) == std::make_tuple(1, 0));
  REQUIRE(calcParent(2, 1, 2) == std::make_tuple(1, 0));
  REQUIRE(calcParent(2, 2, 2) == std::make_tuple(1, 1));
  REQUIRE(calcParent(2, 3, 2) == std::make_tuple(1, 1));

  REQUIRE(calcParent(1, 2, 3) == std::make_tuple(0, 0));

  // Exceptions Check
  CHECK_THROWS(calcParent(0, 0, 3));
  CHECK_THROWS(calcParent(1, 0, 0));
  CHECK_THROWS(calcParent(1, 5, 2));
  CHECK_THROWS(calcParent(1, 2, 2));
  CHECK_THROWS(calcParent(1, 3, 3));
  CHECK_THROWS(calcParent(2, 4, 2));
  CHECK_THROWS(calcParent(2, 9, 3));
  CHECK_THROWS(calcParent(1, 65535, 255));
}

TEST_CASE("RoutingCalculations calcChildren", "[RoutingCalculations][calcChildren]") {
  REQUIRE(calcChildren(0, 0, 2) == std::vector<std::tuple<uint32_t, uint32_t>>{
                                       std::make_tuple(1, 0), std::make_tuple(1, 1)});
  REQUIRE(calcChildren(0, 0, 3) ==
          std::vector<std::tuple<uint32_t, uint32_t>>{std::make_tuple(1, 0), std::make_tuple(1, 1),
                                                      std::make_tuple(1, 2)});
  REQUIRE(calcChildren(1, 0, 3) ==
          std::vector<std::tuple<uint32_t, uint32_t>>{std::make_tuple(2, 0), std::make_tuple(2, 1),
                                                      std::make_tuple(2, 2)});
  REQUIRE(calcChildren(2, 3, 3) ==
          std::vector<std::tuple<uint32_t, uint32_t>>{std::make_tuple(3, 9), std::make_tuple(3, 10),
                                                      std::make_tuple(3, 11)});

  // Boundaries Check
  REQUIRE_NOTHROW(calcChildren(65534, 256, 255));
  REQUIRE_NOTHROW(calcChildren(2, 256, 255));
  CHECK_THROWS(calcChildren(2, 3, 0));  // Fanout cannot be 0

  // Non-Existent Nodes
  CHECK_THROWS(calcChildren(1, 5, 2));
  CHECK_THROWS(calcChildren(1, 2, 2));
  CHECK_THROWS(calcChildren(1, 3, 3));
  CHECK_THROWS(calcChildren(2, 4, 2));
  CHECK_THROWS(calcChildren(2, 9, 3));
  CHECK_THROWS(calcChildren(0, 1, 3));
}

TEST_CASE("RoutingCalculations calcRoutingSequence", "[RoutingCalculations][calcRoutingSequence]") {
  REQUIRE(calcRoutingSequence(0, 2) == std::vector<uint32_t>{});
  REQUIRE(calcRoutingSequence(0, 3) == std::vector<uint32_t>{});
  REQUIRE(calcRoutingSequence(0, 13) == std::vector<uint32_t>{});

  // Result Check
  REQUIRE(calcRoutingSequence(2, 2) == std::vector<uint32_t>{1, 2});
  REQUIRE(calcRoutingSequence(2, 3) == std::vector<uint32_t>{1, 2, 3, 6, 9, 18, 27, 54});
  REQUIRE(calcRoutingSequence(1, 3) == std::vector<uint32_t>{1, 2, 3, 6});
  REQUIRE(calcRoutingSequence(2, 4) == std::vector<uint32_t>{1, 2, 3, 4, 8, 12, 16, 32, 48, 64, 128,
                                                             192, 256, 512, 768, 1024, 2048, 3072});

  // Boundaries Check
  REQUIRE(calcRoutingSequence(0, 255) == std::vector<uint32_t>{});
  CHECK_THROWS(calcRoutingSequence(0, 0));
}

TEST_CASE("RoutingCalculations calcLeftRT", "[RoutingCalculations][calcLeftRT]") {
  REQUIRE(calcLeftRT(0, 0, 2) == std::vector<std::tuple<uint32_t, uint32_t>>{});
  REQUIRE(calcLeftRT(0, 0, 5) == std::vector<std::tuple<uint32_t, uint32_t>>{});
  REQUIRE(calcLeftRT(42, 0, 3) == std::vector<std::tuple<uint32_t, uint32_t>>{});
  REQUIRE(calcLeftRT(100, 0, 3) == std::vector<std::tuple<uint32_t, uint32_t>>{});

  REQUIRE(calcLeftRT(1, 1, 2) ==
          std::vector<std::tuple<uint32_t, uint32_t>>{std::make_tuple(1, 0)});
  REQUIRE(calcLeftRT(1, 1, 13) ==
          std::vector<std::tuple<uint32_t, uint32_t>>{std::make_tuple(1, 0)});
  REQUIRE(calcLeftRT(2, 3, 2) == std::vector<std::tuple<uint32_t, uint32_t>>{
                                     std::make_tuple(2, 2), std::make_tuple(2, 1)});

  REQUIRE(calcLeftRT(1, 4, 5) == std::vector<std::tuple<uint32_t, uint32_t>>{
                                     std::make_tuple(1, 3), std::make_tuple(1, 2),
                                     std::make_tuple(1, 1), std::make_tuple(1, 0)});

  // Boundaries Check
  REQUIRE(calcLeftRT(0, 0, 255) == std::vector<std::tuple<uint32_t, uint32_t>>{});
  REQUIRE(calcLeftRT(1, 5, 255) ==
          std::vector<std::tuple<uint32_t, uint32_t>>{std::make_tuple(1, 4), std::make_tuple(1, 3),
                                                      std::make_tuple(1, 2), std::make_tuple(1, 1),
                                                      std::make_tuple(1, 0)});
  CHECK_THROWS(calcLeftRT(1, 234, 0));

  // Not Existing Nodes
  CHECK_THROWS(calcLeftRT(1, 65535, 255));
  CHECK_THROWS(calcLeftRT(0, 1, 2));
}

TEST_CASE("RoutingCalculations calcRightRT", "[RoutingCalculations][calcRightRT]") {
  REQUIRE(calcRightRT(0, 0, 2) == std::vector<std::tuple<uint32_t, uint32_t>>{});
  REQUIRE(calcRightRT(0, 0, 5) == std::vector<std::tuple<uint32_t, uint32_t>>{});
  REQUIRE(calcRightRT(5, 242, 3) == std::vector<std::tuple<uint32_t, uint32_t>>{});

  REQUIRE(calcRightRT(1, 0, 2) ==
          std::vector<std::tuple<uint32_t, uint32_t>>{std::make_tuple(1, 1)});
  REQUIRE(calcRightRT(1, 11, 13) ==
          std::vector<std::tuple<uint32_t, uint32_t>>{std::make_tuple(1, 12)});
  REQUIRE(calcRightRT(2, 0, 2) == std::vector<std::tuple<uint32_t, uint32_t>>{
                                      std::make_tuple(2, 1), std::make_tuple(2, 2)});
  REQUIRE(calcRightRT(1, 0, 5) == std::vector<std::tuple<uint32_t, uint32_t>>{
                                      std::make_tuple(1, 1), std::make_tuple(1, 2),
                                      std::make_tuple(1, 3), std::make_tuple(1, 4)});

  // Boundaries Check
  REQUIRE(calcRightRT(0, 0, 255) == std::vector<std::tuple<uint32_t, uint32_t>>{});
  REQUIRE(calcRightRT(1, 250, 255) == std::vector<std::tuple<uint32_t, uint32_t>>{
                                          std::make_tuple(1, 251), std::make_tuple(1, 252),
                                          std::make_tuple(1, 253), std::make_tuple(1, 254)});
  CHECK_THROWS(calcRightRT(1, 321, 0));

  // Overflow
  CHECK_THROWS(calcRightRT(6, 16620, 5));  // Overflow in Addition

  // Not Existing Nodes
  CHECK_THROWS(calcLeftRT(1, 65535, 255));
  CHECK_THROWS(calcLeftRT(0, 1, 2));
}

TEST_CASE("RoutingCalculations calcPrioSet", "[RoutingCalculations][calcPrioSet]") {
  // (level, fanout)
  std::vector<std::tuple<uint32_t, uint16_t>> tests = {
      std::make_tuple(0, 2),  std::make_tuple(1, 2), std::make_tuple(2, 2), std::make_tuple(4, 2),
      std::make_tuple(10, 2), std::make_tuple(4, 3), std::make_tuple(5, 3), std::make_tuple(2, 3),
      std::make_tuple(6, 5),  std::make_tuple(5, 5), std::make_tuple(4, 6), std::make_tuple(3, 10),
      std::make_tuple(4, 7)};

  for (auto const &test : tests) {
    uint32_t level = std::get<0>(test);
    uint16_t fanout = std::get<1>(test);
    uint16_t max_num = pow(fanout, level);

    auto prio_set = calcPrioSet(level, fanout);
    auto covered = std::set<uint32_t>();

    for (auto const &prio_num : prio_set) {
      covered.insert(prio_num);

      for (auto const &left_rt : calcLeftRT(level, prio_num, fanout)) {
        covered.insert(std::get<1>(left_rt));
      }

      for (auto const &right_rt : calcRightRT(level, prio_num, fanout)) {
        covered.insert(std::get<1>(right_rt));
      }
    }

    // TODO is connected

    // every node covered
    REQUIRE(covered.size() == max_num);
    REQUIRE(*covered.rbegin() == max_num - 1);

    // no prio out of level range
    REQUIRE(*prio_set.rbegin() < max_num);
  }
}

TEST_CASE("RoutingCalculations TreeMapper Fanout 2",
          "[RoutingCalculations][TreeMapper][Fanout][2]") {
  uint16_t fanout = 2;
  double K = 100;

  double lastValue;
  double curValue;

  lastValue = treeMapper(3, 0, fanout, K);

  curValue = treeMapper(2, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(0, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 4, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 5, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 6, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 7, fanout, K);
  REQUIRE(lastValue < curValue);
}

TEST_CASE("RoutingCalculations TreeMapper Fanout 3",
          "[RoutingCalculations][TreeMapper][Fanout][3]") {
  uint16_t fanout = 3;
  double K = 100;

  double lastValue;
  double curValue;

  lastValue = treeMapper(3, 0, fanout, K);

  curValue = treeMapper(3, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 4, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 5, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 6, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 7, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 8, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 9, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 10, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 11, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 12, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 13, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 4, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 14, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 15, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 16, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 5, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 17, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(0, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 18, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 19, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 6, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 20, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 21, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 22, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 7, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 23, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 24, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 25, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 8, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 26, fanout, K);
  REQUIRE(lastValue < curValue);

  // ------------- testing fanout 3 edge case ---------------
  // 3:14 is the critical point

  lastValue = treeMapper(4, 43, fanout, K);

  curValue = treeMapper(6, 393, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(6, 394, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(5, 131, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(6, 395, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 14, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(6, 396, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(6, 397, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(5, 132, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(6, 398, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(5, 133, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(4, 44, fanout, K);
  REQUIRE(lastValue < curValue);
}

TEST_CASE("RoutingCalculations TreeMapper Fanout 4",
          "[RoutingCalculations][TreeMapper][Fanout][4]") {
  uint16_t fanout = 4;
  double K = 100;

  double lastValue;
  double curValue;

  lastValue = treeMapper(2, 0, fanout, K);

  curValue = treeMapper(2, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 4, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 5, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 24, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 25, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 6, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 26, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 27, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 7, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(0, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 8, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 9, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 10, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 11, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 12, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 13, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 14, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 15, fanout, K);
  REQUIRE(lastValue < curValue);
}

TEST_CASE("RoutingCalculations TreeMapper Fanout 5", "[RoutingCalculations][TreeMapper]") {
  uint16_t fanout = 5;
  double K = 100;

  double lastValue;
  double curValue;

  lastValue = treeMapper(3, 0, fanout, K);

  curValue = treeMapper(3, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 4, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 4, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 5, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 6, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 7, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 8, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 9, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 10, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 11, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 12, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 13, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 14, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(0, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 15, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 16, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 17, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 3, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 18, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 19, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 20, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 21, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 22, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 4, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 23, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 24, fanout, K);
  REQUIRE(lastValue < curValue);
}

TEST_CASE("RoutingCalculations TreeMapper Fanout 6",
          "[RoutingCalculations][TreeMapper][Fanout][6]") {
  uint16_t fanout = 6;
  double K = 100;

  double lastValue;
  double curValue;

  lastValue = treeMapper(1, 0, fanout, K);

  curValue = treeMapper(3, 36, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 37, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 38, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 6, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 39, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 40, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(3, 41, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 7, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 8, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 1, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 9, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 10, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(2, 11, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 2, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(0, 0, fanout, K);
  REQUIRE(lastValue < curValue);
  lastValue = curValue;

  curValue = treeMapper(1, 3, fanout, K);
  REQUIRE(lastValue < curValue);
}

TEST_CASE("RoutingCalculations TreeMapper Invalid Levels/Numbers",
          "[RoutingCalculations][TreeMapper]") {
  // TODO
}

TEST_CASE("RoutingCalculations isPositionValid", "[RoutingCalculations][isPositionValid]") {
  // valid positions
  REQUIRE(isPositionValid(0, 0, kFanoutMinimum));
  REQUIRE(isPositionValid(0, 0, 3));
  REQUIRE(isPositionValid(0, 0, 4));
  REQUIRE(isPositionValid(0, 0, 5));
  REQUIRE(isPositionValid(0, 0, 6));
  REQUIRE(isPositionValid(0, 0, 7));
  REQUIRE(isPositionValid(0, 0, 8));
  REQUIRE(isPositionValid(0, 0, kFanoutMaximum));

  REQUIRE(isPositionValid(1, 0, 2));
  REQUIRE(isPositionValid(1, 1, 2));
  REQUIRE(isPositionValid(2, 0, 2));
  REQUIRE(isPositionValid(2, 2, 2));
  REQUIRE(isPositionValid(2, 3, 2));

  REQUIRE(isPositionValid(1, 0, 3));
  REQUIRE(isPositionValid(1, 2, 3));
  REQUIRE(isPositionValid(2, 0, 3));
  REQUIRE(isPositionValid(2, 4, 3));
  REQUIRE(isPositionValid(2, 8, 3));

  REQUIRE(isPositionValid(1, 0, 4));
  REQUIRE(isPositionValid(1, 3, 4));
  REQUIRE(isPositionValid(2, 0, 4));
  REQUIRE(isPositionValid(2, 7, 4));
  REQUIRE(isPositionValid(2, 15, 4));

  REQUIRE(isPositionValid(1, 0, 5));
  REQUIRE(isPositionValid(1, 4, 5));
  REQUIRE(isPositionValid(2, 0, 5));
  REQUIRE(isPositionValid(2, 12, 5));
  REQUIRE(isPositionValid(2, 24, 5));

  // invalid positions
  REQUIRE_FALSE(isPositionValid(1, 2, 2));
  REQUIRE_FALSE(isPositionValid(1, 3, 3));
  REQUIRE_FALSE(isPositionValid(1, 4, 4));
  REQUIRE_FALSE(isPositionValid(1, 15, 2));
  REQUIRE_FALSE(isPositionValid(2, 9, 3));
  REQUIRE_FALSE(isPositionValid(2, 25, 5));
  REQUIRE_FALSE(isPositionValid(3, 10000, 4));

  // invalid fanouts
  REQUIRE_FALSE(isPositionValid(0, 0, 0));
  REQUIRE_FALSE(isPositionValid(0, 0, 1));
  REQUIRE_FALSE(isPositionValid(0, 0, kFanoutMaximum + 1));
}

TEST_CASE("RoutingCalculations isFanoutValid", "[RoutingCalculations][isFanoutValid]") {
  // valid fanouts
  REQUIRE(isFanoutValid(kFanoutMinimum));
  REQUIRE(isFanoutValid(2));
  REQUIRE(isFanoutValid(3));
  REQUIRE(isFanoutValid(4));
  REQUIRE(isFanoutValid(kFanoutMaximum));

  // invalid at lower range
  REQUIRE_FALSE(isFanoutValid(kFanoutMinimum - 1));
  REQUIRE_FALSE(isFanoutValid(kFanoutMinimum - 2));
  REQUIRE_FALSE(isFanoutValid(0));
  REQUIRE_FALSE(isFanoutValid(1));

  // invalid at higher range
  REQUIRE_FALSE(isFanoutValid(kFanoutMaximum + 1));
}

TEST_CASE("RoutingCalculations getDSNSet", "[RoutingCalculations][getDSNSet]") {
  // (level, fanout)
  std::vector<std::tuple<uint32_t, uint16_t>> tests = {
      std::make_tuple(0, 2),  std::make_tuple(2, 2), std::make_tuple(4, 2),
      std::make_tuple(10, 2), std::make_tuple(4, 3), std::make_tuple(2, 3),
      std::make_tuple(6, 5),  std::make_tuple(4, 6), std::make_tuple(4, 7)};

  for (auto const &test : tests) {
    uint32_t level = std::get<0>(test);
    uint16_t fanout = std::get<1>(test);
    uint32_t max_num = pow(fanout, level);

    auto dsn_set = getDSNSet(level, fanout);
    auto covered = std::set<uint32_t>();

    for (auto const &dsn_num : dsn_set) {
      covered.insert(dsn_num);

      for (auto const &left_rt : calcLeftRT(level, dsn_num, fanout)) {
        covered.insert(std::get<1>(left_rt));
      }

      for (auto const &right_rt : calcRightRT(level, dsn_num, fanout)) {
        covered.insert(std::get<1>(right_rt));
      }
    }

    // every node covered
    REQUIRE(covered.size() == max_num);
    REQUIRE(*covered.rbegin() == max_num - 1);

    // no prio out of level range
    REQUIRE(*dsn_set.rbegin() < max_num);

    // each dsn is connected to the next one
    if (dsn_set.size() > 2) {
      for (uint32_t i = 0; i < dsn_set.size() - 2; i++) {
        uint32_t dsn_num = dsn_set.at(i);
        uint32_t dsn_num_next = dsn_set.at(i + 1);

        bool connected = false;
        for (auto const &right_rt : calcRightRT(level, dsn_num, fanout)) {
          if (dsn_num_next == std::get<1>(right_rt)) {
            connected = true;
            break;
          }
        }

        REQUIRE(connected);
      }
    }
  }
}

TEST_CASE("RoutingCalculations getCoverArea, getCoveringDSN",
          "[RoutingCalculations][getCoverArea,getCoveringDSN]") {
  // (level, fanout)
  std::vector<std::tuple<uint32_t, uint16_t>> tests = {
      std::make_tuple(4, 2), std::make_tuple(10, 2), std::make_tuple(4, 3), std::make_tuple(4, 5),
      std::make_tuple(6, 5), std::make_tuple(4, 6),  std::make_tuple(4, 7),

      std::make_tuple(1, 2), std::make_tuple(5, 5),  std::make_tuple(3, 2)

  };

  for (auto const &test : tests) {
    uint32_t level = std::get<0>(test);
    uint16_t fanout = std::get<1>(test);

    uint32_t max_num_this_level = pow(fanout, level);
    uint32_t max_num_next_level = pow(fanout, level + 1);

    auto covered_this_level = std::set<uint32_t>();
    auto covered_next_level = std::set<uint32_t>();

    auto dsn_set = getDSNSet(level, fanout);

    if (level % 2 == 1) {
      REQUIRE(dsn_set.empty());
      break;
    }

    for (auto const &dsn_num : dsn_set) {
      covered_this_level.insert(dsn_num);
      auto cover_area = getCoverArea(level, dsn_num, fanout);

      for (auto const &covered_node : cover_area) {
        uint32_t covered_level = std::get<0>(covered_node);
        uint32_t covered_number = std::get<1>(covered_node);

        if (covered_level == level) {
          covered_this_level.insert(covered_number);
        } else {
          covered_next_level.insert(covered_number);
        }

        auto covering_dsn = getCoveringDSN(covered_level, covered_number, fanout);
        uint32_t covering_dsn_level = std::get<0>(covering_dsn);
        uint32_t covering_dsn_number = std::get<1>(covering_dsn);

        REQUIRE(covering_dsn_level == level);
        REQUIRE(covering_dsn_number == dsn_num);
      }

      for (auto const &child : calcChildren(level, dsn_num, fanout)) {
        covered_next_level.insert(std::get<1>(child));
        auto covering_dsn = getCoveringDSN(std::get<0>(child), std::get<1>(child), fanout);

        uint32_t covering_dsn_level = std::get<0>(covering_dsn);
        uint32_t covering_dsn_number = std::get<1>(covering_dsn);

        REQUIRE(covering_dsn_level == level);
        REQUIRE(covering_dsn_number == dsn_num);
      }
    }

    // every node covered on even level
    REQUIRE(covered_this_level.size() == max_num_this_level);
    REQUIRE(*covered_this_level.rbegin() == max_num_this_level - 1);

    // every node covered on uneven level
    REQUIRE(covered_next_level.size() == max_num_next_level);
    REQUIRE(*covered_next_level.rbegin() == max_num_next_level - 1);

    // no dsn out of level range on even level
    REQUIRE(*dsn_set.rbegin() < max_num_this_level);
  }
}
