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

#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"

namespace daisi::material_flow {

Task::Task(std::string name, std::string connection_string, const std::vector<Order> &orders,
           const std::vector<std::string> &follow_up_task_uuids)
    : name_(std::move(name)),
      connection_string_(std::move(connection_string)),
      orders_(orders),
      follow_up_task_uuids_(std::move(follow_up_task_uuids)) {
  if (orders.empty()) {
    throw std::invalid_argument("Orders cannot be empty.");
  }

  uuid_ = solanet::uuidToString(solanet::generateUUID());
}

const std::string &Task::getUuid() const { return uuid_; }

const std::string &Task::getName() const { return name_; }

const std::string &Task::getConnectionString() const { return connection_string_; }

const std::vector<Order> &Task::getOrders() const { return orders_; }

const std::vector<std::string> &Task::getFollowUpTaskUuids() const { return follow_up_task_uuids_; }

const std::vector<std::string> &Task::getPrecedingTaskUuids() const {
  return preceding_task_uuids_;
}

void Task::setPrecedingTasks(const std::vector<std::string> &preceding_tasks) {
  preceding_task_uuids_ = preceding_tasks;
}

cpps::amr::AmrStaticAbility Task::getAbilityRequirement() const { return ability_requirement_; }

void Task::setAbilityRequirement(const cpps::amr::AmrStaticAbility &ability) {
  ability_requirement_ = ability;
}

bool Task::hasTimeWindow() const { return time_window_.has_value(); }

void Task::setTimeWindow(const TimeWindow &time_window) { time_window_ = time_window; }

const TimeWindow &Task::getTimeWindow() const { return time_window_.value(); }

void Task::setSpawnTime(const util::Duration &spawn_time) {
  if (hasTimeWindow()) {
    time_window_.value().setSpawnTime(spawn_time);
  }
  throw std::runtime_error("TimeWindow not set");
}

void Task::setOrderState(uint8_t order_index, daisi::cpps::OrderStates state) {
  auto &order = orders_.at(order_index);
  std::visit([this, state](auto &ord) { this->setOrderState(ord, state); }, order);
}

void Task::setOrderState(TransportOrder &order, daisi::cpps::OrderStates state) {
  order.setOrderState(state);
}

void Task::setOrderState(MoveOrder & /*order*/, daisi::cpps::OrderStates /*state*/) {
  throw std::runtime_error("not supported yet");
}

void Task::setOrderState(ActionOrder & /*order*/, daisi::cpps::OrderStates /*state*/) {
  throw std::runtime_error("not supported yet");
}

}  // namespace daisi::material_flow
