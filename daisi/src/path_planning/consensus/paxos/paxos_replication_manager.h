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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_REPLICATION_MANAGER_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_REPLICATION_MANAGER_H_

#include <memory>

#include "path_planning/consensus/paxos/message/replication_message.h"
#include "path_planning/path_planning_logger_ns_3.h"
#include "paxos_settings.h"

namespace daisi::path_planning::consensus {
//! Replication manager that receives replication messages and once it received the same message
//! from all acceptors it will store the entry in a ledger.
class PaxosReplicationManager {
public:
  PaxosReplicationManager(std::string station_id, std::shared_ptr<PathPlanningLoggerNs3> logger,
                          const PaxosSettings &settings);

  void processReplicationMessage(const ReplicationMessage &msg);

private:
  const std::string station_id_;
  std::shared_ptr<PathPlanningLoggerNs3> logger_;
  const PaxosSettings settings_;

  struct Data {
    uint32_t proposal_id;
    uint32_t station_id;
    // TO BE EXTENDED
  };

  //! Data that was already received but not from all acceptors
  struct UnfinishedData {
    Data data;
    uint32_t remaining_replications;
  };

  using InstanceID = uint32_t;
  using InstanceProposalStationTuple = std::tuple<InstanceID, uint32_t, uint32_t>;

  struct TupleHash {
    std::size_t operator()(const InstanceProposalStationTuple &tuple) const {
      return std::hash<uint32_t>()(std::get<0>(tuple)) ^ std::hash<uint32_t>()(std::get<1>(tuple)) ^
             std::hash<uint32_t>()(std::get<2>(tuple));
    }
  };

  std::unordered_map<InstanceID, Data> log_;  //!< Final ledger with data for each instance
  std::unordered_map<InstanceProposalStationTuple, UnfinishedData, TupleHash> outstanding_data_;
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_REPLICATION_MANAGER_H_
