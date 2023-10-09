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

#include "cpps_logger_ns3.h"

#include "ns3/simulator.h"

using TableDefinition = const DatabaseTable;
using ViewDefinition = const std::unordered_map<std::string, std::string>;

namespace daisi::cpps {

// Refer to DatabaseTable and DatabaseColumnInfo definitions in ../logging/definitions.h
//! Don't forget to use .c_str() to convert std::string into a char array

// * CppsExecutedOrderUtility
static TableDefinition kExecutedOrderUtility("CppsExecutedOrderUtility",
                                             {{"OrderUuid", "%s", true,
                                               "TransportOrder(OrderUuid)"},
                                              {"AMRUuid", "%s", true, "AMR(ApplicationUuid)"},
                                              {"Timestamp_ms", "%lu", true},
                                              {"ExpectedStartTime_ms", "%lf", true},
                                              {"ExecutionDuration_ms", "%lf", true},
                                              {"ExecutionDistance_m", "%lf", true},
                                              {"TravelToPickupDuration_ms", "%lf", true},
                                              {"TravelToPickupDistance_m", "%lf", true},
                                              {"Makespan", "%lf", true},
                                              {"Delay_ms", "%lf", true},
                                              {"DeltaExecutionDuration_ms", "%lf", true},
                                              {"DeltaExecutionDistance_m", "%lf", true},
                                              {"DeltaTravelToPickupDuration_ms", "%lf", true},
                                              {"DeltaTravelToPickupDistance_m", "%lf", true},
                                              {"DeltaMakespan", "%lf", true},
                                              {"QueueSize", "%u", true},
                                              {"Quality", "%lf", true},
                                              {"Costs", "%lf", true},
                                              {"Utility", "%lf", true}},
                                             "PRIMARY KEY(AMRUuid, OrderUuid, Timestamp_ms)");
static const std::string kCreateExecutedOrderUtility =
    getCreateTableStatement(kExecutedOrderUtility);

void CppsLoggerNs3::logExecutedOrderCost(const ExecutedOrderUtilityLoggingInfo &logging_info) {
  static bool executed_order_utility_exists = false;
  if (!executed_order_utility_exists) {
    log_(kCreateExecutedOrderUtility);
    executed_order_utility_exists = true;
  }

  auto t = std::make_tuple(
      /* OrderUuid */ logging_info.order.c_str(),
      /* AMRUuid */ logging_info.amr.c_str(),
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* ExpectedStartTime_ms */ logging_info.expected_start_time,
      /* ExecutionDuration_ms */ logging_info.execution_duration,
      /* ExecutionDistance_m */ logging_info.execution_distance,
      /* TravelToPickupDuration_ms */ logging_info.travel_to_pickup_duration,
      /* TravelToPickupDistance_m */ logging_info.travel_to_pickup_distance,
      /* Makespan */ logging_info.makespan,
      /* Delay_ms */ logging_info.delay,
      /* DeltaExecutionDuration_ms */ logging_info.delta_execution_duration,
      /* DeltaExecutionDistance_m */ logging_info.delta_execution_distance,
      /* DeltaTravelToPickupDuration_ms */ logging_info.delta_travel_to_pickup_duration,
      /* DeltaTravelToPickupDistance_m */ logging_info.delta_travel_to_pickup_distance,
      /* DeltaMakespan */ logging_info.delta_makespan,
      /* QueueSize */ logging_info.queue_size,
      /* Quality */ logging_info.quality,
      /* Costs */ logging_info.costs,
      /* Utility */ logging_info.utility);
  log_(getInsertStatement(kExecutedOrderUtility, t));
}

// * CppsMaterialFlow
static TableDefinition kMaterialFlow("CppsMaterialFlow", {DatabaseColumnInfo{"Id"},
                                                          {"Timestamp_ms", "%u", true},
                                                          {"Uuid", "%s", true},
                                                          {"IpLogicalCore", "%s", true},
                                                          {"PortLogicalCore", "%u", true},
                                                          {"State", "%u", true}});
static const std::string kCreateMaterialFlow = getCreateTableStatement(kMaterialFlow);

void CppsLoggerNs3::logMaterialFlow(const std::string &mf_uuid, const std::string &ip,
                                    uint16_t port, uint8_t state) {
  static bool material_flow_exists = false;
  if (!material_flow_exists) {
    log_(kCreateMaterialFlow);
    material_flow_exists = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Uuid */ mf_uuid.c_str(),
      /* IpLogicalCore */ ip.c_str(),
      /* PortLogicalCore */ port,
      /* State */ state);
  log_(getInsertStatement(kMaterialFlow, t));
}

// * CppsNegotiationTraffic
static TableDefinition kNegotiationTraffic("CppsNegotiationTraffic", {DatabaseColumnInfo{"Id"},
                                                                      {"TransportOrderId", "sql%u",
                                                                       true, "TransportOrder(Id)"},
                                                                      {"Timestamp_ms", "%u", true},
                                                                      {"SenderIp", "%s", true},
                                                                      {"SenderPort", "%u", true},
                                                                      {"TargetIp", "%s", true},
                                                                      {"TargetPort", "%u", true},
                                                                      {"MsgType", "%u", true},
                                                                      {"Content", "%s"}});
static const std::string kCreateNegotiationTraffic = getCreateTableStatement(kNegotiationTraffic);

static TableDefinition kEnumCppsMessageType("enumCppsMessageType",
                                            {{"Id", "%u", true, "", true}, {"Name", "%s", true}});
static const std::string kCreateEnumCppsMessageType = getCreateTableStatement(kEnumCppsMessageType);

void CppsLoggerNs3::logCppsMessageTypes() {
  // TODO: add logging of refactored message types
  // for (auto const &[key, name] : kMapCppsMessageTypeStrings) {
  //   auto t = std::make_tuple(
  //       /* Id */ key,
  //       /* Name */ name.c_str());
  //   log_(getInsertStatement(kEnumCppsMessageType, t));
  // }
}

static TableDefinition kCppsMessage("CppsTopicMessage", {
                                                            DatabaseColumnInfo{"Id"},
                                                            {"MessageUuid", "%s", true},
                                                            {"MessageContent", "%s", true},
                                                        });
static const std::string kCreateCppsMessage = getCreateTableStatement(kCppsMessage);

void CppsLoggerNs3::logCppsMessage(solanet::UUID msg_uuid, const std::string &msg_content) {
  static bool cpps_message_exists = false;
  if (!cpps_message_exists) {
    log_(kCreateCppsMessage);
    cpps_message_exists = true;
  }

  std::string msg_uuid_str = solanet::uuidToString(msg_uuid);

  auto t = std::make_tuple(
      /* MessageUuid */ msg_uuid_str.c_str(),
      /* MessageContent */ msg_content.c_str());
  log_(getInsertStatement(kCppsMessage, t));
}

static ViewDefinition kNegotiationTrafficReplacements = {
    {"MsgType", "enumCppsMessageType.Name AS MessageType"}};
static const std::string kCreateViewNegotiationTraffic = getCreateViewStatement(
    kNegotiationTraffic, kNegotiationTrafficReplacements,
    {"LEFT JOIN enumCppsMessageType ON CppsNegotiationTraffic.MsgType = enumCppsMessageType.Id"});

void CppsLoggerNs3::logNegotiationTraffic(const NegotiationTrafficLoggingInfo &logging_info) {
  static bool negotiation_traffic_exists = false;
  if (!negotiation_traffic_exists) {
    log_(kCreateNegotiationTraffic);
    negotiation_traffic_exists = true;

    log_(kCreateEnumCppsMessageType);
    logCppsMessageTypes();

    log_(kCreateViewNegotiationTraffic);
  }

  std::string transport_order_id =
      "(SELECT Id FROM TransportOrder WHERE OrderUuid='" + logging_info.order + "')";
  std::string sender_ip = logging_info.sender_ip;
  std::string target_ip = logging_info.target_ip;
  std::string content = logging_info.content;
  auto t = std::make_tuple(
      /* TransportOrderId */ transport_order_id.c_str(),
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* SenderIp */ sender_ip.c_str(),
      /* SenderPort */ logging_info.sender_port,
      /* TargetIp */ target_ip.c_str(),
      /* TargetPort */ logging_info.sender_port,
      /* MsgType */ logging_info.message_type,
      /* Content */ content.c_str());
  log_(getInsertStatement(kNegotiationTraffic, t));
}

// * CppsStation
static TableDefinition kStation("CppsStation", {DatabaseColumnInfo{"Id"},
                                                // {"ApplicationUuid", "%s", true},
                                                {"Name", "%s", true},
                                                {"Type", "%s", true},
                                                {"PosX_m", "%lf", true},
                                                {"PosY_M", "%lf", true}});
static const std::string kCreateStation = getCreateTableStatement(kStation);

void CppsLoggerNs3::logStation(const std::string &name, const std::string &type,
                               ns3::Vector2D position,
                               const std::vector<ns3::Vector2D> &additionalPositions) {
  static bool station_exists = false;
  if (!station_exists) {
    log_(kCreateStation);
    station_exists = true;
  }

  std::stringstream stream;
  for (auto pos : additionalPositions)
    stream << pos.x << pos.y << ";";

  auto t = std::make_tuple(
      // /* ApplicationUuid */ "",
      /* Name */ name.c_str(),
      /* Type */ type.c_str(),
      /* PosX_m */ position.x,
      /* PosY_m */ position.y);
  log_(getInsertStatement(kStation, t));
  // additionalPositions.empty() ? "" : stream.str().c_str()
}

// * Constructor & Other methods
CppsLoggerNs3::CppsLoggerNs3(LogDeviceApp log_device_application, LogFunction log)
    : log_device_application_(std::move(log_device_application)), log_(std::move(log)) {}

CppsLoggerNs3::~CppsLoggerNs3() {
  auto current_time = ns3::Simulator::Now().GetMilliSeconds();
  log_(toSQL("UPDATE DeviceApplication SET StopTime_ms=%lu WHERE ApplicationUuid='%s';",
             current_time, uuid_.c_str()));
}

}  // namespace daisi::cpps
