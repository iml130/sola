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

#include "task.h"

#include <algorithm>

#include "cpps/common/uuid_generator.h"

namespace daisi::path_planning {

std::ostream &operator<<(std::ostream &os, const Task &task) {
  // os << "OrderId: {" << task.getUuid() << "}, From: {" << task.getPickupLocation() << "}, To: {"
  //    << tas.getDeliveryLocation() << "}";
  return os;
}

Task::Task(const ns3::Vector &from, const ns3::Vector &to)
    : Task(UUIDGenerator::get()(), from, to) {}

Task::Task(const std::string &uuid, const ns3::Vector &from, const ns3::Vector &to)
    : uuid_(uuid), current_pos_(from) {
  from_x_ = from.x;
  from_y_ = from.y;

  to_x_ = to.x;
  to_y_ = to.y;

  order_state_ = cpps::OrderStates::kCreated;
}

cpps::OrderStates Task::getOrderState() const { return order_state_; }

void Task::setOrderState(const cpps::OrderStates &state) { order_state_ = state; }

ns3::Vector Task::getPickupLocation() const { return ns3::Vector(from_x_, from_y_, 0); }

ns3::Vector Task::getDeliveryLocation() const { return ns3::Vector(to_x_, to_y_, 0); }

ns3::Vector Task::getCurrentPosition() const { return current_pos_; };

void Task::setCurrentPosition(const ns3::Vector &currentPosition) {
  current_pos_ = currentPosition;
}

std::string Task::getUuid() const { return uuid_; }

void Task::setConnection(const std::string &connection) { connection_ = connection; }

std::string Task::getConnection() const { return connection_; }

void Task::setName(const std::string &name) { name_ = name; }

std::string Task::getName() const { return name_; }

}  // namespace daisi::path_planning
