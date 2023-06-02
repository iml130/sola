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

#ifndef DAISI_PATH_PLANNING_PICKUP_STATION_H_
#define DAISI_PATH_PLANNING_PICKUP_STATION_H_

#include <queue>

#include "agv_info.h"
#include "agv_queue.h"
#include "agv_state.h"
#include "next_to_mode.h"
#include "ns3/vector.h"
#include "path_planning/consensus/consensus.h"
#include "path_planning/consensus/consensus_types.h"
#include "path_planning/consensus/paxos/constants.h"
#include "path_planning/consensus/paxos/message/accept_message.h"
#include "path_planning/consensus/paxos/message/ok_message.h"
#include "path_planning/consensus/paxos/message/prepare_message.h"
#include "path_planning/consensus/paxos/message/promise_message.h"
#include "path_planning/constants.h"
#include "path_planning/message/misc/new_authority_agv.h"
#include "path_planning/message/misc/reached_goal.h"
#include "path_planning/path_planning_logger_ns_3.h"
#include "sola-ns3/sola_ns3_wrapper.h"
#include "station.h"
#include "transport_order_info.h"

namespace daisi::path_planning {

//! Logical agent of station that processes output transport orders and controls AGVs.  As it is in
//! control of AGVs, it is also responsible for collision free movement of those
class PickupStation {
public:
  PickupStation(PickupStationInfo info, std::shared_ptr<PathPlanningLoggerNs3> logger,
                consensus::ConsensusSettings settings, NextTOMode next_to_mode, uint32_t device_id);
  void postInit();
  [[nodiscard]] std::string getConnectionString() const { return sola_->getConectionString(); }

  void setRoutes(StationRoutes routes);
  void setDeliveryStations(DeliveryStationPoints points);

  [[nodiscard]] size_t numberAGVAtStation() const;

  [[nodiscard]] PickupStationInfo getStationInfo() const { return info_; }

  void scheduleSpawnTO(uint32_t start_time, uint32_t stop_time,
                       std::function<TransportOrderInfo(void)> order_create);

private:
  [[nodiscard]] std::string getFullName() const;

  void spawnTO();

  /**
   * Calculates and initiates the next step for the given AGV
   * @param agv_id ID of agv
   */
  void update(const std::string &agv_id);

  /**
   * Add the new AGV to our ownership list
   **/
  void addToAGVOwnership(const message::NewAuthorityAGV &msg);

  /**
   *
   * @param delivery_station_id
   * @return next pickup station if side should be changed (other side is nearer), otherwise
   * std::nullopt
   */
  [[nodiscard]] std::optional<uint32_t> getRandomStationIfOtherSideNearer(
      uint32_t delivery_station_id);

  /**
   * Sets the finished negotiating state and initiates driving to the pickup/delivery station
   * Should be called after negotiating the route and once it is time to start driving.
   * @param agv_uuid ID of agv
   */
  void negotiationGo(const std::string &agv_uuid);

  void processMessage(const sola::Message &msg);

  void processReachedGoal(const message::ReachedGoal &msg);
  void processNewAuthorityAGV(const message::NewAuthorityAGV &msg);
  void topicMessage(const sola::TopicMessage &msg);

  /**
   * Get the next point that the AGV of the passed AGVInfo should drive to
   * @param agv
   * @return next point
   */
  [[nodiscard]] ns3::Vector2D getNextPoint(AGVInfo &agv);

  void initiateConsensus(const std::string &agv_id, RouteIdentifier d);
  void initiateConsensusToDelivery(AGVInfo &agv);
  void initiateConsensusToPickup(AGVInfo &agv);
  void init();

  const uint32_t device_id_ = 0;

  PickupStationInfo info_;

  uint32_t current_delay_ = 0;  //!< Random delay that is used if consensus is failing
  uint32_t retry_ = 1;          //!< Number of retries for a failing consensus

  std::function<TransportOrderInfo()>
      create_order_;  //!< Function that creates a new order (with random delivery station)

  AGVQueue queue_;
  uint32_t amrs_to_initialize_ = 0;

  uint32_t to_stop_time_ = 0;  //!< Timestamp in milliseconds after which no TO should be spawned

  std::shared_ptr<PathPlanningLoggerNs3> logger_;
  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;

  consensus::ConsensusSettings consensus_settings_;
  std::optional<consensus::Consensus> consensus_;  //!< Interface to consensus finding procedures

  std::unordered_map<std::string, AGVInfo> agv_ownership_;  //!< AGVs of this station by ID

  struct ConsensusData {
    std::string agv_uuid;
    PointTimePairs pairs;
  };

  std::optional<ConsensusData>
      current_consensus_;  //!< Data of the currently running consensus (only one consensus at a
                           //!< time per PickupStation allowed yet)
  NextTOMode next_to_mode_;
};

}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_PICKUP_STATION_H_
