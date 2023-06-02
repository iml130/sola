// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_MINHCAST_LEVEL_NUMBER_H_
#define NATTER_MINHCAST_LEVEL_NUMBER_H_

#include <cstdint>
#include <tuple>

namespace natter::minhcast {
using Level = uint32_t;
using Number = uint32_t;
using Fanout = uint32_t;

using LevelNumber = std::tuple<Level, Number, Fanout>;
}  // namespace natter::minhcast

#endif  // DAISI_MINHCAST_LEVEL_NUMBER_H_
