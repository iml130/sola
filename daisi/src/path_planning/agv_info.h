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

#ifndef DAISI_PATH_PLANNING_AGV_INFO_H_
#define DAISI_PATH_PLANNING_AGV_INFO_H_

#include <cstdint>
#include <string>

#include "agv_state.h"
#include "cpps/model/kinematics.h"
#include "transport_order_info.h"

namespace daisi::path_planning {

//! Static and dynamic AGV information that a station needs to know
struct AGVInfo {
  std::string id;
  std::string ip;
  cpps::Kinematics kinematics;
  ns3::Vector2D next_position;
  ns3::Vector2D last_position;

  // ID of the next station this AGV should approach. Set to UINT32_MAX if the AGV should stay at
  // the current station
  uint32_t next_station = UINT32_MAX;
  AGVState state = AGVState::kWaitingInQueue;
  TransportOrderInfo current_to;

  [[nodiscard]] bool empty() const { return id.empty() && ip.empty(); }
};
}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_AGV_INFO_H_
