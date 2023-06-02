// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

#include "message/types_all.h"
#include "utils/uuid.h"

using namespace minhton;

void testKeys(uint64_t number) {
  std::set<uint64_t> set;

  for (uint64_t i = 0; i < number; i++) {
    uint64_t temp_value = minhton::generateEventId();
    if (set.find(temp_value) != set.end())
      throw std::runtime_error("SAME NUMBERS " + std::to_string(i));
    set.insert(temp_value);
  }
}

TEST_CASE("Utils KeyCollision", "[Utils][KeyCollision]") {
  SECTION("Utils KeyCollision", "Utils KeyCollision") { REQUIRE_NOTHROW(testKeys(10000000)); }
}
