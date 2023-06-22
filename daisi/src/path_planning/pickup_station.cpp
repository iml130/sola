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

#include "pickup_station.h"

#include <random>
#include <utility>

#include "cpps/common/uuid_generator.h"
#include "minhton/utils/config_reader.h"
#include "ns3/simulator.h"
#include "path_planning/consensus/paxos/message/prepare_message.h"
#include "path_planning/consensus/paxos/paxos_consensus.h"
#include "path_planning/message/serializer.h"
#include "sola_check.h"
#include "task.h"
#include "time_calculation_helper.h"
#include "utils/daisi_check.h"
#include "utils/random_engine.h"
#include "utils/sola_utils.h"

namespace daisi::path_planning {

PickupStation::PickupStation(PickupStationInfo info, std::shared_ptr<PathPlanningLoggerNs3> logger,
                             consensus::ConsensusSettings settings, NextTOMode next_to_mode,
                             uint32_t device_id)
    : device_id_(device_id),
      info_(std::move(info)),
      logger_(std::move(logger)),
      consensus_settings_(std::move(settings)),
      next_to_mode_(next_to_mode) {
  logger_->setApplicationUUID(UUIDGenerator::get()());
  logger_->logStation(getFullName(), "pickup", info_.center_pos);
  init();
}

void PickupStation::init() {
  const std::string config_file = "configurations/join.yml";

  sola::ManagementOverlayMinhton::Config config_mo = minhton::config::readConfig(config_file);

  sola_ns3::SOLAWrapperNs3::setJoinIp(config_mo);

  // Nothing to configure (yet)
  sola::EventDisseminationMinhcast::Config config_ed;

  sola_ = std::make_shared<sola_ns3::SOLAWrapperNs3>(
      config_mo, config_ed, [&](const sola::Message &m) { processMessage(m); },
      [this](const sola::TopicMessage &m) { this->topicMessage(m); }, logger_, getFullName(),
      device_id_);
}

void PickupStation::postInit() {
  // Initialize consensus
  consensus_.emplace(consensus_settings_, sola_, info_.station_id, logger_);
}

std::string PickupStation::getFullName() const {
  return "/station " + std::to_string(info_.station_id);
}

void PickupStation::processMessage(const sola::Message &msg) {
  auto message = message::deserialize<message::MiscMessage>(msg.content);
  if (auto new_agv = std::get_if<message::NewAuthorityAGV>(&message)) {
    processNewAuthorityAGV(*new_agv);
  } else if (auto reached_goal = std::get_if<message::ReachedGoal>(&message)) {
    processReachedGoal(*reached_goal);
  } else {
    throw std::runtime_error("invalid message");
  }
}

void PickupStation::processReachedGoal(const message::ReachedGoal &msg) {
  logger_->logRecvPathPlanningTraffic(msg.agv_uuid, std::to_string(info_.station_id), 3);

  // Fix to avoid rounding errors
  agv_ownership_[msg.agv_uuid].last_position = agv_ownership_[msg.agv_uuid].next_position;
  update(msg.agv_uuid);
}

void PickupStation::addToAGVOwnership(const message::NewAuthorityAGV &msg) {
  SOLA_CHECK(agv_ownership_.count(msg.agv_uuid) == 0, "Already having authority over AGV");
  AGVInfo info;
  info.id = msg.agv_uuid;
  info.ip = msg.agv_ip;
  info.kinematics = {msg.max_velocity, msg.min_velocity, msg.max_acceleration,
                     msg.min_acceleration};
  //, msg.load_time_s,  msg.unload_time_s};
  info.last_position = {msg.current_x, msg.current_y};
  info.next_station = info_.station_id;  // Is inbound to ourselves
  agv_ownership_.insert({msg.agv_uuid, info});
}

void PickupStation::processNewAuthorityAGV(const message::NewAuthorityAGV &msg) {
  logger_->logRecvPathPlanningTraffic(msg.agv_uuid, std::to_string(info_.station_id), 2);
  addToAGVOwnership(msg);

  if (msg.initial) {
    // AMR is initializing and sitting in our station
    amrs_to_initialize_++;
    queue_.add(msg.agv_uuid);
    agv_ownership_[msg.agv_uuid].state = AGVState::kWaitingInQueue;
    agv_ownership_[msg.agv_uuid].next_position =
        agv_ownership_[msg.agv_uuid].last_position;  // Will stay at current pos
  } else {
    // Is currently on way to delivery
    agv_ownership_[msg.agv_uuid].state = AGVState::kDriveToDelivery;
    assert(msg.current_del_dest_x != std::numeric_limits<double>::quiet_NaN());
    assert(msg.current_del_dest_y != std::numeric_limits<double>::quiet_NaN());
    agv_ownership_[msg.agv_uuid].next_position = {msg.current_del_dest_x, msg.current_del_dest_y};

    // Find del station from coordinates
    uint16_t station_id = UINT16_MAX;
    for (auto [id, pos] : info_.delivery_station) {
      if (agv_ownership_[msg.agv_uuid].next_position == pos) {
        station_id = id;
        break;
      }
    }
    assert(station_id != UINT16_MAX);

    TransportOrderInfo info{station_id};
    info.uuid = "AFTERHANDOVER-" + msg.agv_uuid;
    agv_ownership_[msg.agv_uuid].current_to = info;
  }
}

void PickupStation::setRoutes(StationRoutes routes) { info_.routes = std::move(routes); }
void PickupStation::setDeliveryStations(DeliveryStationPoints points) {
  info_.delivery_station = std::move(points);
}

void PickupStation::topicMessage(const sola::TopicMessage &msg) {
  consensus_->recvTopicMessage(msg.topic, msg.content);
}

void PickupStation::update(const std::string &agv_id) {
  AGVInfo &agv = agv_ownership_[agv_id];
  assert(agv.last_position == agv.next_position);

  if (agv.state == AGVState::kWaitingInQueue) {
    return;
  }

  if (agv.state == AGVState::kDriveToDelivery) {
    agv.state = AGVState::kUnloading;
    ns3::Simulator::Schedule(ns3::MilliSeconds(1000), &PickupStation::update, this, agv_id);
    return;
  }

  if (agv.state == AGVState::kUnloading) {
    agv.state = AGVState::kUnloadingFinished;

    // Pack to/from station IDS into first element of vector
    ns3::Vector pickup(agv.current_to.delivery_station, -1, -1);
    ns3::Vector delivery(info_.station_id, -1, -1);
    Task task(agv.current_to.uuid, pickup, delivery);
    // logger_->logTransportOrder(task.getUuid(), agv.current_to.delivery_station,
    // info_.station_id);
    logger_->logPPTransportOrderUpdate(agv.current_to.uuid, 4);
  }

  if (agv.last_position == info_.center_pos && agv.state == AGVState::kDriveToQueue) {
    agv.state = AGVState::kWaitingInQueue;
    queue_.add(agv.id);
    spawnTO();
    return;
  }

  if (agv.state == AGVState::kDriveToPickupOut) {
    agv.state = AGVState::kNegotiatingDriveToDelivery;
    initiateConsensusToDelivery(agv);
    return;
  }

  if (agv.state == AGVState::kUnloadingFinished) {
    agv.state = AGVState::kNegotiatingDriveToPickup;
    // agv.current_to.uuid = "empty-" + agv.current_to.uuid;

    logger_->logTOSpawn(agv.current_to.uuid, agv.current_to.delivery_station, info_.station_id);
    logger_->logPPTransportOrderUpdate(agv.current_to.uuid, 0, agv.id);
    initiateConsensusToPickup(agv);
    return;
  }

  agv.next_position = getNextPoint(agv);
  if (agv.next_station != info_.station_id && agv.state == AGVState::kDriveToDelivery) {
    // Handover
    auto next_station = std::find_if(
        info_.pickup_station_registry.begin(), info_.pickup_station_registry.end(),
        [agv](const PickupStationHandoverInfo &info) { return info.id == agv.next_station; });
    assert(next_station != info_.pickup_station_registry.end());
    message::HandoverMessage msg(message::DriveMessage(agv.next_position), next_station->id,
                                 next_station->ip);
    logger_->logSendPathPlanningTraffic(std::to_string(info_.station_id), agv.id, 1);
    sola_->sendData(message::serialize<message::StationAGVMessage>(msg), sola::Endpoint(agv.ip));
    agv_ownership_.erase(agv.id);
  } else {
    message::DriveMessage msg{agv.next_position};
    logger_->logSendPathPlanningTraffic(std::to_string(info_.station_id), agv.id, 1);
    assert(msg.isInitialized());
    sola_->sendData(message::serialize<message::StationAGVMessage>(msg), sola::Endpoint(agv.ip));
  }
}

ns3::Vector2D PickupStation::getNextPoint(AGVInfo &agv) {
  switch (agv.state) {
    case AGVState::kLeavingQueue:
      agv.state = AGVState::kDriveToPickupOut;
      return info_.out_pickup;
    case AGVState::kNegotiatedDriveToDelivery: {
      RouteIdentifier d;
      assert(agv.current_to.initialized());
      d.from_station_id = info_.station_id;
      d.to_station_id = agv.current_to.delivery_station;
      d.out_point = MainConnPoints::kOutPickup;
      d.in_point = MainConnPoints::kInDelivery;

      agv.state = AGVState::kDriveToDelivery;

      return info_.routes.at(d).end;
    }
    case AGVState::kNegotiatedDriveToPickup:
      agv.state = AGVState::kDriveToPickupIn;
      return info_.in_pickup;
    case AGVState::kDriveToPickupIn:
      logger_->logPPTransportOrderUpdate(agv.current_to.uuid, 4);
      agv.current_to = {};
      agv.state = AGVState::kDriveToQueue;
      return info_.center_pos;
    default:
      throw std::runtime_error("unhandled");
  }
}

void PickupStation::initiateConsensusToDelivery(AGVInfo &agv) {
  assert(agv.current_to.initialized());

  RouteIdentifier d;
  d.from_station_id = info_.station_id;
  d.to_station_id = agv.current_to.delivery_station;
  d.out_point = MainConnPoints::kOutPickup;
  d.in_point = MainConnPoints::kInDelivery;

  if (info_.routes.at(d).intersections.empty()) {
    // Should not happen with current station setup
    assert(false);
    // Nothing needs to be negotiated
    agv.state = AGVState::kNegotiatedDriveToDelivery;
    update(agv.id);
    return;
  }

  initiateConsensus(agv.id, d);
}

void PickupStation::initiateConsensusToPickup(AGVInfo &agv) {
  assert(agv.current_to.initialized());
  RouteIdentifier d;
  d.from_station_id = agv.current_to.delivery_station;
  d.to_station_id = info_.station_id;
  d.out_point = MainConnPoints::kOutDelivery;
  d.in_point = MainConnPoints::kInPickup;

  DAISI_CHECK(!info_.routes.at(d).intersections.empty(),
              "Found route with no intersections but all routes have some intersections");

  initiateConsensus(agv.id, d);
}

void PickupStation::initiateConsensus(const std::string &agv_id, RouteIdentifier d) {
  logger_->logPPTransportOrderUpdate(agv_ownership_[agv_id].current_to.uuid, 1);

  if (current_consensus_.has_value()) {
    // Already one consensus in progress, try again in 300 ms
    constexpr uint32_t kConsensusInProgressRetryMs = 300;
    logger_->logPPTransportOrderUpdate(agv_ownership_[agv_id].current_to.uuid, 5);
    ns3::Simulator::Schedule(ns3::MilliSeconds(kConsensusInProgressRetryMs),
                             &PickupStation::initiateConsensus, this, agv_id, d);
    return;
  }
  assert(!current_consensus_.has_value());

  const cpps::AmrKinematics &kinematics = agv_ownership_.at(agv_id).kinematics;

  ns3::Vector2D start = info_.routes[d].start;
  ns3::Vector2D end = info_.routes[d].end;

  std::vector<ns3::Vector2D> intersections;
  std::for_each(
      info_.routes[d].intersections.begin(), info_.routes[d].intersections.end(),
      [&intersections](ns3::Vector2D intersection) { intersections.emplace_back(intersection); });

  auto intersection_times = calculateTimeTillPoints(kinematics, start, end, intersections);

  current_consensus_.emplace(ConsensusData{agv_id, intersection_times});

  consensus_->findConsensus(
      intersection_times, 42,  // kinematics.getLoadTime(),
      [this](uint32_t instance, double start_time) {
        assert(current_consensus_.has_value());
        retry_ = 1;
        logger_->logConsensusFinished(instance, info_.station_id,
                                      current_consensus_.value().agv_uuid);
        const AGVInfo &agv = agv_ownership_[current_consensus_.value().agv_uuid];
        logger_->logPPTransportOrderUpdate(agv.current_to.uuid, 2);
        uint64_t current_time = ns3::Simulator::Now().GetMilliSeconds();
        assert(start_time * 1000 - current_time > 0);
        ns3::Simulator::Schedule(ns3::MilliSeconds(start_time * 1000 - current_time),
                                 &PickupStation::negotiationGo, this,
                                 current_consensus_.value().agv_uuid);
        uint32_t to = agv.state == AGVState::kNegotiatingDriveToDelivery
                          ? agv.current_to.delivery_station
                          : info_.station_id;
        for (auto point : current_consensus_.value().pairs) {
          logger_->logIntersectOccupancy(
              info_.station_id, to, agv.id,
              agv.state == AGVState::kNegotiatingDriveToDelivery ? "LOADED" : "EMPTY",
              point.first.x, point.first.y, start_time + point.second);
        }
        current_consensus_.reset();
      },
      [&](uint32_t /*unused*/) {
        assert(current_consensus_.has_value());
        AGVInfo agv = agv_ownership_[current_consensus_.value().agv_uuid];
        logger_->logPPTransportOrderUpdate(agv.current_to.uuid, 6);

        current_consensus_.reset();
        std::uniform_int_distribution<uint32_t> dist(300, 1000);
        if (retry_ == 1) {
          current_delay_ = dist(daisi::global_random_engine);
        }
        if (agv.state == AGVState::kNegotiatingDriveToDelivery) {
          ns3::Simulator::Schedule(ns3::MilliSeconds(current_delay_ * retry_),
                                   &PickupStation::initiateConsensusToDelivery, this, agv);
          retry_++;
        } else if (agv.state == AGVState::kNegotiatingDriveToPickup) {
          ns3::Simulator::Schedule(ns3::MilliSeconds(current_delay_ * retry_),
                                   &PickupStation::initiateConsensusToPickup, this, agv);
          retry_++;
        }
      });
}

void PickupStation::negotiationGo(const std::string &agv_uuid) {
  AGVInfo &agv = agv_ownership_[agv_uuid];
  if (agv.state == AGVState::kNegotiatingDriveToDelivery) {
    agv.state = AGVState::kNegotiatedDriveToDelivery;
  } else if (agv.state == AGVState::kNegotiatingDriveToPickup) {
    agv.state = AGVState::kNegotiatedDriveToPickup;
  } else {
    throw std::runtime_error("invalid state");
  }

  logger_->logPPTransportOrderUpdate(agv.current_to.uuid, 3);

  update(agv.id);
}

void PickupStation::scheduleSpawnTO(uint32_t start_time, uint32_t stop_time,
                                    std::function<TransportOrderInfo(void)> order_create) {
  assert(start_time == ns3::Simulator::Now().GetMilliSeconds());
  to_stop_time_ = stop_time;
  create_order_ = std::move(order_create);

  // Here we do not know our initial AMRs yet.
  // Hence we need to schedule the initial start at runtime.
  ns3::Simulator::Schedule(ns3::MilliSeconds(0), &PickupStation::spawnTO, this);
}

std::optional<uint32_t> PickupStation::getRandomStationIfOtherSideNearer(
    uint32_t delivery_station_id) {
  double left = info_.pickup_station_registry[0].pos.x;
  double right = info_.pickup_station_registry[info_.pickup_station_registry.size() / 2].pos.x;
  double own_pos = info_.center_pos.x;
  assert(left < right);
  double middle = left + (right - left) / 2.0;
  if (own_pos == left && info_.delivery_station[delivery_station_id].x >= middle) {
    // "Left to right"
    std::uniform_int_distribution<uint32_t> dist(info_.pickup_station_registry.size() / 2,
                                                 info_.pickup_station_registry.size() - 1);
    return dist(daisi::global_random_engine);
  }

  if (own_pos == right && info_.delivery_station[delivery_station_id].x < middle) {
    // "Right to left"
    std::uniform_int_distribution<uint32_t> dist(0, info_.pickup_station_registry.size() / 2 - 1);
    return dist(daisi::global_random_engine);
  }

  return std::nullopt;
}

void PickupStation::spawnTO() {
  // SpawnTO is called on every start even if we do not have an AMR ourselves.
  if (!queue_.hasAGV()) return;

  if (ns3::Simulator::Now().GetMilliSeconds() >= to_stop_time_) {
    DAISI_CHECK(amrs_to_initialize_ == 0, "Not enough time to start all AMRs at least once");
    // Never create new orders after stop time
    return;
  }

  TransportOrderInfo info = create_order_();
  // Pack to/from station IDS into first element of vector
  ns3::Vector pickup(info_.station_id, -1, -1);
  ns3::Vector delivery(info.delivery_station, -1, -1);

  // When not changing site, always return to origin pickup station.
  uint32_t next_station = info_.station_id;

  if (next_to_mode_ == NextTOMode::kRandomNearestSide) {
    std::optional<uint32_t> next = getRandomStationIfOtherSideNearer(info.delivery_station);
    if (next.has_value()) next_station = next.value();
  }

  Task task(info.uuid, pickup, delivery);
  // logger_->logTransportOrder(task, info_.station_id, info.delivery_station);

  logger_->logTOSpawn(info.uuid, info_.station_id, info.delivery_station);

  assert(queue_.hasAGV());

  std::string agv_id = queue_.next();
  logger_->logPPTransportOrderUpdate(info.uuid, 0, agv_id);

  AGVInfo &agv = agv_ownership_.at(agv_id);
  assert(!agv.empty());
  agv.state = AGVState::kLeavingQueue;
  agv.current_to = info;
  agv.next_position = agv.last_position;
  agv.next_station = next_station;
  update(agv_id);

  if (amrs_to_initialize_ != 0) {
    amrs_to_initialize_--;
    if (amrs_to_initialize_ != 0)
      // Still more AMRs at this station which have not started yet.
      ns3::Simulator::Schedule(ns3::MilliSeconds(1000), &PickupStation::spawnTO, this);
  }
}
size_t PickupStation::numberAGVAtStation() const { return queue_.numberAGV(); }
}  // namespace daisi::path_planning
