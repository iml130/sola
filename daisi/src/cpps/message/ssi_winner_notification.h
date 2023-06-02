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

#ifndef DAISI_CPPS_MESSAGE_SSI_WINNER_NOTIFICATION_TYPES_H_
#define DAISI_CPPS_MESSAGE_SSI_WINNER_NOTIFICATION_TYPES_H_

#include "cpps/message/serialize.h"
#include "cpps/model/task.h"

namespace daisi::cpps {

class SSIWinnerNotification {
public:
  SSIWinnerNotification() = default;
  SSIWinnerNotification(const std::string &order_uuid, const double &latest_finish_time,
                        const std::string &initiator_connection);

  std::string getTaskUuid() const;
  double getOrderLatestFinishTime() const;
  std::string getInitiatorConnection() const;
  //   std::string getParticipantConnection() const;

  std::string getLoggingContent() const;

  SERIALIZE(task_uuid_, latest_finish_time_, initiator_connection_);

private:
  std::string task_uuid_;
  double latest_finish_time_ = 0.0;
  std::string initiator_connection_;
  // std::string participant_connection_;
};

}  // namespace daisi::cpps

#endif
