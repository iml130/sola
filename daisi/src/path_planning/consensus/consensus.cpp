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

#include "consensus.h"

#include <utility>

#include "utils/daisi_check.h"

namespace daisi::path_planning::consensus {
Consensus::Consensus(ConsensusSettings settings,
                     const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola, uint32_t node_id,
                     std::shared_ptr<PathPlanningLoggerNs3> logger) {
  // Create and initialize the consensus algorithm instance
  if (auto paxos = std::get_if<PaxosSettings>(&settings)) {
    consensus_.emplace<PaxosConsensus>(sola, node_id, *paxos, std::move(logger));
  } else if (auto central = std::get_if<CentralSettings>(&settings)) {
    consensus_.emplace<CentralParticipant>(sola, *central, std::move(logger));
  }
  DAISI_CHECK(!std::holds_alternative<std::monostate>(consensus_),
              "Consensus algorithm not initialized");
}
void Consensus::findConsensus(const PointTimePairs &points, double seconds_earliest_start,
                              std::function<void(uint32_t, double)> success_cb,
                              std::function<void(uint32_t)> fail_cb) {
  // Pass call to consensus algorithm
  if (auto *paxos = std::get_if<PaxosConsensus>(&consensus_)) {
    paxos->findConsensus(points, seconds_earliest_start, std::move(success_cb), std::move(fail_cb));
  } else if (auto *central = std::get_if<CentralParticipant>(&consensus_)) {
    central->findConsensus(points, seconds_earliest_start, std::move(success_cb),
                           std::move(fail_cb));
  } else {
    throw std::runtime_error("unsupported consensus type");
  }
}
void Consensus::recvTopicMessage(const std::string &topic, const std::string &msg) {
  // Only paxos uses topic messages so far
  if (auto *paxos = std::get_if<PaxosConsensus>(&consensus_)) {
    paxos->recvTopicMessage(topic, msg);
  } else {
    throw std::runtime_error("unsupported consensus type");
  }
}
}  // namespace daisi::path_planning::consensus
