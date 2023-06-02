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

#ifndef DAISI_PATH_PLANNING_DELIVERY_STATION_H_
#define DAISI_PATH_PLANNING_DELIVERY_STATION_H_

#include "agv_info.h"
#include "agv_queue.h"
#include "ns3/vector.h"
#include "path_planning/consensus/consensus_types.h"
#include "path_planning/consensus/constants.h"
#include "path_planning/consensus/paxos/message/accept_message.h"
#include "path_planning/consensus/paxos/message/ok_message.h"
#include "path_planning/consensus/paxos/message/prepare_message.h"
#include "path_planning/consensus/paxos/message/promise_message.h"
#include "path_planning/consensus/paxos/paxos_replication_manager.h"
#include "path_planning/path_planning_logger_ns_3.h"
#include "sola-ns3/sola_ns3_wrapper.h"
#include "station.h"

namespace daisi::path_planning {

//! Logical agent of station that processes inbound transport orders. It can store replications of
//! Paxos instances
class DeliveryStation {
public:
  DeliveryStation(DeliveryStationInfo info, consensus::ConsensusSettings consensus_settings,
                  std::shared_ptr<PathPlanningLoggerNs3> logger, uint32_t device_id);
  void postInit();

  std::string getConnectionString() const { return sola_->getConectionString(); }

  DeliveryStationInfo getStationInfo() const { return info_; }

  std::string getFullName() const;

private:
  const uint32_t device_id_ = 0;
  DeliveryStationInfo info_;

  consensus::ConsensusSettings consensus_settings_;
  std::optional<consensus::PaxosReplicationManager> replication_manager_;

  std::shared_ptr<PathPlanningLoggerNs3> logger_;
  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;
  void processTopicMessage(const sola::TopicMessage &message);

  //! Initialize replication by subscribing to relevant topic
  void initReplication();

  //! Initialize by start listening for inbound physical AGV connections and starting SOLA
  void init();
};

}  // namespace daisi::path_planning
#endif  // DAISI_PATH_PLANNING_DELIVERY_STATION_H_
