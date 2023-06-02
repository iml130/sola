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

#include "cpps/message/serializer.h"

#ifndef CPPCHECK_IGNORE
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#endif

namespace daisi::cpps {

std::string serialize(const Message &msg) {
  std::stringstream ss;
  cereal::BinaryOutputArchive archive(ss);
  archive(msg);
  return ss.str();
}

Message deserialize(const std::string &msg) {
  Message m;
  std::istringstream iss(msg);
  cereal::BinaryInputArchive a(iss);
  a(m);
  return m;
}

}  // namespace daisi::cpps
