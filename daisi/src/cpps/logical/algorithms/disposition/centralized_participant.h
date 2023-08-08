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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_CENTRALIZED_PARTICIPANT_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_CENTRALIZED_PARTICIPANT_H_

#include "cpps/logical/order_management/simple_order_management.h"
#include "disposition_participant.h"

namespace daisi::cpps::logical {

/// @brief Participant of a centralized task assignment strategy. Since the task assignment strategy
/// is defined by the central allocator, the participant only needs to handle new task assignments.
class CentralizedParticipant : public DispositionParticipant {
public:
  CentralizedParticipant(std::shared_ptr<SOLACppsWrapper> sola,
                         std::shared_ptr<SimpleOrderManagement> order_management);
  ~CentralizedParticipant() override = default;

  /// @brief React on new task assignment and respond to it.
  REGISTER_IMPLEMENTATION(AssignmentNotification)

  /// @brief Send a status update as a reaction.
  REGISTER_IMPLEMENTATION(StatusUpdateRequest)

private:
  /// @brief the AMR's order management. Simply accepts a new task assignment.
  std::shared_ptr<SimpleOrderManagement> order_management_;
};
}  // namespace daisi::cpps::logical

#endif
