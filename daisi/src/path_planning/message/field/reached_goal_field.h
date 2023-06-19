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

#ifndef DAISI_PATH_PLANNING_MESSAGE_FIELD_REACHED_GOAL_FIELD_H_
#define DAISI_PATH_PLANNING_MESSAGE_FIELD_REACHED_GOAL_FIELD_H_

#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::message {
//! Message that \c AGVPhysicalBasic sends to inform \c AGVLogical that previous goal from \c
//! DriveMessageField has been reached.
struct ReachedGoalField {
  // No payload needed
};
}  // namespace daisi::path_planning::message

#endif  // DAISI_PATH_PLANNING_MESSAGE_FIELD_REACHED_GOAL_FIELD_H_
