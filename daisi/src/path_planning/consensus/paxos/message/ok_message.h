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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_OK_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_OK_H_

#include <cstdint>

#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::consensus {
//!< OK message from all acceptors to all other participants to commit the requested occupancy
struct OKMessage {
  SERIALIZE(instance_id, station_id, proposal_id, sender_station);
  uint32_t instance_id = 0;
  uint32_t proposal_id = 0;
  uint32_t station_id = 0;
  uint32_t sender_station = 0;
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_OK_H_
