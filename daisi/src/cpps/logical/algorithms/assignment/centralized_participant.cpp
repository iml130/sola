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

#include "centralized_participant.h"

namespace daisi::cpps::logical {

CentralizedParticipant::CentralizedParticipant(
    daisi::cpps::common::CppsCommunicatorPtr communicator,
    std::shared_ptr<SimpleOrderManagement> task_management)
    : AssignmentParticipant(communicator), task_management_(std::move((task_management))){};

bool CentralizedParticipant::process(const AssignmentNotification &assignment_notification) {
  const material_flow::Task assigned_task = assignment_notification.getTask();
  if (task_management_->canAddTask(assigned_task)) {
    task_management_->addTask(assigned_task);
  } else {
    throw std::runtime_error("The task management should always accept the assigned task.");
  }

  // Respond to the central allocator. Send the acception / rejection as well as the current status.
  AssignmentResponse response(assigned_task.getUuid(), true, task_management_->getFinalMetrics(),
                              task_management_->getExpectedEndPosition(),
                              communicator_->network.getConnectionString());

  std::string initiator_connection = assignment_notification.getInitiatorConnection();
  communicator_->network.send({initiator_connection, serialize(response)});
  return true;
};

bool CentralizedParticipant::process(const StatusUpdateRequest &status_request) {
  StatusUpdate update(communicator_->network.getConnectionString(),
                      task_management_->getFinalMetrics(),
                      task_management_->getExpectedEndPosition());
  std::string initiator_connection = status_request.getInitiatorConnection();
  communicator_->network.send({initiator_connection, serialize(update)});

  return true;
};

}  // namespace daisi::cpps::logical
