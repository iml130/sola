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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_H_

#include "path_planning/consensus/central/central_participant.h"
#include "path_planning/consensus/consensus_types.h"
#include "path_planning/consensus/constants.h"
#include "path_planning/consensus/paxos/constants.h"
#include "path_planning/consensus/paxos/paxos_consensus.h"
#include "path_planning/constants.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::path_planning::consensus {

//! Interface for different consensus algorithms. This class creates the wanted consensus algorithm
//! instance and forwards all calls to the algorithm
class Consensus {
public:
  Consensus(consensus::ConsensusSettings settings,
            const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola, uint32_t node_id,
            std::shared_ptr<PathPlanningLoggerNs3> logger);

  /**
   * Try to get an consensus/clearance for the route with given parameters
   * @param points intersections with corresponding relative timestamp (e.g. first waypoint (station
   * out) will be reached in 0 seconds)
   * @param seconds_earliest_start seconds after calling this function, at which the AGV could be
   * start driving
   * @param success_cb callback that will be called when getting a consensus succeeded, passing the
   * global start time
   * @param fail_cb callback that will be called when getting a consensus failed
   */
  void findConsensus(const PointTimePairs &points, double seconds_earliest_start,
                     std::function<void(uint32_t id, double start_time)> success_cb,
                     std::function<void(uint32_t id)> fail_cb);

  /**
   * Arriving topic messages must be passed from the station to the consensus algorithm, because the
   * consensus interface has no access to the middleware
   * @param topic
   * @param msg
   */
  void recvTopicMessage(const std::string &topic, const std::string &msg);

private:
  std::variant<std::monostate, PaxosConsensus,
               CentralParticipant>
      consensus_;  //!< Variant holding all possible consensus algorithm instances
};
}  // namespace daisi::path_planning::consensus
#endif  // DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_H_
