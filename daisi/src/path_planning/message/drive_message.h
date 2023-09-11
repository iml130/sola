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

#ifndef DAISI_PATH_PLANNING_MESSAGE_DRIVE_MESSAGE_H_
#define DAISI_PATH_PLANNING_MESSAGE_DRIVE_MESSAGE_H_

#include <ns3/vector.h>

#include <cstdint>
#include <optional>

#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::message {

//! Message that is send from \c PickupStation to logical AGV with drive instructions
class DriveMessage {
public:
  DriveMessage() = default;

  explicit DriveMessage(ns3::Vector2D pos) : initialized_(true), pos_x_(pos.x), pos_y_(pos.y) {}

  [[nodiscard]] bool isInitialized() const { return initialized_; }

  [[nodiscard]] ns3::Vector2D getGoal() const { return {pos_x_, pos_y_}; };

  SERIALIZE(pos_x_, pos_y_)

private:
  bool initialized_ = false;
  double pos_x_ = 0;
  double pos_y_ = 0;
};
}  // namespace daisi::path_planning::message

#endif  // DAISI_PATH_PLANNING_MESSAGE_DRIVE_MESSAGE_H_
