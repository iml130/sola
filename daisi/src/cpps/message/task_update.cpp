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

#include "cpps/message/task_update.h"

namespace daisi::cpps {
TaskUpdate::TaskUpdate(const std::string &order_uuid, OrderStates order_state, ns3::Vector position)
    : task_uuid_(order_uuid),
      order_state_(order_state),
      position_x_(position.x),
      position_y_(position.y) {}

const std::string &TaskUpdate::getTaskUuid() const { return task_uuid_; }

ns3::Vector TaskUpdate::getPosition() const { return ns3::Vector(position_x_, position_y_, 0.0F); }

OrderStates TaskUpdate::getOrderState() const { return order_state_; }

std::string TaskUpdate::getLoggingContent() const {
  std::stringstream stream;

  stream << task_uuid_ << ";" << static_cast<int>(order_state_) << ";" << position_x_ << ";"
         << position_y_;

  return stream.str();
}

}  // namespace daisi::cpps
