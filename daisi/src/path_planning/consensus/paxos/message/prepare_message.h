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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_PREPARE_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_PREPARE_H_

#include <cstdint>
#include <string>
#include <vector>

#include "path_planning/station.h"
#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::consensus {
//!< Prepare message send from proposer to all acceptors which is used to get a promise from
//!< all acceptors (quorum) so that they will accept the occupancies.
struct PrepareMessage {
  SERIALIZE(instance_id, prepare_id, station_id);

  uint32_t instance_id = 0;
  uint32_t prepare_id = 0;
  uint32_t station_id = 0;
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_PREPARE_H_
