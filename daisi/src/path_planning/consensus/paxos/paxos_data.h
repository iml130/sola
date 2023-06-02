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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_DATA_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_DATA_H_

#include <vector>

#include "path_planning/constants.h"
#include "path_planning/path_planning_logger_ns_3.h"
#include "path_planning/station.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::path_planning::consensus {

using InstanceID = uint32_t;
using RequestID = std::pair<uint32_t, uint32_t>;

struct AcceptedProposal {
  uint32_t instance_id = 0;
  uint32_t proposal_id = 0;
  uint32_t station_id = 0;
  std::vector<IntersectionTimeInfo> data;

  [[nodiscard]] bool initialized() const { return !data.empty(); }
};

struct PaxosContainer {
  PaxosContainer(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola, uint32_t node_id,
                 PaxosSettings settings, std::shared_ptr<PathPlanningLoggerNs3> logger)
      : sola(std::move(sola)),
        node_id(node_id),
        settings(std::move(settings)),
        logger(std::move(logger)) {}
  const std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola{};
  const uint32_t node_id;
  const PaxosSettings settings;
  std::shared_ptr<PathPlanningLoggerNs3> logger;

  std::unordered_map<InstanceID, RequestID>
      instance_to_promised;  //!< map of an instance ID to the currently promised proposal id and
                             //!< station id

  AcceptedProposal accepted_data_current_instance;  //!< The current accepted outstanding data for
                                                    //!< the latest instance (currently in OK phase)

  std::vector<std::vector<IntersectionTimeInfo>>
      finally_accepted_data;  //!< List of instances with their finally accepted occupant for every
                              //!< intersection

  IntersectionOccupancy
      agreed_data;  //!< Global occupancy of all intersections (same data as in \p
                    //!< finally_accepted_data) needed to calculate possible start times
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_DATA_H_
