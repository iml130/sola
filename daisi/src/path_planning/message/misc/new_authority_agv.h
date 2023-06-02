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

#ifndef DAISI_PATH_PLANNING_MESSAGE_MISC_NEW_AUTHORITY_AGV_H_
#define DAISI_PATH_PLANNING_MESSAGE_MISC_NEW_AUTHORITY_AGV_H_

#include <string>

#include "cpps/message/serialize.h"

namespace daisi::path_planning::message {
//! Message from \c AGVLogical to \c PickupStation to register the AGV with the pickup station (the
//! pickup stations "takes authority" over the AGV)
class NewAuthorityAGV {
public:
  SERIALIZE(agv_ip, agv_uuid, min_acceleration, max_acceleration, min_velocity, max_velocity,
            load_time_s, unload_time_s, current_x, current_y, initial, current_del_dest_x,
            current_del_dest_y);

  std::string agv_ip;
  std::string agv_uuid;
  double min_acceleration;
  double max_acceleration;
  double min_velocity;
  double max_velocity;
  double load_time_s;
  double unload_time_s;
  double current_x;
  double current_y;
  bool initial = true;  //!< True if this message is not outcome of an AGV handover
  double current_del_dest_x = std::numeric_limits<double>::quiet_NaN();
  double current_del_dest_y = std::numeric_limits<double>::quiet_NaN();
};
}  // namespace daisi::path_planning::message

#endif  // DAISI_PATH_PLANNING_MESSAGE_MISC_NEW_AUTHORITY_AGV_H_
