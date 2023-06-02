// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "utils/tree_helper.h"

#include <catch2/catch_test_macros.hpp>

using namespace natter::minhcast;

TEST_CASE("[TREE_HELPER] childFromNode", "TREE_HELPER isChild") {
  REQUIRE(childFromNode({1, 0, 2}, {0, 0, 2}));
  REQUIRE(childFromNode({1, 15, 16}, {0, 0, 16}));
  REQUIRE_FALSE(childFromNode({36, 22, 13}, {36, 5, 13}));
  REQUIRE(childFromNode({37, 77, 13}, {36, 5, 13}));

  REQUIRE(childFromNode({128, 128, 10}, {0, 0, 10}));
}

TEST_CASE("[TREE_HELPER] calculateRRT", "TREE_HELPER calculateRRT") {
  REQUIRE(calculateRRT({0, 0, 2}).empty());
  REQUIRE(calculateRRT({0, 0, 16}).empty());
  REQUIRE(calculateRRT({1, 0, 2}).size() == 1);
  REQUIRE(calculateRRT({1, 1, 2}).empty());

  REQUIRE(calculateRRT({16, 0, 2}).size() == 16);
  REQUIRE(calculateRRT({16, 65535, 2}).empty());
}

TEST_CASE("[TREE_HELPER] calculateLRT", "TREE_HELPER calculateLRT") {
  REQUIRE(calculateLRT({0, 0, 2}).empty());
  REQUIRE(calculateLRT({0, 0, 16}).empty());
  REQUIRE(calculateLRT({1, 0, 2}).empty());
  REQUIRE(calculateLRT({1, 1, 2}).size() == 1);

  REQUIRE(calculateLRT({16, 0, 2}).empty());
  REQUIRE(calculateLRT({16, 65535, 2}).size() == 16);
}

TEST_CASE("[TREE_HELPER] LevelNumberPair", "TREE_HELPER LevelNumberPair") {
  LevelNumber node{14, 2, 2};
  std::string pair = toLevelNumberPair(node);
  REQUIRE(pair == "14:2");
  REQUIRE(fromStringToLevelNumber(pair) == LevelNumber{14, 2, 0});
}
