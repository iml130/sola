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

#ifndef DAISI_CPPS_MESSAGE_TASK_INFO_H_
#define DAISI_CPPS_MESSAGE_TASK_INFO_H_

#include <cstdint>

#include "cpps/model/task.h"
#include "ns3/vector.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {

class TaskInfo {
public:
  TaskInfo() = default;
  explicit TaskInfo(const Task &order);

  const std::string &getTaskUuid() const;
  ns3::Vector getPickupLocation() const;
  ns3::Vector getDeliveryLocation() const;
  amr::AmrStaticAbility getAbilityRequirement() const;

  SERIALIZE(task_uuid_, pickup_x_, pickup_y_, delivery_x_, delivery_y_, ability_requirement_);

private:
  std::string task_uuid_;
  double pickup_x_ = 0.0;
  double pickup_y_ = 0.0;
  double delivery_x_ = 0.0;
  double delivery_y_ = 0.0;
  amr::AmrStaticAbility ability_requirement_;
};

}  // namespace daisi::cpps

#endif
