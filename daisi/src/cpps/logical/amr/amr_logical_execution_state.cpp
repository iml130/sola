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

#include "amr_logical_execution_state.h"

namespace daisi::cpps::logical {

const util::Position &AmrLogicalExecutionState::getPosition() const { return position; }

const AmrState &AmrLogicalExecutionState::getAmrState() const { return amr_state; }

const material_flow::Task &AmrLogicalExecutionState::getTask() const { return task; }

const int &AmrLogicalExecutionState::getOrderIndex() const { return order_index; }

const OrderStates &AmrLogicalExecutionState::getOrderState() const { return order_state; }

bool AmrLogicalExecutionState::shouldSendNextTaskToPhysical() const {
  return send_next_task_to_physical_;
}

void AmrLogicalExecutionState::processAmrStatusUpdate(const AmrStatusUpdate &amr_status_update) {
  position = amr_status_update.getPosition();
  amr_state = amr_status_update.getState();

  if (amr_state == AmrState::kIdle) {
    send_next_task_to_physical_ = true;
  }
}

void AmrLogicalExecutionState::processAmrOrderUpdate(const AmrOrderUpdate &amr_order_update) {
  position = amr_order_update.getPosition();
  order_state = amr_order_update.getState();

  if (order_state == OrderStates::kFinished) {
    setNextOrder();
  }
}

void AmrLogicalExecutionState::setNextOrder() {
  order_index++;
  order_state = OrderStates::kCreated;

  if (task.getOrders().size() >= order_index) {
    order_index = -1;
    if (amr_state == AmrState::kIdle) {
      send_next_task_to_physical_ = true;
    }
  }
}

void AmrLogicalExecutionState::setNextTask(const material_flow::Task &next_task) {
  task = next_task;
  order_index = 0;
  order_state = OrderStates::kCreated;

  send_next_task_to_physical_ = false;
}

}  // namespace daisi::cpps::logical
