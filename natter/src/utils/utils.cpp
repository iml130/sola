// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "utils.h"

#include <uuid/uuid.h>

#include <array>
#include <cstddef>

namespace natter {
std::string uuidToString(UUID uuid) {
  constexpr size_t kUuidLength = 37;  // 00000000-0000-0000-0000-000000000000 + null-terminator
  std::array<char, kUuidLength> id{};
  uuid_unparse(uuid.data(), id.data());
  return {id.data()};
}
}  // namespace natter
