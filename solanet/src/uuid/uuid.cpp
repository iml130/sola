// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "solanet/uuid.h"

#include "uuid/uuid.h"

namespace solanet {

std::string uuidToString(UUID uuid) {
  // 00000000-0000-0000-0000-000000000000 + null-terminator
  constexpr size_t kUuidStringLength = 37;
  std::array<char, kUuidStringLength> id{};
  uuid_unparse(uuid.data(), id.data());
  return {id.data()};
}
}  // namespace solanet
