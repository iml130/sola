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

#ifndef DAISI_MATERIAL_FLOW_TASK_H_
#define DAISI_MATERIAL_FLOW_TASK_H_

#include <variant>

#include "action_order.h"
#include "cpps/model/ability.h"
#include "move_order.h"
#include "solanet/serializer/serialize.h"
#include "transport_order.h"

namespace daisi::material_flow {

using Order = std::variant<TransportOrder, MoveOrder, ActionOrder>;

class Task {
public:
  Task() = default;

  Task(std::string name, const std::vector<Order> &orders,
       const std::vector<std::string> &follow_up_tasks);

  const std::string &getUuid() const;

  const std::string &getName() const;
  const std::vector<Order> &getOrders() const;
  const std::vector<std::string> &getFollowUpTasks() const;

  void setPrecedingTasks(const std::vector<std::string> &preceding_tasks);
  const std::vector<std::string> &getPrecedingTasks() const;

  bool hasTimeWindow() const;

  daisi::cpps::mrta::model::Ability getAbilityRequirement() const;

  bool operator<(const Task &other) const { return name_ < other.name_; }

  bool operator==(const Task &other) const { return name_ == other.name_; }

  bool operator!=(const Task &other) const { return name_ != other.name_; }

  SERIALIZE(uuid_, name_, orders_, follow_up_tasks_, preceding_tasks_);

private:
  std::string uuid_ = "TODO";

  std::string name_;
  std::vector<Order> orders_;
  std::vector<std::string> follow_up_tasks_;

  std::vector<std::string> preceding_tasks_;
};

}  // namespace daisi::material_flow

// bool operator==(const Order &lhs, const Order &rhs);

namespace std {

template <> struct hash<daisi::material_flow::Task> {
  std::size_t operator()(const daisi::material_flow::Task &task) const {
    string repr = task.getName();
    return hash<string>()(repr);
  }
};

}  // namespace std

#endif
