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

namespace daisi::material_flow {

Task::Task(std::string name, const std::vector<Order> &orders,
           const std::vector<std::string> &follow_up_tasks)
    : name_(std::move(name)), orders_(orders), follow_up_tasks_(follow_up_tasks) {
  if (orders.empty()) {
    throw std::invalid_argument("Orders cannot be empty.");
  }

  generateUuid();
}

void Task::generateUuid() {
  std::vector<std::string> order_uuids;
  for (const auto &order : orders_) {
    if (auto mo = std::get_if<MoveOrder>(&order)) {
      order_uuids.push_back(mo->getUuid());
    } else if (auto ao = std::get_if<ActionOrder>(&order)) {
      order_uuids.push_back(ao->getUuid());
    } else if (auto to = std::get_if<TransportOrder>(&order)) {
      order_uuids.push_back(to->getUuid());
    } else {
      throw std::runtime_error("Order type not handled");
    }
  }

  auto it = order_uuids.begin();
  uuid_ = *it++;

  for (; it != order_uuids.end(); it++) {
    uuid_ += " " + *it;
  }
}

const std::string &Task::getUuid() const { return uuid_; }

const std::string &Task::getName() const { return name_; }

const std::vector<Order> &Task::getOrders() const { return orders_; }

const std::vector<std::string> &Task::getFollowUpTasks() const { return follow_up_tasks_; }

const std::vector<std::string> &Task::getPrecedingTasks() const { return preceding_tasks_; }

void Task::setPrecedingTasks(const std::vector<std::string> &preceding_tasks) {
  preceding_tasks_ = preceding_tasks;
}

bool Task::hasTimeWindow() const {
  return false;  // TODO integrate constraints
}

cpps::amr::AmrStaticAbility Task::getAbilityRequirement() const { return ability_requirement_; }

void Task::setAbilityRequirement(const cpps::amr::AmrStaticAbility &ability) {
  ability_requirement_ = ability;
}

}  // namespace daisi::material_flow
