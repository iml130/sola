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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_SETTINGS_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_SETTINGS_H_

#include <cstdint>

namespace daisi::path_planning::consensus {
struct PaxosSettings {
  bool pickup_active_participate = true;
  bool delivery_active_participate = false;
  bool amr_active_participate = false;
  bool replication = true;
  uint64_t number_paxos_participants{};  // including ourselves
  double time_delta_intersections{};
  double max_preplanning_time{};
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_SETTINGS_H_
