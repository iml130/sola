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

#define TableDefinition static const DatabaseTable
#define ViewDefinition static const std::unordered_map<std::string, std::string>

namespace daisi::cpps {

// Refer to DatabaseTable and DatabaseColumnInfo definitions in ../logging/definitions.h
//! Don't forget to use .c_str() to convert std::string into a char array

// * CppsTopicMessage
TableDefinition kTopicMessage("CppsTopicMessage", {DatabaseColumnInfo{"Id"},
                                                   {"Timestamp_ms", "%lu", true},
                                                   {"Topic", "%s", true},
                                                   {"MessageUuid", "%s", true},
                                                   {"Node", "%s", true},
                                                   {"Content", "%s"},
                                                   {"Receive", "%u", true}});
static const std::string kCreateTopicMessage = getCreateTableStatement(kTopicMessage);
static bool topic_message_exists_ = false;

void CppsLoggerNs3::logTopicMessage(const std::string &topic, const std::string &message_id,
                                    const std::string &node, const std::string &message,
                                    bool receive) {
  if (!topic_message_exists_) {
    log_(kCreateTopicMessage);
    topic_message_exists_ = true;
  }

  // TODO Very inefficient way to escape characters in sqlite statement. Change to sqlite prepare
  std::string msg = message;
  const char separator = '\'';
  for (int i = 0; i < msg.size(); i++) {
    if (msg[i] == separator && (i + 1 >= msg.size() || msg[i + 1] != separator)) {
      msg.insert(i, "\'");
      i++;
    }
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Topic */ topic.c_str(),
      /* MessageUuid */ message_id.c_str(),
      /* Node */ node.c_str(),
      /* Content */ msg.c_str(),
      /* Receive */ receive ? 1 : 0);
  log_(getInsertStatement(kTopicMessage, t));
}

// * ExecutedOrderUtility
TableDefinition kExecutedOrderUtility("ExecutedOrderUtility",
                                      {{"OrderUuid", "%s", true, "TransportOrder(OrderUuid)"},
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
static bool executed_order_utility_exists_ = false;

void CppsLoggerNs3::logExecutedOrderCost(const ExecutedOrderUtilityLoggingInfo &logging_info) {
  if (!executed_order_utility_exists_) {
    log_(kCreateExecutedOrderUtility);
    executed_order_utility_exists_ = true;
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

// * MaterialFlow
TableDefinition kMaterialFlow("MaterialFlow", {DatabaseColumnInfo{"Id"},
                                               {"Timestamp_ms", "%u", true},
                                               {"Uuid", "%s", true},
                                               {"IpLogicalCore", "%s", true},
                                               {"PortLogicalCore", "%u", true},
                                               {"State", "%u", true}});
static const std::string kCreateMaterialFlow = getCreateTableStatement(kMaterialFlow);
static bool material_flow_exists_ = false;

void CppsLoggerNs3::logMaterialFlow(const std::string &mf_uuid, const std::string &ip,
                                    uint16_t port, uint8_t state) {
  if (!material_flow_exists_) {
    log_(kCreateMaterialFlow);
    material_flow_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Uuid */ mf_uuid.c_str(),
      /* IpLogicalCore */ ip.c_str(),
      /* PortLogicalCore */ port,
      /* State */ state);
  log_(getInsertStatement(kMaterialFlow, t));
}

// * NegotiationTraffic
TableDefinition kNegotiationTraffic("NegotiationTraffic",
                                    {DatabaseColumnInfo{"Id"},
                                     {"TransportOrderId", "sql%u", true, "TransportOrder(Id)"},
                                     {"Timestamp_ms", "%u", true},
                                     {"SenderIp", "%s", true},
                                     {"SenderPort", "%u", true},
                                     {"TargetIp", "%s", true},
                                     {"TargetPort", "%u", true},
                                     {"MsgType", "%u", true},
                                     {"Content", "%s"}});
static const std::string kCreateNegotiationTraffic = getCreateTableStatement(kNegotiationTraffic);
static bool negotiation_traffic_exists_ = false;

TableDefinition kEnumCppsMessageType("enumCppsMessageType",
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

ViewDefinition kNegotiationTrafficReplacements = {
    {"MsgType", "enumCppsMessageType.Name AS MessageType"}};
static const std::string kCreateViewNegotiationTraffic = getCreateViewStatement(
    kNegotiationTraffic, kNegotiationTrafficReplacements,
    {"LEFT JOIN enumCppsMessageType ON NegotiationTraffic.MsgType = enumCppsMessageType.Id"});

void CppsLoggerNs3::logNegotiationTraffic(const NegotiationTrafficLoggingInfo &logging_info) {
  if (!negotiation_traffic_exists_) {
    log_(kCreateNegotiationTraffic);
    negotiation_traffic_exists_ = true;

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

// * Station
TableDefinition kStation("Station", {DatabaseColumnInfo{"Id"},
                                     // {"ApplicationUuid", "%s", true},
                                     {"Name", "%s", true},
                                     {"Type", "%s", true},
                                     {"PosX_m", "%lf", true},
                                     {"PosY_M", "%lf", true}});
static const std::string kCreateStation = getCreateTableStatement(kStation);
static bool station_exists_ = false;

void CppsLoggerNs3::logStation(const std::string &name, const std::string &type,
                               ns3::Vector2D position,
                               const std::vector<ns3::Vector2D> &additionalPositions) {
  if (!station_exists_) {
    log_(kCreateStation);
    station_exists_ = true;
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

// * TopicEvent
TableDefinition kTopicEvent("TopicEvent",
                            {DatabaseColumnInfo{"Id"},
                             {"Timestamp_ms", "%lu", true},
                             {"Topic", "%s", true},
                             {"SourceUuid", "%s",
                              true},  // TODO: what is this?! materialFlow / transportOrder
                             {"Subscribe", "%u", true}});
static const std::string kCreateTopicEvent = getCreateTableStatement(kTopicEvent);
static bool topic_event_exists_ = false;

void CppsLoggerNs3::logTopicEvent(const std::string &topic, const std::string &node,
                                  bool subscribe) {
  if (!topic_event_exists_) {
    log_(kCreateTopicEvent);
    topic_event_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Topic */ topic.c_str(),
      /* SourceUuid */ node.c_str(),
      /* Subscribe */ subscribe ? 1 : 0);
  log_(getInsertStatement(kTopicEvent, t));
}

// * Constructor & Other methods
CppsLoggerNs3::CppsLoggerNs3(LogDeviceApp log_device_application, LogFunction log)
    : log_device_application_(log_device_application),
      log_(std::move(log)),
      uuid_("NOT-KNOWN-YET") {}

CppsLoggerNs3::~CppsLoggerNs3() {
  auto current_time = ns3::Simulator::Now().GetMilliSeconds();
  log_(toSQL("UPDATE DeviceApplication SET StopTime_ms=%lu WHERE ApplicationUuid='%s';",
             current_time, uuid_.c_str()));
}

}  // namespace daisi::cpps

#undef TableDefinition
#undef ViewDefinition
