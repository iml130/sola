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

#include "paxos_replication_manager.h"

#include <utility>

#include "utils/daisi_check.h"

namespace daisi::path_planning::consensus {

PaxosReplicationManager::PaxosReplicationManager(std::string station_id,
                                                 std::shared_ptr<PathPlanningLoggerNs3> logger,
                                                 const PaxosSettings &settings)
    : station_id_(std::move(station_id)), logger_(std::move(logger)), settings_(settings) {}

void PaxosReplicationManager::processReplicationMessage(const ReplicationMessage &msg) {
  auto already_accepted = log_.find(msg.instance_id);
  DAISI_CHECK(already_accepted == log_.end(),
              "already accepted for this instance! Duplicate message");

  auto it = outstanding_data_.find({msg.instance_id, msg.proposal_id, msg.station_id});
  if (it != outstanding_data_.end()) {
    // Already received data for this instance, proposal and station
    it->second.remaining_replications--;
    if (it->second.remaining_replications == 0) {
      log_[msg.instance_id] = it->second.data;
      outstanding_data_.erase(it);
      logger_->logReplication(station_id_, msg.instance_id, log_[msg.instance_id].proposal_id,
                              log_[msg.instance_id].station_id);
      // TODO Clear unused data with same instance
    }
  } else {
    // First time that we receive this data
    // -2: Already received this message and proposer is not replicating
    uint32_t outstanding_replications = settings_.number_paxos_participants - 2;
    outstanding_data_[{msg.instance_id, msg.proposal_id, msg.station_id}] =
        UnfinishedData{{msg.proposal_id, msg.station_id}, outstanding_replications};
  }
}
}  // namespace daisi::path_planning::consensus
