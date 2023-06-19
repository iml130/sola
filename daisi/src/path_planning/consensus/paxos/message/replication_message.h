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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_REPLICATION_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_REPLICATION_H_

#include <cstdint>

#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::consensus {
//!< Replication message send from all acceptors on the replication topic to let other network
//!< participants know about this consensus
//!< Possible to add other data, e.g., billing relevant data
struct ReplicationMessage {
  SERIALIZE(instance_id, station_id, proposal_id);
  uint32_t instance_id = 0;
  uint32_t proposal_id = 0;
  uint32_t station_id = 0;
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_REPLICATION_H_
