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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_MESSAGE_REQUEST_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_MESSAGE_REQUEST_H_

#include <cstdint>
#include <tuple>
#include <vector>

#include "cpps/message/serialize.h"

namespace daisi::path_planning::consensus {
//! Request message from a pickup station to the central server
struct Request {
  SERIALIZE(request_id, earliest_possible_start_ms, intersection_times);
  uint32_t request_id;  //!< Per station unique request id to match requests and responses
  uint64_t earliest_possible_start_ms;  //!< Earliest possible time at which the AGV can start
  std::vector<std::tuple<float, float, double>>
      intersection_times;  //!< List of tuples which contains x,y coordinates and a relative
                           //!< timestamp
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_MESSAGE_REQUEST_H_
