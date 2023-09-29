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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_CENTRAL_ALLOCATION_STATUS_UPDATE_REQUEST_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_CENTRAL_ALLOCATION_STATUS_UPDATE_REQUEST_H_

#include <string>

#include "solanet/serializer/serialize.h"
#include "solanet/uuid.h"
#include "solanet/uuid_generator.h"

namespace daisi::cpps::logical {

/// might need some revision / additions in the future
// A request to the receiving central participant to send its current status.
class StatusUpdateRequest {
public:
  StatusUpdateRequest() = default;
  explicit StatusUpdateRequest(std::string initiator_connection)
      : initiator_connection_(std::move(initiator_connection)) {}

  const std::string &getInitiatorConnection() const { return initiator_connection_; }

  solanet::UUID getUUID() const { return uuid_; }

  SERIALIZE(uuid_, initiator_connection_);

private:
  solanet::UUID uuid_ = solanet::generateUUID();

  std::string initiator_connection_;
};
}  // namespace daisi::cpps::logical

#endif
