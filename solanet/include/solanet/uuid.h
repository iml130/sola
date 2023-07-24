// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLANET_UUID_H_
#define SOLANET_UUID_H_

#include <array>
#include <cstdint>
#include <string>

namespace solanet {

namespace detail {
static constexpr uint32_t kUuidByteLength = 16;  // 16 byte = 128 bit
}

using UUID = std::array<uint8_t, detail::kUuidByteLength>;

/// Transform UUID to string representation
std::string uuidToString(UUID uuid);
}  // namespace solanet

#endif  // SOLANET_UUID_H_
