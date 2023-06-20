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

#include "cpps/common/cpps_logger_ns3.h"

#include "cpps/message/types_all.h"

#define TableDefinition static const DatabaseTable
#define ViewDefinition static const std::unordered_map<std::string, std::string>

namespace daisi::cpps {

// Refer to DatabaseTable and DatabaseColumnInfo definitions in ../logging/definitions.h
//! Don't forget to use .c_str() to convert std::string into a char array

// * AMRHistory
TableDefinition kAmrHistory("AMRHistory", {DatabaseColumnInfo{"Id"},
                                           {"Timestamp_ms", "%u", true},
                                           {"AmrId", "sql%u", true, "AutonomousMobileRobot(Id)"},
                                           {"PosX_m", "%f", true},
                                           {"PosY_m", "%f", true},
                                           {"State", "%u", true}});
static const std::string kCreateAmrHistory = getCreateTableStatement(kAmrHistory);
static bool amr_history_exists_ = false;

void CppsLoggerNs3::logPositionUpdate(const AGVPositionLoggingInfo &logging_info) {
  if (!amr_history_exists_) {
    log_(kCreateAmrHistory);
    amr_history_exists_ = true;
  }

  std::string amr_id =
      "(SELECT Id FROM AutonomousMobileRobot WHERE ApplicationUuid='" + logging_info.uuid + "')";
  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* AmrId */ amr_id.c_str(),
      /* PosX_m */ logging_info.x,
      /* PosY_m */ logging_info.y,
      /* State */ logging_info.state);
  log_(getInsertStatement(kAmrHistory, t));
  // logging_info.z_
}

// * AutonomousMobileRobot
TableDefinition kAutonomousMobileRobot("AutonomousMobileRobot",
                                       {DatabaseColumnInfo{"Id"},
                                        {"Timestamp_ms", "%u", true},
                                        {"ApplicationUuid", "%s", true,
                                         "DeviceApplication(ApplicationUuid)"},
                                        {"FriendlyName", "%s"},
                                        {"ModelName", "%s"},
                                        {"IpLogicalService", "%s", true},
                                        {"PortLogicalService", "%u", true},
                                        {"IpPhysicalAsset", "%s", true},
                                        {"PortPhysicalAsset", "%u", true},
                                        {"IpLocalAsset", "%s", true},
                                        {"PortLocalAsset", "%u", true},
                                        {"LoadTime_ms", "%u", true},
                                        {"UnloadTime_ms", "%u", true},
                                        {"MaxWeight_kg", "%f", true},
                                        {"MaxVelocity_mps", "%f", true},
                                        {"MinVelocity_mps", "%f", true},
                                        {"MaxAcceleration_mpss", "%f", true},
                                        {"MaxDeceleration_mpss", "%f", true}});
static const std::string kCreateAmr = getCreateTableStatement(kAutonomousMobileRobot);
static bool amr_exists_ = false;

void CppsLoggerNs3::logAGV(const AGVLoggingInfo &agv_info) {
  if (!amr_exists_) {
    log_(kCreateAmr);
    amr_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* ApplicationUuid */ uuid_.c_str(),
      /* FriendlyName */ agv_info.friendly_name.c_str(),
      /* ModelName */ agv_info.model_name.c_str(),
      /* IpLogicalService */ agv_info.ip_logical_core.c_str(),
      /* PortLogicalService */ agv_info.port_logical_core,
      /* IpPhysicalAsset */ agv_info.ip_physical.c_str(),
      /* PortPhysicalAsset */ agv_info.port_physical,
      /* IpLocalAsset */ agv_info.ip_logical_asset.c_str(),
      /* PortLocalAsset */ agv_info.port_logical_asset,
      /* LoadTime_ms */ agv_info.load_time,
      /* UnloadTime_ms */ agv_info.unload_time,
      /* MaxWeight_kg */ agv_info.max_weight,
      /* MaxVelocity_mps */ agv_info.max_velocity,
      /* MinVelocity_mps */ agv_info.min_velocity,
      /* MaxAcceleration_mpss */ agv_info.max_acceleration,
      /* MaxDeceleration_mpss */ agv_info.min_acceleration);
  log_(getInsertStatement(kAutonomousMobileRobot, t));
  //     agv_info.manufacturer_.c_str(),
  //     agv_info.model_name_.c_str(), agv_info.serial_number_,
  //     agv_info.load_carrier_type_.c_str()
}

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
      /* AgvUuid */ logging_info.agv.c_str(),
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

void CppsLoggerNs3::logMaterialFlow(const uint64_t &timestamp, const std::string &mf_uuid,
                                    const std::string &ip, uint16_t port, uint8_t state) {
  if (!material_flow_exists_) {
    log_(kCreateMaterialFlow);
    material_flow_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ timestamp,
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
  for (auto const &[key, name] : kMapCppsMessageTypeStrings) {
    auto t = std::make_tuple(
        /* Id */ key,
        /* Name */ name.c_str());
    log_(getInsertStatement(kEnumCppsMessageType, t));
  }
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

// * Service
TableDefinition kService("Service", {{"Uuid", "%s", true, "", true},
                                     {"StartTime_ms", "%u"},
                                     {"Type", "%u", true}});
static const std::string kCreateService = getCreateTableStatement(kService);
static bool service_exists_ = false;

void CppsLoggerNs3::logService(const std::string &uuid, uint8_t type) {
  if (!service_exists_) {
    log_(kCreateService);
    service_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Uuid */ uuid.c_str(),
      /* StartTime_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Type */ type);
  log_(getInsertStatement(kService, t));
}

// * ServiceTransport
TableDefinition kServiceTransport("ServiceTransport",
                                  {DatabaseColumnInfo{"Id"},
                                   {"Uuid", "%s", true},
                                   {"AmrId", "sql%u", true, "AutonomousMobileRobot(Id)"},
                                   {"LoadCarrierType", "%s", true},
                                   {"MaxWeightPayload_kg", "%f", true}});
// PRIMARY KEY(service_uuid, agv_uuid, timestamp, state)
static const std::string kCreateServiceTransport = getCreateTableStatement(kServiceTransport);
static bool service_exists_transport_ = false;

void CppsLoggerNs3::logTransportService(const sola::Service &service, bool active) {
  if (!service_exists_transport_) {
    log_(kCreateServiceTransport);
    service_exists_transport_ = true;
  }

  // int state = (active ? 1 : 0);
  // auto load_carrier = std::any_cast<std::string>(service.key_values_.at("loadcarriertype"));
  auto agv_uuid = std::any_cast<std::string>(service.key_values.at("agvuuid"));
  auto type = std::any_cast<std::string>(service.key_values.at("servicetype"));
  auto max_payload = std::any_cast<float>(service.key_values.at("maxpayload"));

  logService(service.uuid, 0);
  std::string uuid = service.uuid;
  std::string amr_id =
      "(SELECT Id FROM AutonomousMobileRobot WHERE ApplicationUuid='" + agv_uuid + "')";
  auto t = std::make_tuple(
      /* Uuid */ uuid.c_str(),
      /* AmrId */ amr_id.c_str(),
      /* LoadCarrierType */ type.c_str(),  // TODO: Change to id based field?
      /* MaxWeightPayload_kg */ max_payload);
  log_(getInsertStatement(kServiceTransport, t));
  // load_carrier.c_str(), state
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

// * TransportOrder
TableDefinition kTransportOrder("TransportOrder",
                                {DatabaseColumnInfo{"Id"},
                                 {"OrderUuid", "%s", true},
                                 {"MaterialFlowUuid", "%s", true,
                                  "DeviceApplication(ApplicationUuid)"},
                                 {"Name", "%s", true},
                                 {"MaterialFlowId", "sql%u", false, "MaterialFlow(Id)"},
                                 {"PickupX_m", "%lf", true},
                                 {"PickupY_m", "%lf", true},
                                 {"DeliveryX_m", "%lf", true},
                                 {"DeliveryY_m", "%lf", true},
                                 // {"PickupStationId", "%u", false, "Station(Id)"},
                                 // {"DeliveryStationId", "%u", false, "Station(Id)"},
                                 {"LoadCarrierType", "%s", true},
                                 {"Weight_kg", "%f", true},
                                 {"EarliestStart_ms", "%lf", true},
                                 {"LatestFinish_ms", "%lf", true},
                                 {"PrecedenceConstraints", "%s", true}});
static const std::string kCreateTransportOrder = getCreateTableStatement(kTransportOrder);
static bool transport_order_exists_ = false;

void CppsLoggerNs3::logTransportOrder(const Task &order, uint32_t pickup_station_id,
                                      uint32_t delivery_station_id, const std::string &mf_uuid) {
  // TODO: Use ids instead of x/y for Stations
  if (!transport_order_exists_) {
    log_(kCreateTransportOrder);
    transport_order_exists_ = true;
  }

  double earliest_start = order.time_window.getEarliestStart();
  double latest_finish = order.time_window.getLatestFinish();

  std::string precedence_constraints;
  for (const std::string &constraint_uuid : order.precedence_constraints.getConstraintUUIDs()) {
    precedence_constraints += constraint_uuid + ";";
  }

  auto table = kTransportOrder;
  std::string order_uuid = order.getUUID();
  std::string name = order.getName();
  ns3::Vector3D start = order.getPickupLocation();
  ns3::Vector3D stop = order.getDeliveryLocation();
  std::string load_carrier_type = order.getAbilityRequirement().getLoadCarrier().getTypeAsString();

  if (mf_uuid.empty()) {
    table.columns[4] = {"MaterialFlowId", "NULL"};
    auto t = std::make_tuple(
        /* OrderUuid */ order_uuid.c_str(),
        /* MaterialFlowUuid */ uuid_.c_str(),
        /* Name */ name.c_str(),
        /* PickupX_m */ start.x,
        /* PickupY_m */ start.y,
        /* DeliveryX_m */ stop.x,
        /* DeliveryY_m */ stop.y,
        // /* PickupStationId */ pickup_station_id,
        // /* DeliveryStationId */ delivery_station_id,
        /* LoadCarrierType */
        load_carrier_type.c_str(),  // TODO: Change to id based field?
        /* Weight_kg */ order.getAbilityRequirement().getMaxPayloadWeight(),
        /* EarliestStart_ms */ earliest_start,
        /* LatestFinish_ms */ latest_finish,
        /* PrecedenceConstraints */ precedence_constraints.c_str());
    log_(getInsertStatement(table, t));
  } else {
    std::string material_flow = "(SELECT Id FROM MaterialFlow WHERE Uuid='" + mf_uuid + "')";
    auto t = std::make_tuple(
        /* OrderUuid */ order_uuid.c_str(),
        /* MaterialFlowUuid */ uuid_.c_str(),
        /* Name */ name.c_str(),
        /* MaterialFlowId */ material_flow.c_str(),
        /* PickupX_m */ start.x,
        /* PickupY_m */ start.y,
        /* DeliveryX_m */ stop.x,
        /* DeliveryY_m */ stop.y,
        // /* PickupStationId */ pickup_station_id,
        // /* DeliveryStationId */ delivery_station_id,
        /* LoadCarrierType */
        load_carrier_type.c_str(),  // TODO: Change to id based field?
        /* Weight_kg */ order.getAbilityRequirement().getMaxPayloadWeight(),
        /* EarliestStart_ms */ earliest_start,
        /* LatestFinish_ms */ latest_finish,
        /* PrecedenceConstraints */ precedence_constraints.c_str());
    log_(getInsertStatement(table, t));
  }
}

void CppsLoggerNs3::logTransportOrder(const Task &order, uint32_t pickup_station_id,
                                      uint32_t delivery_station_id) {
  logTransportOrder(order, pickup_station_id, delivery_station_id, "");
}

// * TransportOrderHistory
TableDefinition kTransportOrderHistory("TransportOrderHistory",
                                         {DatabaseColumnInfo{"Id"},
                                          {"TransportOrderId", "sql%u", true, "TransportOrder(Id)"},
                                          {"Timestamp_ms", "%u", true},
                                          {"State", "%u", true},
                                          {"PosX_m", "%f", true},
                                          {"PosY_m", "%f", true},
                                          {"AmrId", "sql%u", false, "AutonomousMobileRobot(Id)"}
                                          /*{"TransportServiceId", "%u", false,
                                           "TransportService(Id)"}*/});
static const std::string kCreateTransportOrderHistory =
    getCreateTableStatement(kTransportOrderHistory);
static bool transport_order_history_exists_ = false;

void CppsLoggerNs3::logTransportOrderUpdate(const Task &order, const std::string &assigned_agv) {
  if (!transport_order_history_exists_) {
    log_(kCreateTransportOrderHistory);
    transport_order_history_exists_ = true;
  }

  ns3::Vector pos = order.getCurrentPosition();
  auto order_state = order.getOrderState();

  std::string transport_order_id =
      "(SELECT Id FROM TransportOrder WHERE OrderUuid='" + order.getUUID() + "')";

  auto table = kTransportOrderHistory;
  if (!assigned_agv.empty()) {
    std::string amr_id =
        "(SELECT Id FROM AutonomousMobileRobot WHERE ApplicationUuid='" + assigned_agv + "')";
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

void CppsLoggerNs3::logTransportOrderUpdate(const std::string &order_uuid, uint32_t status,
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
        "(SELECT Id FROM AutonomousMobileRobot WHERE ApplicationUuid='" + assigned_agv + "')";
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
