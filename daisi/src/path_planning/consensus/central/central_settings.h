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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_SETTINGS_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_SETTINGS_H_

#include <cstdint>
#include <string>

namespace daisi::path_planning::consensus {
struct CentralSettings {
  std::string server_ip;
  uint16_t server_port = 0;
  double time_between_intersects;  //!< Delta time between two occupancies of one intersection in
                                   //!< seconds
  double max_preplanning_time;     //!< Maximum time to look in the future to find a collision free
                                   //!< occupancy in seconds
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_SETTINGS_H_
