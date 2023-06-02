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

#ifndef DAISI_CPPS_MESSAGE_SSI_WINNER_RESPONSE_TYPES_H_
#define DAISI_CPPS_MESSAGE_SSI_WINNER_RESPONSE_TYPES_H_

#include <optional>

#include "cpps/message/serialize.h"
#include "cpps/model/task.h"

namespace daisi::cpps {

class SSIWinnerResponse {
public:
  SSIWinnerResponse() = default;
  SSIWinnerResponse(const std::string &order_uuid, const bool &accept,
                    const std::string &participant_connection);

  std::string getTaskUuid() const;
  bool doesAccept() const;
  std::string getParticipantConnection() const;

  std::string getLoggingContent() const;

  SERIALIZE(task_uuid_, accept_, participant_connection_);

private:
  std::string task_uuid_;
  bool accept_ = false;
  std::string participant_connection_;
};

}  // namespace daisi::cpps

#endif
