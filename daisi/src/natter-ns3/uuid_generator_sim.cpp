// Copyright 2023 The SOLA authors
//
// This file is part of DAISI.
//
// DAISI is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation; version 2.
//
// DAISI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with DAISI. If not, see
// <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-2.0-only

#include <cstdint>
#include <cstring>
#include <random>

#include "utils.h"
#include "utils/random_engine.h"

namespace natter::utils {

UUID generateUUID() {
  // Use for simulation only! This joins two random uint64_ts but is not as random as a proper 128
  // bit UUID. See here for further discussion:
  // https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library#comment114528438_60198074
  std::uniform_int_distribution<uint64_t> dist;
  uint64_t first_part = dist(daisi::global_random_engine);
  uint64_t second_part = dist(daisi::global_random_engine);
  UUID uuid{};
  std::memcpy(uuid.data(), &first_part, sizeof(uint64_t));
  std::memcpy(uuid.data() + uuid.size() / 2 * sizeof(uint8_t), &second_part, sizeof(uint64_t));
  return uuid;
}

}  // namespace natter::utils
