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

#ifndef DAISI_CPPS_COMMON_UUID_GENERATOR_H_
#define DAISI_CPPS_COMMON_UUID_GENERATOR_H_

#include <string>

#define GENERATE_ASCENDING_UUIDS
#undef GENERATE_REAL_UUIDS

#ifdef GENERATE_ASCENDING_UUIDS
#include <cstdint>
#endif

class UUIDGenerator {
public:
  static UUIDGenerator &get() {
    static UUIDGenerator generator;
    return generator;
  }

  std::string operator()();

private:
#ifdef GENERATE_ASCENDING_UUIDS
  //__int128 next_uuid_ = 0; // https://gcc.gnu.org/onlinedocs/gcc/_005f_005fint128.html
  uint64_t next_uuid_ = 0;
#endif
};

#endif
