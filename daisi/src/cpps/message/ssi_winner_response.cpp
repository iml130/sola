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

#include "cpps/message/ssi_winner_response.h"

namespace daisi::cpps {

SSIWinnerResponse::SSIWinnerResponse(const std::string &order_uuid, const bool &accept,
                                     const std::string &participant_connection)
    : task_uuid_(order_uuid), accept_(accept), participant_connection_(participant_connection) {}

std::string SSIWinnerResponse::getTaskUuid() const { return task_uuid_; }

bool SSIWinnerResponse::doesAccept() const { return accept_; }

std::string SSIWinnerResponse::getParticipantConnection() const { return participant_connection_; }

std::string SSIWinnerResponse::getLoggingContent() const {
  std::stringstream stream;

  stream << task_uuid_ << ";" << participant_connection_ << ";accept=" << accept_;

  return stream.str();
}

}  // namespace daisi::cpps
