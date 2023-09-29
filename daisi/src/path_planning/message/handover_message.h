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

#ifndef DAISI_PATH_PLANNING_MESSAGE_HANDOVER_MESSAGE_H_
#define DAISI_PATH_PLANNING_MESSAGE_HANDOVER_MESSAGE_H_

#include <utility>

#include "drive_message.h"
#include "solanet/serializer/serialize.h"

namespace daisi::path_planning::message {

//! Message that is send from \c PickupStation to logical AGV with drive instructions and
//! information about a pickup station. The AGV should change its authority to this station.
class HandoverMessage {
public:
  HandoverMessage() = default;

  HandoverMessage(const DriveMessage &drive, uint32_t next_station, std::string next_station_ip)
      : drive_(drive), next_station_(next_station), next_station_ip_(std::move(next_station_ip)){};

  [[nodiscard]] uint32_t getNextStation() const { return next_station_; }
  [[nodiscard]] std::string getNextStationIP() const { return next_station_ip_; }
  [[nodiscard]] DriveMessage getDriveMessage() const { return drive_; }

  SERIALIZE(drive_, next_station_, next_station_ip_);

private:
  DriveMessage drive_;
  uint32_t next_station_ = 0;
  std::string next_station_ip_;
};

}  // namespace daisi::path_planning::message

#endif  // DAISI_PATH_PLANNING_MESSAGE_HANDOVER_MESSAGE_H_
