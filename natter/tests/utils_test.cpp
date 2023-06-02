// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "utils/utils.h"

#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <set>

using namespace natter;

/**
 * Tests unique message-id generation
 */
TEST_CASE("[UTILS] General", "Async") {
  constexpr size_t runs = 10000;
  std::set<UUID> ids{};
  for (int i = 0; i < runs; i++) {
    UUID message_id = utils::generateUUID();
    auto it = ids.find(message_id);

    // Should not be a duplicate
    REQUIRE(it == ids.end());
    ids.insert(message_id);
  }
}
