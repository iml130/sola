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

#include "cpps/message/ssi_iteration_notification.h"

namespace daisi::cpps {

SSIIterationNotification::SSIIterationNotification(
    const std::vector<std::string> &auctioned_tasks_uuids, const std::string &initiator_connection)
    : auctioned_tasks_uuids_(auctioned_tasks_uuids), initiator_connection_(initiator_connection) {}

std::string SSIIterationNotification::getInitiatorConnection() const {
  return initiator_connection_;
}

std::vector<std::string> SSIIterationNotification::getAuctionedTasksUuids() const {
  return auctioned_tasks_uuids_;
}

std::string SSIIterationNotification::getLoggingContent() const {
  std::stringstream stream;
  stream << initiator_connection_;
  return stream.str();
}

}  // namespace daisi::cpps
