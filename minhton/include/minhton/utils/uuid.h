// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_UUID_H_
#define MINHTON_UUID_H_

#include <array>
#include <cstdint>
#include <string>

namespace minhton {
static constexpr uint32_t kUuidByteLength = 16;  // 16 byte = 128 bit

using UUID = std::array<uint8_t, kUuidByteLength>;

//! Transform UUID to string representation1
std::string uuidToString(UUID uuid);

minhton::UUID generateUUID();

/// Generates a unique event id using the Mersenne Twister 19937 generator
/// \returns unique event id
uint64_t generateEventId();

}  // namespace minhton

#endif  // MINHTON_UUID_H_
