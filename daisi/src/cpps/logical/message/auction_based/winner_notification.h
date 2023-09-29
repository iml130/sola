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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_AUCTION_BASED_WINNER_NOTIFICATION_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_AUCTION_BASED_WINNER_NOTIFICATION_H_

#include <string>

#include "solanet/serializer/serialize.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

class WinnerNotification {
public:
  WinnerNotification() = default;
  WinnerNotification(std::string task_uuid, std::string initiator_connection,
                     double latest_finish_time)
      : task_uuid_(std::move(task_uuid)),
        initiator_connection_(std::move(initiator_connection)),
        latest_finish_time_(latest_finish_time) {}

  const std::string &getTaskUuid() const { return task_uuid_; }

  const std::string &getInitiatorConnection() const { return initiator_connection_; }

  const daisi::util::Duration &getLatestFinishTime() const { return latest_finish_time_; }

  solanet::UUID getUUID() const { return uuid_; }

  SERIALIZE(uuid_, task_uuid_, initiator_connection_, latest_finish_time_);

private:
  solanet::UUID uuid_ = solanet::generateUUID();

  std::string task_uuid_;

  std::string initiator_connection_;

  daisi::util::Duration latest_finish_time_;
};

}  // namespace daisi::cpps::logical

#endif
