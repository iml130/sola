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
    : name_(std::move(name)), orders_(orders), follow_up_tasks_(follow_up_tasks) {}

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

cpps::amr::AmrStaticAbility Task::getAbilityRequirement() const {
  // TODO
  cpps::amr::AmrStaticAbility ability(
      cpps::amr::LoadCarrier(cpps::amr::LoadCarrier::Types::kNoLoadCarrierType), 0);
  return ability;
}

}  // namespace daisi::material_flow
