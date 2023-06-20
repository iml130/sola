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

#ifndef DAISI_PATH_PLANNING_MESSAGE_SERIALIZER_H_
#define DAISI_PATH_PLANNING_MESSAGE_SERIALIZER_H_

#include "path_planning/consensus/paxos/message/types.h"
#include "solanet/serializer/serialize.h"
#include "types_all.h"

namespace daisi::path_planning::message {

/**
 * Binary serialize \p msg into a std::string (as container)
 * @tparam T
 * @param msg
 * @return
 */
template <typename T> std::string serialize(const T &msg);

/**
 * Deserialize binary message into message of type T.
 * @tparam T
 * @param msg
 * @return
 */
template <typename T> T deserialize(const std::string &msg);

// Explicit template instantiation declaration

extern template std::string serialize<consensus::PaxosMessage>(const consensus::PaxosMessage &msg);
extern template consensus::PaxosMessage deserialize<consensus::PaxosMessage>(
    const std::string &msg);

extern template std::string serialize<StationAGVMessage>(const StationAGVMessage &msg);
extern template StationAGVMessage deserialize<StationAGVMessage>(const std::string &msg);

extern template std::string serialize<MiscMessage>(const MiscMessage &msg);
extern template MiscMessage deserialize<MiscMessage>(const std::string &msg);

extern template std::string serialize<FieldMessage>(const FieldMessage &msg);
extern template FieldMessage deserialize<FieldMessage>(const std::string &msg);

extern template std::string serialize<consensus::Request>(const consensus::Request &msg);
extern template consensus::Request deserialize<consensus::Request>(const std::string &msg);

extern template std::string serialize<consensus::Response>(const consensus::Response &msg);
extern template consensus::Response deserialize<consensus::Response>(const std::string &msg);

extern template std::string serialize<consensus::ReplicationMessage>(
    const consensus::ReplicationMessage &msg);
extern template consensus::ReplicationMessage deserialize<consensus::ReplicationMessage>(
    const std::string &msg);

}  // namespace daisi::path_planning::message

#endif  // DAISI_PATH_PLANNING_MESSAGE_SERIALIZER_H_
