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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_SERVER_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_SERVER_H_

#include <memory>
#include <set>

#include "cpps/common/cpps_logger_ns3.h"
#include "path_planning/constants.h"
#include "path_planning/path_planning_logger_ns_3.h"
#include "solanet/network_udp/network_udp.h"

namespace daisi::path_planning::consensus {
//! Central server that processes occupancy requests from clients by managing all intersection
//! occupancies centralized, calculate possible start times and can therefore guarantee conflict
//! free intersection occupancies.
class CentralServer {
public:
  CentralServer(CentralSettings settings, std::shared_ptr<PathPlanningLoggerNs3> logger);

  /**
   * Set the intersections that this server is handling
   * @param intersections all global intersections
   */
  void setIntersections(const std::set<PPVector> &intersections);

private:
  std::unique_ptr<solanet::Network> network_;  //!< Network interface to communicate with clients

  void processMessage(const solanet::Message &msg);

  IntersectionOccupancy
      intersection_occupancy_;  //!< Global intersection occupancy (with the centralized approach,
                                //!< this is only known by the server)

  CentralSettings settings_;
  std::shared_ptr<PathPlanningLoggerNs3> logger_;
};

}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_SERVER_H_
