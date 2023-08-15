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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_CENTRAL_ALLOCATION_ASSIGNMENT_RESPONSE_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_CENTRAL_ALLOCATION_ASSIGNMENT_RESPONSE_H_

#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/logical/order_management/metrics_composition.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps::logical {

/// @brief Response of a central participant as a reaction of a task assignment. Consists of the
/// task_uuid, the acception / rejection, its connection string and of the new current status.
class AssignmentResponse {
public:
  AssignmentResponse() = default;
  AssignmentResponse(std::string task_uuid, bool accept, const Metrics &metrics,
                     util::Position position, std::string participant_connection)
      : task_uuid_(std::move(task_uuid)),
        accept_(accept),
        metrics_(metrics),
        end_position_(position),
        participant_connection_(std::move(participant_connection)) {}

  const std::string &getTaskUuid() const { return task_uuid_; }

  const std::string &getParticipantConnection() const { return participant_connection_; }

  bool doesAccept() const { return accept_; }

  SERIALIZE(task_uuid_, accept_, metrics_, end_position_, participant_connection_)

private:
  std::string task_uuid_;

  bool accept_;

  Metrics metrics_;
  util::Position end_position_;

  std::string participant_connection_;
};

}  // namespace daisi::cpps::logical

#endif
