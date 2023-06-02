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

#ifndef DAISI_CPPS_MESSAGE_SSI_ITERATION_NOTIFICATION_TYPES_H_
#define DAISI_CPPS_MESSAGE_SSI_ITERATION_NOTIFICATION_TYPES_H_

#include "cpps/message/serialize.h"
#include "cpps/model/task.h"

namespace daisi::cpps {

class SSIIterationNotification {
public:
  SSIIterationNotification() = default;
  SSIIterationNotification(const std::vector<std::string> &auctioned_tasks_uuids,
                           const std::string &initiator_connection);

  std::vector<std::string> getAuctionedTasksUuids() const;

  std::string getInitiatorConnection() const;

  std::string getLoggingContent() const;

  SERIALIZE(auctioned_tasks_uuids_, initiator_connection_);

private:
  std::vector<std::string> auctioned_tasks_uuids_;

  std::string initiator_connection_;
};

}  // namespace daisi::cpps

#endif
