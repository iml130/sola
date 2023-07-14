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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_AUCTION_BASED_WINNER_RESPONSE_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_AUCTION_BASED_WINNER_RESPONSE_H_

#include "solanet/serializer/serialize.h"

namespace daisi::cpps::logical {

class WinnerResponse {
public:
  WinnerResponse() = default;
  WinnerResponse(std::string task_uuid, std::string participant_connection, bool accept)
      : task_uuid_(std::move(task_uuid)),
        participant_connection_(std::move(participant_connection)),
        accept_(accept){};

  const std::string &getTaskUuid() const { return task_uuid_; }

  const std::string &getParticipantConnection() const { return participant_connection_; }

  bool doesAccept() const { return accept_; }

  SERIALIZE(task_uuid_, accept_, participant_connection_);

private:
  std::string task_uuid_;

  std::string participant_connection_;

  bool accept_ = false;
};

}  // namespace daisi::cpps::logical

#endif
