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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_PARTICIPANT_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_PARTICIPANT_H_

#include "path_planning/consensus/central/central_settings.h"
#include "path_planning/consensus/consensus_base.h"
#include "path_planning/path_planning_logger_ns_3.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::path_planning::consensus {
//! Participant/Client in the centralized client-server occupancy management
class CentralParticipant : public ConsensusBase<CentralParticipant, CentralSettings> {
public:
  CentralParticipant(const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
                     const CentralSettings &settings,
                     std::shared_ptr<PathPlanningLoggerNs3> logger);

private:
  friend ConsensusBase;

  // CRTP implementation
  void findConsensusImpl(const PointTimePairs &points, double seconds_earliest_start,
                         std::function<void(uint32_t, double)> success_cb,
                         std::function<void(uint32_t)> fail_cb);
  void recvTopicMessageImpl(const std::string &topic, const std::string &msg);

  /**
   * Process messages received from central server
   * @param msg
   */
  void processMessage(const solanet::Message &msg);

  std::unique_ptr<solanet::Network>
      network_;  //!< Network interface to communicate with central server. We cannot use SOLA
                 //!< (passed in ctor) because SOLA 1:1 messages cannot be forwarded to us from the
                 //!< pickup station.
  std::shared_ptr<PathPlanningLoggerNs3> logger_;

  uint32_t next_request_id_ = 0;  //!< Per station unique ID to match requests and responses

  //! Data of a request
  struct CurrentRequest {
    uint32_t request_id;
    std::function<void(uint32_t, double)> success_cb;
    std::function<void(uint32_t)> fail_cb;
    std::vector<std::tuple<float, float, double>> intersection_times;
  };

  std::unordered_map<uint32_t, CurrentRequest>
      requests_;  //!< Map of request IDs to content of requests
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_CENTRAL_PARTICIPANT_H_
