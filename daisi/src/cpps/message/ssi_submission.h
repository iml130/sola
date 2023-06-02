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

#ifndef DAISI_CPPS_MESSAGE_SSI_SUBMISSION_TYPES_H_
#define DAISI_CPPS_MESSAGE_SSI_SUBMISSION_TYPES_H_

#include <optional>

#include "cpps/message/serialize.h"
#include "cpps/model/ability.h"
#include "cpps/model/task.h"
#include "cpps/negotiation/utility/accumulated_utility_dimensions.h"
#include "cpps/negotiation/utility/utility_dimensions.h"

namespace daisi::cpps {

class SSISubmission {
public:
  SSISubmission() = default;
  SSISubmission(const std::string &order_uuid, const std::string &initiator_connection,
                const std::string &participant_connection,
                const mrta::model::Ability &participant_ability, const UtilityDimensions &udims);

  std::string getTaskUuid() const;
  std::string getInitiatorConnection() const;

  std::string getParticipantConnection() const;
  mrta::model::Ability getParticipantAbility() const;

  std::optional<UtilityDimensions> getUtilityDimensions() const;

  std::string getLoggingContent() const;

  SERIALIZE(task_uuid_, initiator_connection_, participant_connection_, participant_ability_,
            udims_);

private:
  std::string task_uuid_;
  std::string initiator_connection_;

  std::string participant_connection_;
  mrta::model::Ability participant_ability_;

  std::optional<UtilityDimensions> udims_;
};

}  // namespace daisi::cpps

#endif
