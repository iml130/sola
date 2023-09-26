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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_CENTRAL_ALLOCATION_STATUS_UPDATE_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_CENTRAL_ALLOCATION_STATUS_UPDATE_H_

#include <string>

#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/logical/task_management/metrics_composition.h"
#include "solanet/serializer/serialize.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

/// might need some revision / additions in the future
// Message that consists information about a participant's status.
class StatusUpdate {
public:
  StatusUpdate() = default;
  StatusUpdate(std::string participant_connection, const Metrics &metrics,
               const util::Position end_position)
      : participant_connection_(std::move(participant_connection)),
        metrics_(metrics),
        end_position_(end_position) {}

  const std::string &getParticipantConnection() const { return participant_connection_; }
  const Metrics &getMetrics() { return metrics_; }
  const util::Position getEndPosition() { return end_position_; }

  solanet::UUID getUUID() const { return uuid_; }

  SERIALIZE(uuid_, participant_connection_, metrics_, end_position_)

private:
  solanet::UUID uuid_ = solanet::generateUUID();

  std::string participant_connection_;
  Metrics metrics_;
  util::Position end_position_;
};
}  // namespace daisi::cpps::logical

#endif
