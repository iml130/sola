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

#include "cpps/message/ssi_winner_notification.h"

namespace daisi::cpps {

SSIWinnerNotification::SSIWinnerNotification(const std::string &order_uuid,
                                             const double &latest_finish_time,
                                             const std::string &initiator_connection)
    : task_uuid_(order_uuid),
      latest_finish_time_(latest_finish_time),
      initiator_connection_(initiator_connection) {}

std::string SSIWinnerNotification::getTaskUuid() const { return task_uuid_; }

double SSIWinnerNotification::getOrderLatestFinishTime() const { return latest_finish_time_; }

std::string SSIWinnerNotification::getInitiatorConnection() const { return initiator_connection_; }

// std::string SSIWinnerNotification::getParticipantConnection() const {
//   return participant_connection_;
// }

std::string SSIWinnerNotification::getLoggingContent() const {
  std::stringstream stream;

  stream << task_uuid_ << ";" << latest_finish_time_ << ";" << initiator_connection_;

  return stream.str();
}

}  // namespace daisi::cpps
