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

#include "action_order.h"

#include "cpps/common/uuid_generator.h"

namespace daisi::material_flow {

ActionOrder::ActionOrder(const ActionOrderStep &action_order_step)
    : uuid_(UUIDGenerator::get()()), action_order_step_(action_order_step) {}

ActionOrder::ActionOrder(std::string uuid, const ActionOrderStep &action_order_step)
    : uuid_(std::move(uuid)), action_order_step_(action_order_step) {}

const std::string &ActionOrder::getUuid() const { return uuid_; }

const ActionOrderStep &ActionOrder::getActionOrderStep() const { return action_order_step_; }

bool ActionOrder::operator==(const ActionOrder &other) const {
  return uuid_ == other.uuid_;  // TODO && action_order_step_ == other.action_order_step_;
}

}  // namespace daisi::material_flow
