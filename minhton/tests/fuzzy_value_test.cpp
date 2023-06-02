// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "algorithms/esearch/fuzzy_value.h"

#include <catch2/catch_test_macros.hpp>

using namespace minhton;

TEST_CASE("FuzzyValue Constructor", "[FuzzyValue][Init]") {
  FuzzyValue v1 = FuzzyValue::createFalse();
  REQUIRE(v1.isFalse());

  FuzzyValue v2 = FuzzyValue::createUndecided();
  REQUIRE(v2.isUndecided());

  FuzzyValue v3 = FuzzyValue::createTrue();
  REQUIRE(v3.isTrue());
}

TEST_CASE("FuzzyValue Negation", "[FuzzyValue][Negation]") {
  FuzzyValue v1 = FuzzyValue::createFalse();
  REQUIRE(v1.isFalse());
  REQUIRE((!v1).isTrue());

  FuzzyValue v2 = FuzzyValue::createUndecided();
  REQUIRE(v2.isUndecided());
  REQUIRE((!v2).isUndecided());

  FuzzyValue v3 = FuzzyValue::createTrue();
  REQUIRE(v3.isTrue());
  REQUIRE((!v3).isFalse());
}

TEST_CASE("FuzzyValue And", "[FuzzyValue][And]") {
  auto false_and_false = FuzzyValue::createFalse() && FuzzyValue::createFalse();
  REQUIRE(false_and_false.isFalse());

  auto false_and_undecided = FuzzyValue::createFalse() && FuzzyValue::createUndecided();
  REQUIRE(false_and_undecided.isFalse());

  auto false_and_true = FuzzyValue::createFalse() && FuzzyValue::createTrue();
  REQUIRE(false_and_true.isFalse());

  auto undecided_and_false = FuzzyValue::createUndecided() && FuzzyValue::createFalse();
  REQUIRE(undecided_and_false.isFalse());

  auto undecided_and_undecided = FuzzyValue::createUndecided() && FuzzyValue::createUndecided();
  REQUIRE(undecided_and_undecided.isUndecided());

  auto undecided_and_true = FuzzyValue::createUndecided() && FuzzyValue::createTrue();
  REQUIRE(undecided_and_true.isUndecided());

  auto true_and_false = FuzzyValue::createTrue() && FuzzyValue::createFalse();
  REQUIRE(true_and_false.isFalse());

  auto true_and_undecided = FuzzyValue::createTrue() && FuzzyValue::createUndecided();
  REQUIRE(true_and_undecided.isUndecided());

  auto true_and_true = FuzzyValue::createTrue() && FuzzyValue::createTrue();
  REQUIRE(true_and_true.isTrue());
}

TEST_CASE("FuzzyValue Or", "[FuzzyValue][Or]") {
  auto false_or_false = FuzzyValue::createFalse() || FuzzyValue::createFalse();
  REQUIRE(false_or_false.isFalse());

  auto false_or_undecided = FuzzyValue::createFalse() || FuzzyValue::createUndecided();
  REQUIRE(false_or_undecided.isUndecided());

  auto false_or_true = FuzzyValue::createFalse() || FuzzyValue::createTrue();
  REQUIRE(false_or_true.isTrue());

  auto undecided_or_false = FuzzyValue::createUndecided() || FuzzyValue::createFalse();
  REQUIRE(undecided_or_false.isUndecided());

  auto undecided_or_undecided = FuzzyValue::createUndecided() || FuzzyValue::createUndecided();
  REQUIRE(undecided_or_undecided.isUndecided());

  auto undecided_or_true = FuzzyValue::createUndecided() || FuzzyValue::createTrue();
  REQUIRE(undecided_or_true.isTrue());

  auto true_or_false = FuzzyValue::createTrue() || FuzzyValue::createFalse();
  REQUIRE(true_or_false.isTrue());

  auto true_or_undecided = FuzzyValue::createTrue() || FuzzyValue::createUndecided();
  REQUIRE(true_or_undecided.isTrue());

  auto true_or_true = FuzzyValue::createTrue() || FuzzyValue::createTrue();
  REQUIRE(true_or_true.isTrue());
}

TEST_CASE("FuzzyValue Equal", "[FuzzyValue][Equal]") {
  auto fuzz_true1 = FuzzyValue::createTrue();
  auto fuzz_true2 = FuzzyValue::createTrue();
  auto fuzz_undec1 = FuzzyValue::createUndecided();
  auto fuzz_undec2 = FuzzyValue::createUndecided();
  auto fuzz_false1 = FuzzyValue::createFalse();
  auto fuzz_false2 = FuzzyValue::createFalse();

  REQUIRE(fuzz_true1 == fuzz_true2);
  REQUIRE(fuzz_true1 != fuzz_undec1);
  REQUIRE(fuzz_true1 != fuzz_false1);

  REQUIRE(fuzz_undec1 != fuzz_true1);
  REQUIRE(fuzz_undec1 == fuzz_undec2);
  REQUIRE(fuzz_undec1 != fuzz_false1);

  REQUIRE(fuzz_false1 != fuzz_true1);
  REQUIRE(fuzz_false1 != fuzz_undec1);
  REQUIRE(fuzz_false1 == fuzz_false2);
}
