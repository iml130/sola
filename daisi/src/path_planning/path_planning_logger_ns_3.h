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

#ifndef DAISI_PATH_PLANNING_PATH_PLANNING_LOGGER_NS3_H_
#define DAISI_PATH_PLANNING_PATH_PLANNING_LOGGER_NS3_H_

#include "cpps/common/cpps_logger_ns3.h"
#include "logging/definitions.h"
#include "task.h"

namespace daisi::path_planning {
//! SQLite logger for path planning which extends \c daisi::cpps::CppsLoggerNs3 and its database
//! structure
class PathPlanningLoggerNs3 : public daisi::cpps::CppsLoggerNs3 {
public:
  PathPlanningLoggerNs3(LogDeviceApp log_device_application, LogFunction log);

  void logTOSpawn(const std::string &to_uuid, uint32_t station_id, uint32_t time_to_station);

  void logIntersectOccupancy(uint32_t from, uint32_t to, const std::string &agv_id,
                             const std::string &to_state, float intersect_x, float intersect_y,
                             double time);
  void logConsensusFinished(uint32_t instance, uint32_t station, const std::string &uuid);

  void logReplication(const std::string &id, uint32_t instance_id, uint32_t proposal_id,
                      uint32_t station_id);

  void logSendPathPlanningTraffic(const std::string &sender, const std::string &target,
                                  uint32_t msg_type);
  void logRecvPathPlanningTraffic(const std::string &sender, const std::string &target,
                                  uint32_t msg_type);
  void logPPTransportOrderUpdate(const std::string &order_uuid, uint32_t status,
                                 const std::string &agv = "");

  void logSendPathPlanningTopicTraffic(const std::string &topic, const std::string &node,
                                       uint32_t instance, uint32_t msg_type);
  void logRecvPathPlanningTopicTraffic(const std::string &topic, const std::string &node,
                                       uint32_t instance, uint32_t msg_type);
};
}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_PATH_PLANNING_LOGGER_NS3_H_
