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
#include "cpps/amr/model/amr_static_ability.h"
#include "move_order.h"
#include "solanet/serializer/serialize.h"
#include "time_window.h"
#include "transport_order.h"

namespace daisi::material_flow {

using Order = std::variant<TransportOrder, MoveOrder, ActionOrder>;

class Task {
public:
  Task() = default;

  Task(std::string name, std::string connection_string, const std::vector<Order> &orders,
       const std::vector<std::string> &follow_up_task_uuids);

  const std::string &getUuid() const;
  const std::string &getName() const;
  const std::string &getConnectionString() const;

  const std::vector<Order> &getOrders() const;
  const std::vector<std::string> &getFollowUpTaskUuids() const;

  void setPrecedingTasks(const std::vector<std::string> &preceding_tasks);
  const std::vector<std::string> &getPrecedingTaskUuids() const;

  void setAbilityRequirement(const cpps::amr::AmrStaticAbility &ability);
  cpps::amr::AmrStaticAbility getAbilityRequirement() const;

  bool hasTimeWindow() const;
  void setTimeWindow(const TimeWindow &time_window);
  const TimeWindow &getTimeWindow() const;
  void setSpawnTime(const util::Duration &spawn_time);

  bool operator<(const Task &other) const { return uuid_ < other.uuid_; }
  bool operator==(const Task &other) const { return uuid_ == other.uuid_; }
  bool operator!=(const Task &other) const { return uuid_ != other.uuid_; }

  SERIALIZE(uuid_, name_, connection_string_, orders_, follow_up_task_uuids_, preceding_task_uuids_,
            ability_requirement_)

private:
  std::string uuid_;
  std::string name_;
  std::string connection_string_;

  std::vector<Order> orders_;

  std::vector<std::string> follow_up_task_uuids_;
  std::vector<std::string> preceding_task_uuids_;

  std::optional<TimeWindow> time_window_ = std::nullopt;

  cpps::amr::AmrStaticAbility ability_requirement_ =
      cpps::amr::AmrStaticAbility(cpps::amr::LoadCarrier(), 0);
};

}  // namespace daisi::material_flow

namespace std {

template <> struct hash<daisi::material_flow::Task> {
  std::size_t operator()(const daisi::material_flow::Task &task) const {
    string repr = task.getName();
    return hash<string>()(repr);
  }
};

}  // namespace std

#endif
