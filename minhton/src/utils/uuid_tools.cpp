// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <uuid/uuid.h>

#include <array>
#include <cstring>
#include <string>

#include "minhton/utils/uuid.h"

namespace minhton {
std::string uuidToString(UUID uuid) {
  constexpr size_t kUuidLength = 37;  // 00000000-0000-0000-0000-000000000000 + null-terminator
  std::array<char, kUuidLength> id{};
  uuid_unparse(uuid.data(), id.data());
  return {id.data()};
}
}  // namespace minhton
