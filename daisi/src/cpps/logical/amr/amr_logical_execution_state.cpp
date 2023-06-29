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

const util::Position &AmrLogicalExecutionState::getPosition() const { return position_; }

const AmrState &AmrLogicalExecutionState::getAmrState() const { return amr_state_; }

const material_flow::Task &AmrLogicalExecutionState::getTask() const { return task_; }

const int &AmrLogicalExecutionState::getOrderIndex() const { return order_index_; }

const OrderStates &AmrLogicalExecutionState::getOrderState() const { return order_state_; }

bool AmrLogicalExecutionState::shouldSendNextTaskToPhysical() const {
  return send_next_task_to_physical_;
}

void AmrLogicalExecutionState::processAmrStatusUpdate(const AmrStatusUpdate &amr_status_update) {
  position_ = amr_status_update.getPosition();
  amr_state_ = amr_status_update.getState();

  if (amr_state_ == AmrState::kIdle) {
    send_next_task_to_physical_ = true;
  }
}

void AmrLogicalExecutionState::processAmrOrderUpdate(const AmrOrderUpdate &amr_order_update) {
  position_ = amr_order_update.getPosition();
  order_state_ = amr_order_update.getState();

  if (order_state_ == OrderStates::kFinished) {
    setNextOrder();
  }
}

void AmrLogicalExecutionState::setNextOrder() {
  order_index_++;
  order_state_ = OrderStates::kCreated;

  if (task_.getOrders().size() >= order_index_) {
    order_index_ = -1;
    if (amr_state_ == AmrState::kIdle) {
      send_next_task_to_physical_ = true;
    }
  }
}

void AmrLogicalExecutionState::setNextTask(const material_flow::Task &next_task) {
  task_ = next_task;
  order_index_ = 0;
  order_state_ = OrderStates::kCreated;

  send_next_task_to_physical_ = false;
}

}  // namespace daisi::cpps::logical
