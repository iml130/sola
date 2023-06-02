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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_CONSENSUS_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_CONSENSUS_H_

#include <memory>

#include "path_planning/consensus/consensus_base.h"
#include "paxos_acceptor.h"
#include "paxos_data.h"
#include "paxos_proposer.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::path_planning::consensus {
class PaxosConsensus final : public ConsensusBase<PaxosConsensus, PaxosContainer> {
public:
  PaxosConsensus(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola, uint32_t node_id,
                 PaxosSettings settings, std::shared_ptr<PathPlanningLoggerNs3> logger);

private:
  friend ConsensusBase;

  // CRTP implementation
  void findConsensusImpl(const PointTimePairs &points, double seconds_earliest_start,
                         std::function<void(uint32_t, double)> success_cb,
                         std::function<void(uint32_t)> fail_cb);
  void recvTopicMessageImpl(const std::string &topic, const std::string &msg);

  void processPaxosMessage(const std::string &topic, const std::string &msg_content);

  // Every paxos participant in this use case is proposer and acceptor at the same time
  PaxosProposer proposer_;
  PaxosAcceptor acceptor_;
};

}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_CONSENSUS_H_
