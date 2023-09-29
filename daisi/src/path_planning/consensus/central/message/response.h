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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_MESSAGE_RESPONSE_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_MESSAGE_RESPONSE_H_

#include <cstdint>
#include <tuple>
#include <vector>

#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::consensus {
//! Response to a request from central server to pickup station
struct Response {
  SERIALIZE(success, request_id, start_offset);
  uint32_t request_id;  //!< Per station unique request id to match requests and responses
  bool success;         //!< True if request was successful. If false, \p start_offset_ is not valid
  double start_offset;  //!< Global possible start time in seconds
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_MESSAGE_RESPONSE_H_
