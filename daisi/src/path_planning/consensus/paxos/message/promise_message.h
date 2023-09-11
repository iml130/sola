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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_PROMISE_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_PROMISE_H_

#include <cstdint>

#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::consensus {
//!< Promise message from acceptors to proposer to promise that the acceptor will accept new values
//!< for the given instance (if no other station with higher priority for the same instance is also
//!< sending a prepare message before the original proposer can propagate its occupancy to all
//!< acceptors).
struct PromiseMessage {
  SERIALIZE(instance_id, prepare_id, station_id, already_accepted, accepted_prepare_id,
            accepted_station_id)
  uint32_t instance_id = 0;
  uint32_t prepare_id = 0;
  uint32_t station_id = 0;

  bool already_accepted = false;
  uint32_t accepted_prepare_id = 0;
  uint32_t accepted_station_id = 0;
  // TODO already accepted values if any in the future. Only needed in case of failure
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_MESSAGE_PROMISE_H_
