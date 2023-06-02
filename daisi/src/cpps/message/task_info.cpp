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

#include "cpps/message/task_info.h"

namespace daisi::cpps {

TaskInfo::TaskInfo(const Task &order)
    : task_uuid_(order.getUUID()),
      pickup_x_(order.getPickupLocation().x),
      pickup_y_(order.getPickupLocation().y),
      delivery_x_(order.getDeliveryLocation().x),
      delivery_y_(order.getDeliveryLocation().y),
      ability_requirement_(order.getAbilityRequirement()) {}

const std::string &TaskInfo::getTaskUuid() const { return task_uuid_; }

ns3::Vector TaskInfo::getPickupLocation() const { return ns3::Vector(pickup_x_, pickup_y_, 0.0); }

ns3::Vector TaskInfo::getDeliveryLocation() const {
  return ns3::Vector(delivery_x_, delivery_y_, 0.0);
}

mrta::model::Ability TaskInfo::getAbilityRequirement() const { return ability_requirement_; }

}  // namespace daisi::cpps
