// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_UUID_H_
#define NATTER_UUID_H_

#include <array>
#include <cstdint>
#include <string>

namespace natter {
static constexpr uint32_t kUuidByteLength = 16;  // 16 byte = 128 bit

using UUID = std::array<uint8_t, kUuidByteLength>;

// Public uuid utils
std::string uuidToString(UUID uuid);

}  // namespace natter

#endif  // DAISI_UUID_H_
