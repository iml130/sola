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

#include "cpps/common/uuid_generator.h"

#if defined(GENERATE_ASCENDING_UUIDS) && !defined(GENERATE_REAL_UUIDS)

#include <iomanip>
#include <sstream>

std::string UUIDGenerator::operator()() {
  std::stringstream stream;

  stream << std::setfill('0') << std::setw(32) << std::hex << next_uuid_++ << std::dec;
  std::string id = stream.str();
  id.insert(8, 1, '-');
  id.insert(13, 1, '-');
  id.insert(18, 1, '-');
  id.insert(23, 1, '-');

  return id;
}

#elif defined(GENERATE_REAL_UUIDS) && !defined(GENERATE_ASCENDING_UUIDS)

#include <uuid/uuid.h>

std::string UUIDGenerator::operator()() {
  uuid_t uuid;
  uuid_generate(uuid);
  std::string id_;
  id_.resize(36);
  uuid_unparse(uuid, id_.data());
  return id_;
}

#else
#error No UUID generation method defined
#endif
