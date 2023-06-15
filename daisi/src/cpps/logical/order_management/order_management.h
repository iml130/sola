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

#ifndef DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_ORDER_MANAGEMENT_H_
#define DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_ORDER_MANAGEMENT_H_

#include <algorithm>
#include <memory>
#include <optional>
#include <utility>

#include "material_flow/model/task.h"
#include "metrics_composition.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::order_management {

class OrderManagement {
public:
  struct InsertionPoint {};

  explicit OrderManagement(const AmrDescription &amr_description, const Topology &topology,
                           const daisi::util::Pose &pose)
      : amr_description_(amr_description), topology_(topology), current_pose_(pose) {}

  virtual ~OrderManagement() = default;

  virtual bool hasTasks() const = 0;
  virtual daisi::material_flow::Task getCurrentTask() const = 0;
  virtual bool setNextTask() = 0;

  virtual std::optional<std::pair<MetricsComposition, std::shared_ptr<InsertionPoint>>> canAddTask(
      const daisi::material_flow::Task &task) const = 0;
  virtual std::optional<std::pair<MetricsComposition, std::shared_ptr<InsertionPoint>>> addTask(
      const daisi::material_flow::Task &task,
      std::shared_ptr<InsertionPoint> insertion_point = nullptr) = 0;

protected:
  AmrDescription amr_description_;
  Topology topology_;
  daisi::util::Pose current_pose_;
};

}  // namespace daisi::cpps::order_management

#endif
