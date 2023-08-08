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
    std::shared_ptr<SOLACppsWrapper> sola, std::shared_ptr<SimpleOrderManagement> order_management)
    : DispositionParticipant(sola), order_management_(std::move((order_management))){};

bool CentralizedParticipant::process(const AssignmentNotification &assignment_notification) {
  const material_flow::Task assigned_task = assignment_notification.getTask();
  if (order_management_->canAddTask(assigned_task)) {
    order_management_->addTask(assigned_task);
  } else {
    throw std::runtime_error("The order management should always accept the assigned task.");
  }

  // Respond to the central allocator. Send the acception / rejection as well as the current status.
  AssignmentResponse response(assigned_task.getUuid(), true, order_management_->getFinalMetrics(),
                              order_management_->getExpectedEndPosition(),
                              sola_->getConectionString());

  std::string initiator_connection = assignment_notification.getInitiatorConnection();
  sola_->sendData(serialize(response), sola::Endpoint(initiator_connection));
  return true;
};

bool CentralizedParticipant::process(const StatusUpdateRequest &status_request) {
  StatusUpdate update(sola_->getConectionString(), order_management_->getFinalMetrics(),
                      order_management_->getExpectedEndPosition());
  std::string initiator_connection = status_request.getInitiatorConnection();
  sola_->sendData(serialize(update), sola::Endpoint(initiator_connection));

  return true;
};

}  // namespace daisi::cpps::logical
