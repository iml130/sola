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

#ifndef DAISI_CPPS_MESSAGE_SSI_CALL_FOR_PROPOSAL_TYPES_H_
#define DAISI_CPPS_MESSAGE_SSI_CALL_FOR_PROPOSAL_TYPES_H_

#include <unordered_map>
#include <vector>

#include "cpps/model/task.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {

class SSICallForProposal {
public:
  SSICallForProposal() = default;
  SSICallForProposal(const std::vector<Task> &tasks, const std::string &initiator_connection);

  std::vector<Task> getTasks() const;
  std::string getInitiatorConnection() const;

  std::string getLoggingContent() const;

  SERIALIZE(tasks_, initiator_connection_);

private:
  std::vector<Task> tasks_;
  std::string initiator_connection_;
};

}  // namespace daisi::cpps

#endif
