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

#ifndef DAISI_CPPS_MESSAGE_TASK_UPDATE_H_
#define DAISI_CPPS_MESSAGE_TASK_UPDATE_H_

#include <cstdint>

#include "cpps/model/task.h"
#include "ns3/vector.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {

class TaskUpdate {
public:
  TaskUpdate() = default;
  TaskUpdate(const std::string &order_uuid, OrderStates order_state, ns3::Vector position);

  const std::string &getTaskUuid() const;
  ns3::Vector getPosition() const;
  OrderStates getOrderState() const;

  std::string getLoggingContent() const;

  SERIALIZE(task_uuid_, position_x_, position_y_, order_state_);

private:
  std::string task_uuid_;
  OrderStates order_state_ = OrderStates::kError;
  double position_x_ = 0.0;
  double position_y_ = 0.0;
};

}  // namespace daisi::cpps

#endif
