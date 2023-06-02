// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

#include "natter/network_info_ipv4.h"

TEST_CASE("[MINHCAST] NetworkInfoIpv4Test", "Equality") {
  natter::NetworkInfoIPv4 n1{.ip = "10.0.0.0", .port = 2000};
  natter::NetworkInfoIPv4 n2{.ip = "10.0.0.0", .port = 2000};
  REQUIRE(n1 == n2);
  n2.port = 2001;
  REQUIRE_FALSE(n1 == n2);
  n1.port = 2001;
  REQUIRE(n1 == n2);
  n1.ip = "10.0.0.1";
  REQUIRE_FALSE(n1 == n2);
  n2.ip = "10.0.0.1";
  REQUIRE(n1 == n2);
}
