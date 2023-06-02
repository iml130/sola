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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_ACCEPT_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_ACCEPT_H_

#include <cstdint>
#include <string>

#include "cpps/message/serialize.h"
#include "path_planning/station.h"

namespace daisi::path_planning::consensus {
//!< Accept message send from the proposer to the acceptors with the actual intersection
//!< occupancies that should be set for the given instance
struct AcceptMessage {
  SERIALIZE(instance_id, proposal_id, station_id, intersections);

  uint32_t instance_id = 0;
  uint32_t proposal_id = 0;
  uint32_t station_id = 0;
  std::vector<IntersectionTimeInfo> intersections;
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_ACCEPT_H_
