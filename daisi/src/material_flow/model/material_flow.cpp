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

#include "material_flow.h"

#include <algorithm>
#include <cassert>

namespace daisi::material_flow {

void MFDLScheduler::processOrderUpdate(const daisi::cpps::logical::MaterialFlowUpdate &update) {
  log_order_state_update_fct_(update);
  auto it = std::find(tasks_.begin(), tasks_.end(), update.task);
  assert(it != tasks_.end());
  it->setOrderState(update.order_index, update.order_state);
}

bool MFDLScheduler::isFinished() const {
  for (const Task &task : tasks_) {
    const auto &orders = task.getOrders();
    const bool all_finished = std::all_of(orders.begin(), orders.end(), [](const Order &order) {
      return std::get<TransportOrder>(order).getOrderState() == cpps::OrderStates::kFinished;
    });
    if (!all_finished) return false;
  }
  return true;
}

void MFDLScheduler::addTask(Task task) { tasks_.push_back(std::move(task)); }

}  // namespace daisi::material_flow
