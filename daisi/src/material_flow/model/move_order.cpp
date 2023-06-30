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

#include "move_order.h"

#include "cpps/common/uuid_generator.h"

namespace daisi::material_flow {

MoveOrder::MoveOrder(const MoveOrderStep &move_order_step)
    : uuid_(UUIDGenerator::get()()), move_order_step_(move_order_step) {}
MoveOrder::MoveOrder(std::string uuid, const MoveOrderStep &move_order_step)
    : uuid_(std::move(uuid)), move_order_step_(move_order_step) {}

const std::string &MoveOrder::getUuid() const { return uuid_; }

const MoveOrderStep &MoveOrder::getMoveOrderStep() const { return move_order_step_; }

bool MoveOrder::operator==(const MoveOrder &other) const {
  return uuid_ == other.uuid_;  // TODO && move_order_step_ == other.move_order_step_;
}

}  // namespace daisi::material_flow
