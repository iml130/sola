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

#include "cpps/message/ssi_call_for_proposal.h"

namespace daisi::cpps {

SSICallForProposal::SSICallForProposal(const std::vector<Task> &tasks,
                                       const std::string &initiator_connection)
    : tasks_(tasks), initiator_connection_(initiator_connection) {}

std::vector<Task> SSICallForProposal::getTasks() const { return tasks_; }

std::string SSICallForProposal::getInitiatorConnection() const { return initiator_connection_; }

std::string SSICallForProposal::getLoggingContent() const {
  std::stringstream stream;

  stream << initiator_connection_;
  for (auto const &o : tasks_) {
    stream << ";" << o.getUUID();
  }

  return stream.str();
}

}  // namespace daisi::cpps
