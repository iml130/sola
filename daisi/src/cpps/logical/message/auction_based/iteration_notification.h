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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_AUCTION_BASED_ITERATION_NOTIFICATION_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_AUCTION_BASED_ITERATION_NOTIFICATION_H_

#include "solanet/serializer/serialize.h"

namespace daisi::cpps::logical {

class IterationNotification {
public:
  IterationNotification() = default;
  IterationNotification(std::string initiator_connection, std::vector<std::string> task_uuids)
      : initiator_connection_(std::move(initiator_connection)),
        task_uuids_(std::move(task_uuids)) {}

  const std::string &getInitiatorConnection() const { return initiator_connection_; }

  const std::vector<std::string> &getTaskUuids() const { return task_uuids_; }

  SERIALIZE(initiator_connection_, task_uuids_);

private:
  std::string initiator_connection_;

  std::vector<std::string> task_uuids_;
};

}  // namespace daisi::cpps::logical

#endif
