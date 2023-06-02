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

#include "serializer.h"

// Some cereal includes might be marked as unused. They are actually used, depending on the template
// parameter
#ifndef CPPCHECK_IGNORE
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#endif

namespace daisi::path_planning::message {

template <typename T> std::string serialize(const T &msg) {
  std::stringstream ss;
  cereal::BinaryOutputArchive archive(ss);
  archive(msg);
  return ss.str();
}

template <typename T> T deserialize(const std::string &msg) {
  T m;
  std::istringstream iss(msg);
  cereal::BinaryInputArchive a(iss);
  a(m);
  return m;
}

// Explicit template instantiation definition

template std::string serialize<consensus::PaxosMessage>(const consensus::PaxosMessage &msg);
template consensus::PaxosMessage deserialize<consensus::PaxosMessage>(const std::string &msg);

template std::string serialize<StationAGVMessage>(const StationAGVMessage &msg);
template StationAGVMessage deserialize<StationAGVMessage>(const std::string &msg);

template std::string serialize<MiscMessage>(const MiscMessage &msg);
template MiscMessage deserialize<MiscMessage>(const std::string &msg);

template std::string serialize<FieldMessage>(const FieldMessage &msg);
template FieldMessage deserialize<FieldMessage>(const std::string &msg);

template std::string serialize<consensus::Request>(const consensus::Request &msg);
template consensus::Request deserialize<consensus::Request>(const std::string &msg);

template std::string serialize<consensus::Response>(const consensus::Response &msg);
template consensus::Response deserialize<consensus::Response>(const std::string &msg);

template std::string serialize<consensus::ReplicationMessage>(
    const consensus::ReplicationMessage &msg);
template consensus::ReplicationMessage deserialize<consensus::ReplicationMessage>(
    const std::string &msg);

}  // namespace daisi::path_planning::message
