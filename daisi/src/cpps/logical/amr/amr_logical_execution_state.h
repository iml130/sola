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

#ifndef DAISI_CPPS_LOGICAL_AMR_AMR_LOGICAL_EXECUTION_STATE_H_
#define DAISI_CPPS_LOGICAL_AMR_AMR_LOGICAL_EXECUTION_STATE_H_

#include "cpps/amr/message/amr_order_update.h"
#include "cpps/amr/message/amr_status_update.h"
#include "material_flow/model/task.h"

namespace daisi::cpps::logical {

class AmrLogicalExecutionState {
public:
  void processAmrStatusUpdate(const AmrStatusUpdate &amr_status_update);
  void processAmrOrderUpdate(const AmrOrderUpdate &amr_order_update);
  bool shouldSendNextTaskToPhysical() const;

  const util::Position &getPosition() const;
  const AmrState &getAmrState() const;
  const material_flow::Task &getTask() const;
  const int &getOrderIndex() const;
  const OrderStates &getOrderState() const;

  void setNextTask(const material_flow::Task &next_task);

private:
  void setNextOrder();

  util::Position position_;
  AmrState amr_state_ = AmrState::kIdle;
  material_flow::Task task_;
  int order_index_ = -1;
  OrderStates order_state_ = OrderStates::kCreated;

  bool send_next_task_to_physical_ = true;
};

}  // namespace daisi::cpps::logical

#endif
