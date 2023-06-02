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

#ifndef DAISI_PATH_PLANNING_MESSAGE_TYPES_ALL_H_
#define DAISI_PATH_PLANNING_MESSAGE_TYPES_ALL_H_

#include <variant>

#include "path_planning/consensus/central/message/request.h"
#include "path_planning/consensus/central/message/response.h"
#include "path_planning/message/drive_message.h"
#include "path_planning/message/field/drive_message_field.h"
#include "path_planning/message/field/position_update.h"
#include "path_planning/message/field/reached_goal_field.h"
#include "path_planning/message/handover_message.h"
#include "path_planning/message/misc/new_authority_agv.h"
#include "path_planning/message/misc/reached_goal.h"

namespace daisi::path_planning::message {

//! Message types send from \c PickupStation to \c AGVLogical
using StationAGVMessage = std::variant<DriveMessage, HandoverMessage>;

//! Message types send from \c AGVLogical to \c PickupStation
using MiscMessage = std::variant<NewAuthorityAGV, ReachedGoal>;

//! Message types send between \c AGVLogical and \c AGVPhysicalBasic
using FieldMessage = std::variant<PositionUpdate, ReachedGoalField, DriveMessageField>;

}  // namespace daisi::path_planning::message

#endif  // DAISI_PATH_PLANNING_MESSAGE_TYPES_ALL_H_
