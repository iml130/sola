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

#ifndef DAISI_PATH_PLANNING_AGV_LOGICAL_H_
#define DAISI_PATH_PLANNING_AGV_LOGICAL_H_

#include <memory>

#include "cpps/agv/topology.h"
#include "cpps/amr/amr_kinematics.h"
#include "logging/logger_manager.h"
#include "ns3/socket.h"
#include "path_planning/consensus/consensus_types.h"
#include "path_planning/consensus/constants.h"
#include "path_planning/consensus/paxos/paxos_replication_manager.h"
#include "path_planning/constants.h"
#include "path_planning/message/drive_message.h"
#include "path_planning/message/misc/new_authority_agv.h"
#include "path_planning/message/types_all.h"
#include "path_planning/path_planning_logger_ns_3.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::path_planning {

//! Logical agent of an AGV, that connected together with an physical AGV, follows drive commands
//! and stores replications of Paxos instances.
class AGVLogical {
public:
  AGVLogical(cpps::TopologyNs3 topology, consensus::ConsensusSettings consensus_settings,
             bool first_node, const ns3::Ptr<ns3::Socket> &socket, uint32_t device_id);

  std::string getConnectionString() const;

  /**
   * Register this AGV with an authority (\c PickupStation) by sending an \c
   * message::NewAuthorityAGV message to the pickup station. If the AGV was already registered by an
   * other authority, the other authority will not be notified about the change.
   * @param ip IP of the pickup station
   */
  void registerByAuthority(const std::string &ip);

private:
  bool connectionRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);
  void newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);

  void logAGV() const;

  void processMessageField(const message::FieldMessage &msg);
  void processMessageDescription(const std::string &payload);
  void processMessageUpdate(const message::PositionUpdate &msg);

  void readFromSocket(ns3::Ptr<ns3::Socket> socket);

  const uint32_t device_id_ = 0;
  ns3::Ptr<ns3::Socket> socket_;      //!< Socket listening for connection requests of an AGV
  ns3::Ptr<ns3::Socket> socket_agv_;  //!< Socket for TCP connection to AGV
  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;
  std::shared_ptr<PathPlanningLoggerNs3> logger_;
  bool first_node_;  //!< TODO: Workaround to specify the node that is the first node in the overlay
  std::string uuid_;
  cpps::TopologyNs3 topology_;
  uint32_t current_authority_station_id_ = UINT32_MAX;
  std::string current_authority_ip_ = "NONE";
  uint16_t current_authority_port_ = 0;
  double last_x_ = 0.0;
  double last_y_ = 0.0;

  cpps::AmrKinematics kinematics_;

  void processTopicMessage(const sola::TopicMessage &msg);
  consensus::ConsensusSettings consensus_settings_;
  std::optional<consensus::PaxosReplicationManager> replication_manager_;

  //! Initialize replication by subscribing to relevant topic
  void initReplication();

  void processMessage(const sola::Message &msg);
  void processDriveMessage(const message::DriveMessage &msg);
  void processHandoverMessage(const message::HandoverMessage &msg);
  void processReachedGoal();
  void postInit();

  //! Initialize by start listening for inbound physical AGV connections and starting SOLA
  void init();
};
}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_AGV_LOGICAL_H_
