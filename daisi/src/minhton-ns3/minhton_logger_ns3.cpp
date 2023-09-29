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

#include "minhton_logger_ns3.h"

#include <stdexcept>

#include "ns3/simulator.h"

#define TableDefinition static const DatabaseTable
#define ViewDefinition static const std::unordered_map<std::string, std::string>

namespace minhton {

// From minhton message_header.cpp
extern const std::unordered_map<uint32_t, std::string> kMapMinhtonMessageTypeStrings;

// Refer to DatabaseTable and DatabaseColumnInfo definitions in ../logging/definitions.h
//! Don't forget to use .c_str() to convert std::string into a char array

// * Event
void MinhtonLoggerNs3::logEvent(const LoggerInfoAddEvent &info) {
  log_event_(std::to_string(info.event_id), static_cast<uint8_t>(info.event_type), uuid_);
}

// * FindQuery
TableDefinition kFindQuery("MinhtonFindQuery",
                           {DatabaseColumnInfo{"Id"},
                            {"Timestamp_ms", "%lu", true},
                            {"EventId", "%lu", true, "Event(Id)"},
                            {"NodeUuid", "%s", true, "MinhtonNode(PositionUuid)"},
                            {"Query", "%s"}});
static const std::string kCreateFindQuery = getCreateTableStatement(kFindQuery);
static bool find_query_exists_ = false;

void MinhtonLoggerNs3::logFindQuery(const LoggerInfoAddFindQuery &info) {
  if (!find_query_exists_) {
    log_(kCreateFindQuery);
    find_query_exists_ = true;
  }

  auto t = std::make_tuple(/* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                           /* EventId */ info.event_id,
                           /* NodeUuid */ info.node_uuid.c_str(),
                           /* Query */ info.query.c_str());
  log_(getInsertStatement(kFindQuery, t));
}

// * FindQueryResult
TableDefinition kFindQueryResult("MinhtonFindQueryResult",
                                 {DatabaseColumnInfo{"Id"},
                                  {"Timestamp_ms", "%lu", true},
                                  {"EventId", "%lu", true, "Event(Id)"},
                                  {"NodeUuid", "%s", true, "MinhtonNode(PositionUuid)"}});
static const std::string kCreateFindQueryResult = getCreateTableStatement(kFindQueryResult);
static bool find_query_result_exists_ = false;

void MinhtonLoggerNs3::logFindQueryResult(const LoggerInfoAddFindQueryResult &info) {
  if (!find_query_result_exists_) {
    log_(kCreateFindQueryResult);
    find_query_result_exists_ = true;
  }

  auto t = std::make_tuple(/* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                           /* EventId */ info.event_id,
                           /* NodeUuid */ info.node_uuid.c_str());
  log_(getInsertStatement(kFindQueryResult, t));
}

// * MinhtonPhysicalNodeInfo
TableDefinition kMinhtonPhysicalNodeInfo("MinhtonPhysicalNodeInfo",
                                         {{"ApplicationUuid", "%s", true,
                                           "DeviceApplication(ApplicationUuid)", true},
                                          {"Ip", "%s", true},
                                          {"Port", "%u", true}});
static const std::string kCreateMinhtonPhysicalNodeInfo =
    getCreateTableStatement(kMinhtonPhysicalNodeInfo);
static bool minhton_p_node_info_exists_ = false;

void MinhtonLoggerNs3::logPhysicalNodeInfo(const LoggerPhysicalNodeInfo &info) {
  if (!minhton_p_node_info_exists_) {
    log_(kCreateMinhtonPhysicalNodeInfo);
    minhton_p_node_info_exists_ = true;
  }

  auto t = std::make_tuple(/* ApplicationUuid */ uuid_.c_str(),
                           /* Ip */ info.ip.c_str(),
                           /* Port */ info.port);
  log_(getInsertStatement(kMinhtonPhysicalNodeInfo, t));
}

// * MinhtonNode
TableDefinition kMinhtonNode("MinhtonNode",
                             {{"PositionUuid", "%s", true, "", true},
                              {"ApplicationUuid", "%s", true, "DeviceApplication(ApplicationUuid)"},
                              {"Level", "%u"},
                              {"Number", "%u"},
                              {"Fanout", "%u"}});
static const std::string kCreateMinhtonNode = getCreateTableStatement(kMinhtonNode);
static bool minhton_node_exists_ = false;

void MinhtonLoggerNs3::logNode(const LoggerInfoAddNode &info) {
  if (!minhton_node_exists_) {
    log_(kCreateMinhtonNode);
    minhton_node_exists_ = true;
  }

  if (info.initialized) {
    auto t = std::make_tuple(/* PositionUuid */ info.position_uuid.c_str(),
                             /* ApplicationUuid */ uuid_.c_str(),
                             /* Level */ info.level,
                             /* Number */ info.number,
                             /* Fanout */ info.fanout);
    log_(getInsertStatement(kMinhtonNode, t));
  } else {
    auto t = std::make_tuple(/* PositionUuid */ info.position_uuid.c_str(),
                             /* ApplicationUuid */ uuid_.c_str());
    auto table = kMinhtonNode;
    table.columns[2] = {"Level", "NULL"};
    table.columns[3] = {"Number", "NULL"};
    table.columns[4] = {"Fanout", "NULL"};
    log_(getInsertStatement(table, t));
  }
}

// * MinhtonNodeState
TableDefinition kMinhtonNodeState("MinhtonNodeState",
                                  {DatabaseColumnInfo{"Id"},
                                   {"PositionUuid", "%s", true, "MinhtonNode(PositionUuid)"},
                                   {"Timestamp_ms", "%u", true},
                                   {"State", "%u", true},
                                   {"EventId", "%lu", true, "Event(Id)"}},
                                  "UNIQUE(PositionUuid, State)");
static const std::string kCreateMinhtonNodeState = getCreateTableStatement(kMinhtonNodeState);
static bool minhton_node_state_exists_ = false;

TableDefinition kEnumMinhtonNodeState("enumMinhtonNodeState",
                                      {{"Id", "%u", true, "", true}, {"Name", "%s", true}});
static const std::string kCreateEnumMinhtonNodeState =
    getCreateTableStatement(kEnumMinhtonNodeState);

void MinhtonLoggerNs3::logMinhtonNodeStates() {
  // adding a key-value map to get the string for easier debugging purposes
  const std::map<NodeStatus, std::string> map_minhton_node_state_strings = {
      {NodeStatus::kUninit, "UNINIT"},
      {NodeStatus::kRunning, "RUNNING"},
      {NodeStatus::kLeft, "LEFT"},
      {NodeStatus::kFailed, "FAILED"}};
  for (auto const &[key, name] : map_minhton_node_state_strings) {
    auto t = std::make_tuple(
        /* Id */ key,
        /* Name */ name.c_str());
    log_(getInsertStatement(kEnumMinhtonNodeState, t));
  }
}

ViewDefinition kNodeStateReplacements = {
    {"PositionUuid", "N1.Level AS Level, N1.Number AS Number, Net1.Ip AS Ip, Net1.Port AS Port"},
    {"State", "enumMinhtonNodeState.Name AS State"}};
static const std::string kCreateViewMinhtonNodeState = getCreateViewStatement(
    kMinhtonNodeState, kNodeStateReplacements,
    {"LEFT JOIN MinhtonNode AS N1 ON MinhtonNodeState.PositionUuid = N1.PositionUuid",
     "LEFT JOIN MinhtonPhysicalNodeInfo AS Net1 ON N1.ApplicationUuid = Net1.ApplicationUuid",
     "LEFT JOIN enumMinhtonNodeState ON MinhtonNodeState.State = enumMinhtonNodeState.Id"});

void MinhtonLoggerNs3::logNodeUninit(const LoggerInfoNodeState &info) {
  if (!minhton_node_state_exists_) {
    log_(kCreateMinhtonNodeState);
    minhton_node_state_exists_ = true;

    log_(kCreateEnumMinhtonNodeState);
    logMinhtonNodeStates();

    log_(kCreateViewMinhtonNodeState);
  }

  auto t = std::make_tuple(/* PositionUuid */ info.position_uuid.c_str(),
                           /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                           /* State */ NodeStatus::kUninit,
                           /* EventId*/ info.event_id);
  log_(getInsertStatement(kMinhtonNodeState, t));
}

void MinhtonLoggerNs3::logNodeRunning(const LoggerInfoNodeState &info) {
  if (!minhton_node_state_exists_) {
    log_(kCreateMinhtonNodeState);
    minhton_node_state_exists_ = true;

    log_(kCreateEnumMinhtonNodeState);
    logMinhtonNodeStates();

    log_(kCreateViewMinhtonNodeState);
  }

  auto t = std::make_tuple(/* PositionUuid */ info.position_uuid.c_str(),
                           /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                           /* State */ NodeStatus::kRunning,
                           /* EventId*/ info.event_id);
  log_(getInsertStatement(kMinhtonNodeState, t));
}

void MinhtonLoggerNs3::logNodeLeft(const LoggerInfoNodeState &info) {
  if (!minhton_node_state_exists_) {
    log_(kCreateMinhtonNodeState);
    minhton_node_state_exists_ = true;

    log_(kCreateEnumMinhtonNodeState);
    logMinhtonNodeStates();

    log_(kCreateViewMinhtonNodeState);
  }

  auto t = std::make_tuple(/* PositionUuid */ info.position_uuid.c_str(),
                           /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                           /* State */ NodeStatus::kLeft,
                           /* EventId*/ info.event_id);
  log_(getInsertStatement(kMinhtonNodeState, t));
}

// * MinhtonTraffic
TableDefinition kMinhtonTraffic("MinhtonTraffic",
                                {DatabaseColumnInfo{"Id"},
                                 {"Timestamp_ms", "%lu", true},
                                 {"MsgType", "%u", true},
                                 {"Mode", "%u", true},
                                 {"EventId", "%lu", true, "Event(Id)"},
                                 {"RefEventId", "%lu", true, "Event(Id)"},
                                 {"SenderNodeUuid", "%s", true, "MinhtonNode(PositionUuid)"},
                                 {"TargetNodeUuid", "%s", true, "MinhtonNode(PositionUuid)"},
                                 {"PrimaryOtherNodeUuid", "%s", false, "MinhtonNode(PositionUuid)"},
                                 {"SecondaryOtherNodeUuid", "%s", false,
                                  "MinhtonNode(PositionUuid)"},
                                 {"Content", "%s"}});
static const std::string kCreateMinhtonTraffic = getCreateTableStatement(kMinhtonTraffic);
static bool minhton_traffic_exists_ = false;

TableDefinition kEnumMinhtonMessageType("enumMinhtonMessageType",
                                        {{"Id", "%u", true, "", true}, {"Name", "%s", true}});
static const std::string kCreateEnumMinhtonMessageType =
    getCreateTableStatement(kEnumMinhtonMessageType);

void MinhtonLoggerNs3::logMinhtonMessageTypes() {
  for (auto const &[key, name] : kMapMinhtonMessageTypeStrings) {
    auto t = std::make_tuple(
        /* Id */ key,
        /* Name */ name.c_str());
    log_(getInsertStatement(kEnumMinhtonMessageType, t));
  }
}

ViewDefinition kTrafficReplacements = {
    {"MsgType", "enumMinhtonMessageType.Name AS MessageType"},
    {"SenderNodeUuid", "SNode.Level AS SLevel, SNode.Number AS SNumber, SNetwork.Ip AS SIp"},
    {"TargetNodeUuid", "TNode.Level AS TLevel, TNode.Number AS TNumber, TNetwork.Ip AS TIp"},
    {"PrimaryOtherNodeUuid",
     "Node1.Level AS N1Level, Node1.Number AS N1Number, Network1.Ip AS N1Ip"},
    {"SecondaryOtherNodeUuid",
     "Node2.Level AS N2Level, Node2.Number AS N2Number, Network2.Ip AS N2Ip"}};
static const std::string kCreateViewMinhtonTraffic = getCreateViewStatement(
    kMinhtonTraffic, kTrafficReplacements,
    {"LEFT JOIN enumMinhtonMessageType ON MinhtonTraffic.MsgType = enumMinhtonMessageType.Id",
     "LEFT JOIN MinhtonNode AS SNode ON MinhtonTraffic.SenderNodeUuid = SNode.PositionUuid",
     "LEFT JOIN MinhtonPhysicalNodeInfo AS SNetwork ON SNode.ApplicationUuid = "
     "SNetwork.ApplicationUuid",
     "LEFT JOIN MinhtonNode AS TNode ON MinhtonTraffic.TargetNodeUuid = TNode.PositionUuid",
     "LEFT JOIN MinhtonPhysicalNodeInfo AS TNetwork ON TNode.ApplicationUuid = "
     "TNetwork.ApplicationUuid",
     "LEFT JOIN MinhtonNode AS Node1 ON MinhtonTraffic.PrimaryOtherNodeUuid = Node1.PositionUuid",
     "LEFT JOIN MinhtonPhysicalNodeInfo AS Network1 ON Node1.ApplicationUuid = "
     "Network1.ApplicationUuid",
     "LEFT JOIN MinhtonNode AS Node2 ON MinhtonTraffic.SecondaryOtherNodeUuid = Node2.PositionUuid",
     "LEFT JOIN MinhtonPhysicalNodeInfo AS Network2 ON Node2.ApplicationUuid = "
     "Network2.ApplicationUuid"});

void MinhtonLoggerNs3::logTraffic(const MessageLoggingInfo &info) {
  if (!minhton_traffic_exists_) {
    log_(kCreateMinhtonTraffic);
    minhton_traffic_exists_ = true;

    log_(kCreateEnumMinhtonMessageType);
    logMinhtonMessageTypes();

    log_(kCreateViewMinhtonTraffic);
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* MsgType */ info.cmd_type,
      /* Mode */ info.mode,
      /* EventId */ info.event_id,
      /* RefEventId */ info.ref_event_id,
      /* SenderNodeUuid */ info.sender_uuid.c_str(),
      /* TargetNodeUuid */ info.target_uuid.c_str(),
      /* PrimaryOtherNodeUuid */ info.additional_info.primary_other_uuid.c_str(),
      /* SecondaryOtherNodeUuid */ info.additional_info.secondary_other_uuid.c_str(),
      /* Content */ info.additional_info.content.c_str());
  log_(getInsertStatement(kMinhtonTraffic, t));
}

// * SearchContent
TableDefinition kSearchContent("MinhtonSearchContent",
                               {DatabaseColumnInfo{"Id"},
                                {"Timestamp_ms", "%lu", true},
                                {"NodeUuid", "%s", true, "MinhtonNode(PositionUuid)"},
                                {"State", "%u", true},
                                {"AttributeName", "%s"},
                                {"Type", "%u", true},
                                {"Text", "%s"}});
static const std::string kCreateCreateTraffic = getCreateTableStatement(kSearchContent);
static bool search_content_exists_ = false;

void MinhtonLoggerNs3::logContent(const LoggerInfoAddContent &info) {
  if (!search_content_exists_) {
    log_(kCreateCreateTraffic);
    search_content_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* NodeUuid */ info.node_uuid.c_str(),
      /* State */ info.content_status,
      /* AttributeName */ info.attribute_name.c_str(),
      /* Type */ info.content_type,
      /* Text */ info.content_text.c_str());
  log_(getInsertStatement(kSearchContent, t));
}

// * MinhtonSearchTest
TableDefinition kSearchTest("MinhtonSearchTest", {DatabaseColumnInfo{"Id"},
                                                  {"Timestamp_ms", "%lu", true},
                                                  {"EventId", "%lu", true, "Event(Id)"},
                                                  {"State", "%u", true},
                                                  {"SenderLevel", "%u"},
                                                  {"SenderNumber", "%u"},
                                                  {"TargetLevel", "%u"},
                                                  {"TargetNumber", "%u"},
                                                  {"HopLevel", "%u"},
                                                  {"HopNumber", "%u"}});
static const std::string kCreateSearchTest = getCreateTableStatement(kSearchTest);
static bool search_test_exists_ = false;

void MinhtonLoggerNs3::logSearchExactTest(const LoggerInfoSearchExact &info) {
  if (!search_test_exists_) {
    log_(kCreateSearchTest);
    search_test_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* EventId */ info.event_id,
      /* State */ info.status,
      /* SenderLevel */ info.sender_level,
      /* SenderNumber */ info.sender_number,
      /* TargetLevel */ info.target_level,
      /* TargetNumber */ info.target_number,
      /* HopLevel */ info.hop_level,
      /* HopNumber */ info.hop_number);
  log_(getInsertStatement(kSearchTest, t));
}

// * RoutingInfo
TableDefinition kRoutingInfo("MinhtonRoutingInfo",
                             {DatabaseColumnInfo{"Id"},
                              {"Timestamp_ms", "%lu", true},
                              {"EventId", "%lu", true, "Event(Id)"},
                              {"NodeUuid", "%s", true, "MinhtonNode(PositionUuid)"},
                              {"NeighborNodeUuid", "%s", true, "MinhtonNode(PositionUuid)"},
                              {"Relationship", "%u", true}});
static const std::string kCreateRoutingInfo = getCreateTableStatement(kRoutingInfo);
static bool routing_info_exists_ = false;

TableDefinition kEnumMinhtonRelationship("enumMinhtonRelationship",
                                         {{"Id", "%u", true, "", true}, {"Name", "%s", true}});
static const std::string kCreateEnumMinhtonRelationship =
    getCreateTableStatement(kEnumMinhtonRelationship);

void MinhtonLoggerNs3::logMinhtonRelationships() {
  // adding a key-value map to get the string for easier debugging purposes
  const std::map<NeighborRelationship, std::string> map_minhton_relationship_strings = {
      {NeighborRelationship::kParent, "PARENT"},
      {NeighborRelationship::kChild, "CHILD"},
      {NeighborRelationship::kAdjacentLeft, "ADJACENT_LEFT"},
      {NeighborRelationship::kAdjacentRight, "ADJACENT_RIGHT"},
      {NeighborRelationship::kRoutingTableNeighbor, "RT_NEIGHBOR"},
      {NeighborRelationship::kRoutingTableNeighborChild, "RT_NEIGHBOR_CHILD"},
      {NeighborRelationship::kUnknownRelationship, "UNKNOWN"}};
  for (auto const &[key, name] : map_minhton_relationship_strings) {
    auto t = std::make_tuple(
        /* Id */ key,
        /* Name */ name.c_str());
    log_(getInsertStatement(kEnumMinhtonRelationship, t));
  }
}

ViewDefinition kRtReplacements = {
    {"NodeUuid",
     "N1.Level AS NodeLevel, N1.Number AS NodeNumber, Net1.Ip AS NodeIp, Net1.Port AS NodePort"},
    {"NeighborNodeUuid", "N2.Level AS NeighborLevel, N2.Number AS NeighborNumber, "
                         "Net2.Ip AS NeighborIp, Net2.Port AS NeighborPort"},
    {"Relationship", "enumMinhtonRelationship.Name AS Relationship"}};
static const std::string kCreateViewRoutingInfo = getCreateViewStatement(
    kRoutingInfo, kRtReplacements,
    {"LEFT JOIN MinhtonNode AS N1 ON MinhtonRoutingInfo.NodeUuid = N1.PositionUuid",
     "LEFT JOIN MinhtonPhysicalNodeInfo AS Net1 ON N1.ApplicationUuid = Net1.ApplicationUuid",
     "LEFT JOIN MinhtonNode AS N2 ON MinhtonRoutingInfo.NeighborNodeUuid = N2.PositionUuid",
     "LEFT JOIN MinhtonPhysicalNodeInfo AS Net2 ON N2.ApplicationUuid = Net2.ApplicationUuid",
     "LEFT JOIN enumMinhtonRelationship ON MinhtonRoutingInfo.Relationship = "
     "enumMinhtonRelationship.Id"});

void MinhtonLoggerNs3::logNeighbor(const LoggerInfoAddNeighbor &info) {
  if (!routing_info_exists_) {
    log_(kCreateRoutingInfo);
    routing_info_exists_ = true;

    log_(kCreateEnumMinhtonRelationship);
    logMinhtonRelationships();

    log_(kCreateViewRoutingInfo);
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* EventId */ info.event_id,
      /* NodeUuid */ info.node_uuid.c_str(),
      /* NeighborNodeUuid */ info.neighbor_node_uuid.c_str(),
      /* Relationship */ info.relationship);
  log_(getInsertStatement(kRoutingInfo, t));
}

// * Constructor & Other methods
MinhtonLoggerNs3::MinhtonLoggerNs3(LogDeviceApp log_device_application, LogFunction log,
                                   LogEvent log_event)
    : LoggerInterface("NOT-KNOWN-YET"),
      log_device_application_(std::move(log_device_application)),
      log_(std::move(log)),
      log_event_(std::move(log_event)) {}

MinhtonLoggerNs3::~MinhtonLoggerNs3() {
  auto current_time = ns3::Simulator::Now().GetMilliSeconds();
  log_(toSQL("UPDATE DeviceApplication SET StopTime_ms=%lu WHERE ApplicationUuid='%s';",
             current_time, uuid_.c_str()));
}

void MinhtonLoggerNs3::logCritical(const std::string & /*msg*/) const {}
void MinhtonLoggerNs3::logWarning(const std::string & /*msg*/) const {}
void MinhtonLoggerNs3::logInfo(const std::string & /*msg*/) const {}
void MinhtonLoggerNs3::logDebug(const std::string & /*msg*/) const {}

}  // namespace minhton

#undef TableDefinition
#undef ViewDefinition
