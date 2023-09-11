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

#ifndef DAISI_PATH_PLANNING_MESSAGE_MISC_REACHED_GOAL_H_
#define DAISI_PATH_PLANNING_MESSAGE_MISC_REACHED_GOAL_H_

#include <string>

#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::message {
//! Message that \c AGVLogical sends to inform \c PickupStation that previous goal from \c
//! DriveMessage has been reached.
struct ReachedGoal {
  SERIALIZE(agv_uuid, x, y)
  std::string agv_uuid;
  double x;
  double y;
};
}  // namespace daisi::path_planning::message

#endif  // DAISI_PATH_PLANNING_MESSAGE_MISC_REACHED_GOAL_H_
