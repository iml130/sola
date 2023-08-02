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

#include "path_planning_logger_ns_3.h"

#include "ns3/simulator.h"

#define TableDefinition static const DatabaseTable
#define ViewDefinition static const std::unordered_map<std::string, std::string>

namespace daisi::path_planning {

// Refer to DatabaseTable and DatabaseColumnInfo definitions in ../logging/definitions.h
//! Don't forget to use .c_str() to convert std::string into a char array

// * IntersectOccupancy
TableDefinition kIntersectOccupancy("IntersectOccupancy", {DatabaseColumnInfo{"Id"},
                                                           {"FromStation", "%i", true},
                                                           {"ToStation", "%i", true},
                                                           {"AmrId", "%s", true},
                                                           {"TOState", "%s"},
                                                           {"IntersectX", "%f", true},
                                                           {"IntersectY", "%f", true},
                                                           {"Time_s", "%lf", true}});
static const std::string kCreateIntersectOccupancy = getCreateTableStatement(kIntersectOccupancy);
static bool intersect_occupancy_exists_ = false;

void PathPlanningLoggerNs3::logIntersectOccupancy(uint32_t from, uint32_t to,
                                                  const std::string &agv_id,
                                                  const std::string &to_state, float intersect_x,
                                                  float intersect_y, double time) {
  if (!intersect_occupancy_exists_) {
    log_(kCreateIntersectOccupancy);
    intersect_occupancy_exists_ = true;
  }

  auto t = std::make_tuple(
      /* FromStation */ from,
      /* ToStation */ to,
      /* AmrId */ agv_id.c_str(),
      /* TOState */ to_state.c_str(),
      /* IntersectX */ intersect_x,
      /* IntersectY */ intersect_y,
      /* Time_s */ time);
  log_(getInsertStatement(kIntersectOccupancy, t));
}

// * PathPlanningFinishedConsensus
TableDefinition kFinishedConsensus("PathPlanningFinishedConsensus", {DatabaseColumnInfo{"Id"},
                                                                     {"Instance", "%i", true},
                                                                     {"Station", "%i", true},
                                                                     {"AmrId", "%s"}});
static const std::string kCreateFinishedConsensus = getCreateTableStatement(kFinishedConsensus);
static bool finished_consensus_exists_ = false;

void PathPlanningLoggerNs3::logConsensusFinished(uint32_t instance, uint32_t station,
                                                 const std::string &uuid) {
  if (!finished_consensus_exists_) {
    log_(kCreateFinishedConsensus);
    finished_consensus_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Instance */ instance,
      /* Station */ station,
      /* AmrId */ uuid.c_str());
  log_(getInsertStatement(kFinishedConsensus, t));
}

// * PathPlanningReplication
TableDefinition kReplication("PathPlanningReplication", {DatabaseColumnInfo{"Id"},
                                                         {"ReplicationNode", "%s", true},
                                                         {"Instance", "%u", true},
                                                         {"ProposalId", "%u", true},
                                                         {"StationId", "%u", true}});
static const std::string kCreateReplication = getCreateTableStatement(kReplication);
static bool replication_exists_ = false;

void PathPlanningLoggerNs3::logReplication(const std::string &id, uint32_t instance_id,
                                           uint32_t proposal_id, uint32_t station_id) {
  if (!replication_exists_) {
    log_(kCreateReplication);
    replication_exists_ = true;
  }

  auto t = std::make_tuple(
      /* ReplicationNode */ id.c_str(),
      /* Instance */ instance_id,
      /* ProposalId */ proposal_id,
      /* StationId */ station_id);
  log_(getInsertStatement(kReplication, t));
}

// * PathPlanningTopicTraffic
TableDefinition kTopicTraffic("PathPlanningTopicTraffic", {DatabaseColumnInfo{"Id"},
                                                           {"Timestamp_ms", "%lu", true},
                                                           {"Topic", "%s", true},
                                                           {"Node", "%s", true},
                                                           {"Instance", "%i", true},
                                                           {"Mode", "%i", true},
                                                           {"MsgType", "%i", true}});
static const std::string kCreateTopicTraffic = getCreateTableStatement(kTopicTraffic);
static bool topic_traffic_exists_ = false;

void PathPlanningLoggerNs3::logSendPathPlanningTopicTraffic(const std::string &topic,
                                                            const std::string &node,
                                                            uint32_t instance, uint32_t msg_type) {
  if (!topic_traffic_exists_) {
    log_(kCreateTopicTraffic);
    topic_traffic_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Topic */ topic.c_str(),
      /* Node */ node.c_str(),
      /* Instance */ instance,
      /* Mode */ 1,
      /* MsgType */ msg_type);
  log_(getInsertStatement(kTopicTraffic, t));
}

void PathPlanningLoggerNs3::logRecvPathPlanningTopicTraffic(const std::string &topic,
                                                            const std::string &node,
                                                            uint32_t instance, uint32_t msg_type) {
  if (!topic_traffic_exists_) {
    log_(kCreateTopicTraffic);
    topic_traffic_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Topic */ topic.c_str(),
      /* Node */ node.c_str(),
      /* Instance */ instance,
      /* Mode */ 0,
      /* MsgType */ msg_type);
  log_(getInsertStatement(kTopicTraffic, t));
}

// * PathPlanningTraffic
TableDefinition kPpTraffic("PathPlanningTraffic", {DatabaseColumnInfo{"Id"},
                                                   {"Timestamp_ms", "%lu", true},
                                                   {"Sender", "%s", true},
                                                   {"Target", "%s", true},
                                                   {"Mode", "%i", true},
                                                   {"MsgType", "%i", true}
                                                   /*{"Content", "%s"}*/});
static const std::string kCreatePpTraffic = getCreateTableStatement(kPpTraffic);
static bool pp_traffic_exists_ = false;

void PathPlanningLoggerNs3::logSendPathPlanningTraffic(const std::string &sender,
                                                       const std::string &target,
                                                       uint32_t msg_type) {
  if (!pp_traffic_exists_) {
    log_(kCreatePpTraffic);
    pp_traffic_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Sender */ sender.c_str(),
      /* Target */ target.c_str(),
      /* Mode */ 1,
      /* MsgType */ msg_type
      /* Content */);
  log_(getInsertStatement(kPpTraffic, t));
}

void PathPlanningLoggerNs3::logRecvPathPlanningTraffic(const std::string &sender,
                                                       const std::string &target,
                                                       uint32_t msg_type) {
  if (!pp_traffic_exists_) {
    log_(kCreatePpTraffic);
    pp_traffic_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Sender */ sender.c_str(),
      /* Target */ target.c_str(),
      /* Mode */ 0,
      /* MsgType */ msg_type
      /* Content */);
  log_(getInsertStatement(kPpTraffic, t));
}

// * TransportOrderSpawn
TableDefinition kToSpawn("TransportOrderSpawn", {DatabaseColumnInfo{"Id"},
                                                 {"ToId", "%s", true},
                                                 {"Timestamp_ms", "%i", true},
                                                 {"FromStation", "%i", true, "Station(Id)"},
                                                 {"ToStation", "%i", true, "Station(Id)"}});
static const std::string kCreateToSpawn = getCreateTableStatement(kToSpawn);
static bool to_spawn_exists_ = false;

void PathPlanningLoggerNs3::logTOSpawn(const std::string &to_uuid, uint32_t station_id,
                                       uint32_t time_to_station) {
  if (!to_spawn_exists_) {
    log_(kCreateToSpawn);
    to_spawn_exists_ = true;
  }

  auto t = std::make_tuple(
      /* ToId */ to_uuid.c_str(),
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* FromStation */ station_id,
      /* ToStation */ time_to_station);
  log_(getInsertStatement(kToSpawn, t));
}

// * Constructor & Other methods
PathPlanningLoggerNs3::PathPlanningLoggerNs3(LogDeviceApp log_device_application, LogFunction log)
    : daisi::cpps::CppsLoggerNs3(log_device_application, log) {}

// * Logging to CPPS tables
void PathPlanningLoggerNs3::logPPTransportOrderUpdate(const std::string &order_uuid,
                                                      uint32_t status, const std::string &agv) {
  logTransportOrderUpdate(order_uuid, status, agv);
}

// * TransportOrderHistory
TableDefinition kTransportOrderHistory("TransportOrderHistory",
                                         {DatabaseColumnInfo{"Id"},
                                          {"TransportOrderId", "sql%u", true, "TransportOrder(Id)"},
                                          {"Timestamp_ms", "%u", true},
                                          {"State", "%u", true},
                                          {"PosX_m", "%f", true},
                                          {"PosY_m", "%f", true},
                                          {"AmrId", "sql%u", false, "CppsAutonomousMobileRobot(Id)"}
                                          /*{"TransportServiceId", "%u", false,
                                           "TransportService(Id)"}*/});
static const std::string kCreateTransportOrderHistory =
    getCreateTableStatement(kTransportOrderHistory);
static bool transport_order_history_exists_ = false;

void PathPlanningLoggerNs3::logTransportOrderUpdate(const Task &order,
                                                    const std::string &assigned_agv) {
  if (!transport_order_history_exists_) {
    log_(kCreateTransportOrderHistory);
    transport_order_history_exists_ = true;
  }

  ns3::Vector pos = order.getCurrentPosition();
  auto order_state = order.getOrderState();

  std::string transport_order_id =
      "(SELECT Id FROM TransportOrder WHERE OrderUuid='" + order.getUuid() + "')";

  auto table = kTransportOrderHistory;
  if (!assigned_agv.empty()) {
    std::string amr_id =
        "(SELECT Id FROM CppsAutonomousMobileRobot WHERE ApplicationUuid='" + assigned_agv + "')";
    auto t = std::make_tuple(
        /* TransportOrderId */ transport_order_id.c_str(),
        /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
        /* State */ order_state,
        /* PosX_m */ pos.x,
        /* PosY_m */ pos.y,
        /* AmrId */ amr_id.c_str()
        // /* TransportServiceId */ 1  // TODO
    );
    log_(getInsertStatement(table, t));
  } else {
    table.columns[6] = {"AmrId", "NULL"};
    auto t = std::make_tuple(
        /* TransportOrderId */ transport_order_id.c_str(),
        /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
        /* State */ order_state,
        /* PosX_m */ pos.x,
        /* PosY_m */ pos.y
        // /* TransportServiceId */ 1  // TODO
    );
    log_(getInsertStatement(table, t));
  }
}

void PathPlanningLoggerNs3::logTransportOrderUpdate(const std::string &order_uuid, uint32_t status,
                                                    const std::string &assigned_agv) {
  if (!transport_order_history_exists_) {
    log_(kCreateTransportOrderHistory);
    transport_order_history_exists_ = true;
  }

  std::string transport_order_id =
      "(SELECT Id FROM TransportOrder WHERE OrderUuid='" + order_uuid + "')";

  auto table = kTransportOrderHistory;
  if (!assigned_agv.empty()) {
    std::string amr_id =
        "(SELECT Id FROM CppsAutonomousMobileRobot WHERE ApplicationUuid='" + assigned_agv + "')";
    auto t = std::make_tuple(
        /* TransportOrderId */ transport_order_id.c_str(),
        /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
        /* State */ status,
        /* PosX_m */ 0.0,
        /* PosY_m */ 0.0,
        /* AmrId */ amr_id.c_str()
        // /* TransportServiceId */ 1  // TODO
    );
    log_(getInsertStatement(table, t));
  } else {
    table.columns[6] = {"AmrId", "NULL"};
    auto t = std::make_tuple(
        /* TransportOrderId */ transport_order_id.c_str(),
        /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
        /* State */ status,
        /* PosX_m */ 0.0,
        /* PosY_m */ 0.0
        // /* TransportServiceId */ 1  // TODO
    );
    log_(getInsertStatement(table, t));
  }
}

}  // namespace daisi::path_planning

#undef TableDefinition
#undef ViewDefinition
