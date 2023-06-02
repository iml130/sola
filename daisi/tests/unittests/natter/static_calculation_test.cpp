// Copyright 2023 The SOLA authors
//
// This file is part of DAISI.
//
// DAISI is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation; version 2.
//
// DAISI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with DAISI. If not, see
// <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-2.0-only

#include <catch2/catch_test_macros.hpp>

#include "natter-ns3/static_network_calculation.h"

using namespace daisi::natter_ns3;

TEST_CASE("Calculate Level", "[static_calculation]") {
  REQUIRE(calculateLevel(0, 2) == 0);
  REQUIRE(calculateLevel(1, 2) == 1);
  REQUIRE(calculateLevel(41370, 14) == 4);
  REQUIRE(calculateLevel(41371, 14) == 5);
}

TEST_CASE("Calculate Number", "[static_calculation]") {
  REQUIRE(calculateNumber(0, 2, 0) == 0);
  REQUIRE(calculateNumber(1, 2, 1) == 0);
}

TEST_CASE("Create linear projection", "[static_calculation]") {
  // Fanout 2
  std::vector<uint32_t> proj = createLinearProjection(22, 2, 0);
  std::vector<uint32_t> correct{15, 7,  16, 3, 17, 8, 18, 1, 19, 9, 20,
                                4,  21, 10, 0, 11, 5, 12, 2, 13, 6, 14};
  REQUIRE(correct == proj);

  // Fanout 3
  proj = createLinearProjection(40, 3, 0);
  correct = {13, 14, 4,  15, 16, 17, 5, 18, 1,  19, 20, 6,  21, 22, 23, 7, 24, 25, 26, 8,
             27, 2,  28, 29, 9,  30, 0, 31, 32, 10, 33, 34, 35, 11, 36, 3, 37, 38, 12, 39};
  REQUIRE(correct == proj);
}
