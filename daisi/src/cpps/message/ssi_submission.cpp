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

#include "cpps/message/ssi_submission.h"

namespace daisi::cpps {

SSISubmission::SSISubmission(const std::string &order_uuid, const std::string &initiator_connection,
                             const std::string &participant_connection,
                             const mrta::model::Ability &participant_ability,
                             const UtilityDimensions &udims)
    : task_uuid_(order_uuid),
      initiator_connection_(initiator_connection),
      participant_connection_(participant_connection),
      participant_ability_(participant_ability),
      udims_(udims) {}

std::string SSISubmission::getTaskUuid() const { return task_uuid_; }

std::string SSISubmission::getInitiatorConnection() const { return initiator_connection_; }

std::string SSISubmission::getParticipantConnection() const { return participant_connection_; }

mrta::model::Ability SSISubmission::getParticipantAbility() const { return participant_ability_; }

std::optional<UtilityDimensions> SSISubmission::getUtilityDimensions() const { return udims_; }

std::string SSISubmission::getLoggingContent() const {
  std::stringstream stream;

  stream << task_uuid_ << ";" << initiator_connection_ << ";" << participant_connection_ << ";";
  printAbility(stream, participant_ability_);
  stream << ";" << task_uuid_;

  return stream.str();
}

}  // namespace daisi::cpps
