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

#include "natter_logger_ns3.h"

#include <sstream>
#include <stdexcept>

#include "ns3/simulator.h"

#define TableDefinition static const DatabaseTable
#define ViewDefinition static const std::unordered_map<std::string, std::string>

namespace natter::logging {

// Refer to DatabaseTable and DatabaseColumnInfo definitions in ../logging/definitions.h
//! Don't forget to use .c_str() to convert std::string into a char array

// * Event
void NatterLoggerNs3::logNatterEvent(uint16_t event_type, solanet::UUID event_id) {
  auto event = solanet::uuidToString(event_id);
  log_event_(event, event_type, uuid_);
}

// * NatterConnection
TableDefinition kNatterConnection("NatterConnection",
                                  {DatabaseColumnInfo{"Id"},
                                   {"Timestamp_ms", "%lu", true},
                                   {"Active", "%i", true},
                                   {"NodeId", "sql%u", true, "NatterNode(Id)"},
                                   {"NewNodeId", "sql%u", true, "NatterNode(Id)"}});
static const std::string kCreateNatterConnection = getCreateTableStatement(kNatterConnection);
static bool natter_connection_exists_ = false;

ViewDefinition kNatterConnectionReplacements = {
    {"NodeId",
     "N1.ApplicationUuid AS N1ApplicationUuid, N1.Level AS N1Level, N1.Number AS N1Number, "
     "N1.Ip AS N1Ip, N1.Port AS N1Port"},
    {"NewNodeId",
     "N2.ApplicationUuid AS N2ApplicationUuid, N2.Level AS N2Level, N2.Number AS N2Number, "
     "N2.Ip AS N2Ip, N2.Port AS N2Port"}};
static const std::string kCreateViewNatterConnection =
    getCreateViewStatement(kNatterConnection, kNatterConnectionReplacements,
                           {"LEFT JOIN NatterNode AS N1 ON NatterConnection.NodeId = N1.Id",
                            "LEFT JOIN NatterNode AS N2 ON NatterConnection.NewNodeId = N2.Id"});

void NatterLoggerNs3::logNs3PeerConnection(uint64_t timestamp, bool active, solanet::UUID node_uuid,
                                           solanet::UUID new_node_uuid) {
  if (!natter_connection_exists_) {
    log_(kCreateNatterConnection);
    log_(kCreateViewNatterConnection);
    natter_connection_exists_ = true;
  }

  std::string node_id = "(SELECT Id FROM NatterNode WHERE ApplicationUuid='" +
                        solanet::uuidToString(node_uuid) + "')";
  std::string new_node_id = "(SELECT Id FROM NatterNode WHERE ApplicationUuid='" +
                            solanet::uuidToString(new_node_uuid) + "')";
  auto t = std::make_tuple(
      /* Timestamp_ms */ timestamp,
      /* Active */ active ? 1 : 0,
      /* NodeId */ node_id.c_str(),
      /* NewNodeId */ new_node_id.c_str());
  log_(getInsertStatement(kNatterConnection, t));
}

// * NatterMessage
TableDefinition kMessage("NatterMessage", {DatabaseColumnInfo{"Id"},
                                           {"Uuid", "%s", true},
                                           /*{"Content", "%s"},*/ {"Topic", "%s"}});
static const std::string kCreateMessage = getCreateTableStatement(kMessage);
static bool message_exists_ = false;

// TODO: Enable Content after passing unserialized string?
void NatterLoggerNs3::logNewMessage(const std::string &topic, const std::string &msg,
                                    solanet::UUID msg_uuid) {
  if (!message_exists_) {
    log_(kCreateMessage);
    message_exists_ = true;
  }

  auto uuid_string = solanet::uuidToString(msg_uuid);
  auto t = std::make_tuple(
      /* Uuid */ uuid_string.c_str(),
      // /* Content */ msg.c_str(),
      /* Topic */ topic.c_str());
  log_(getInsertStatement(kMessage, t));
}

// * NatterNode
// TODO: Combine with MinhtonNode to shared table in logger manager?
TableDefinition kNatterNode("NatterNode",
                            {DatabaseColumnInfo{"Id"},
                             {"ApplicationUuid", "%s", true, "DeviceApplication(ApplicationUuid)"},
                             {"Timestamp_ms", "%lu", true},
                             {"Level", "%u"},
                             {"Number", "%u"},
                             {"Ip", "%s", true},
                             {"Port", "%u", true}});
static const std::string kCreateNatterNode = getCreateTableStatement(kNatterNode);
static bool natter_node_exists_ = false;

void NatterLoggerNs3::logNewPeer(const std::string &ip, uint16_t port, solanet::UUID uuid,
                                 const std::string &topic) const {
  // TODO: LOG
}

void NatterLoggerNs3::logRemovePeer(const std::string &ip, uint16_t port, solanet::UUID uuid,
                                    const std::string &topic) const {
  // TODO: LOG
}

void NatterLoggerNs3::logNewNetworkPeer(solanet::UUID uuid, const std::string &ip, uint16_t port,
                                        int level, int number) {
  if (!natter_node_exists_) {
    log_(kCreateNatterNode);
    natter_node_exists_ = true;
  }

  auto uuid_string = solanet::uuidToString(uuid);
  auto t = std::make_tuple(/* ApplicationUuid */ uuid_string.c_str(),
                           /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                           /* Level */ level,
                           /* Number */ number,
                           /* Ip */ ip.c_str(),
                           /* Port */ port);
  log_(getInsertStatement(kNatterNode, t));
}

// * NatterControlMessage
// TODO: Set Not null to true for TargetNodeId
TableDefinition kNatterCtrlMsg("NatterControlMessage",
                               {DatabaseColumnInfo{"Id"},
                                {"Timestamp_ms", "%lu", true},
                                {"Type", "%i", true},
                                {"Mode", "%i", true},
                                {"SenderNodeId", "sql%u", true, "NatterNode(Id)"},
                                {"TargetNodeId", "sql%u", false, "NatterNode(Id)"},
                                {"MessageId", "sql%u", false, "NatterMessage(Id)"}
                                /*{"AdditionalContent", "%s"}*/});
static const std::string kCreateTrafficForward = getCreateTableStatement(kNatterCtrlMsg);
static bool natter_ctrl_msg_exists_ = false;

ViewDefinition kNatterTrafficReplacements = {
    {"SenderNodeId",
     "SN.ApplicationUuid AS SApplicationUuid, SN.Level AS SLevel, SN.Number AS SNumber, "
     "SN.Ip AS SIp, SN.Port AS SPort"},
    {"TargetNodeId",
     "TN.ApplicationUuid AS TApplicationUuid, TN.Level AS TLevel, TN.Number AS TNumber, "
     "TN.Ip AS TIp, TN.Port AS TPort"},
    {"MessageId", "M.Uuid AS MessageUuid, M.Topic AS Topic"}};
static const std::string kCreateViewNatterTraffic = getCreateViewStatement(
    kNatterCtrlMsg, kNatterTrafficReplacements,
    {"LEFT JOIN NatterNode AS SN ON NatterControlMessage.SenderNodeId = SN.Id",
     "LEFT JOIN NatterNode AS TN ON NatterControlMessage.TargetNodeId = TN.Id",
     "LEFT JOIN NatterMessage AS M ON NatterControlMessage.MessageId = M.Id"});

void NatterLoggerNs3::logSendReceive(solanet::UUID msg_uuid, solanet::UUID sender,
                                     solanet::UUID own_uuid, MsgType type, Mode mode) {
  if (!natter_ctrl_msg_exists_) {
    log_(kCreateTrafficForward);
    log_(kCreateViewNatterTraffic);
    natter_ctrl_msg_exists_ = true;
  }

  // workaround for integrating NatterLoggerNs3 into cpps
  // uint64_t peer_id_sender = peer_pos_to_id_.find(sender) != peer_pos_to_id_.end() ?
  // peer_pos_to_id_.at(sender) : 0; uint64_t peer_id_own = peer_pos_to_id_.find(own_uuid) !=
  // peer_pos_to_id_.end() ? peer_pos_to_id_.at(own_uuid) : 0;

  // TODO: own_uuid is minhton posUUID sometimes when used from cpps
  std::string sender_node_id =
      "(SELECT Id FROM NatterNode WHERE ApplicationUuid='" + solanet::uuidToString(sender) + "')";
  std::string target_node_id =
      "(SELECT Id FROM NatterNode WHERE ApplicationUuid='" + solanet::uuidToString(own_uuid) + "')";
  std::string message_str = solanet::uuidToString(msg_uuid);

  auto table = kNatterCtrlMsg;
  if (message_str == solanet::uuidToString(solanet::UUID{})) {
    table.columns[6] = {"MessageId", "NULL"};
    auto t = std::make_tuple(/* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                             /* Type */ type,
                             /* Mode */ mode,
                             /* SenderNodeId */ sender_node_id.c_str(),
                             /* TargetNodeId */ target_node_id.c_str()
                             // /* AdditionalContent */ target.c_str()  // TODO
    );
    log_(getInsertStatement(table, t));
  } else {
    std::string message_id = "(SELECT Id FROM NatterMessage WHERE Uuid='" + message_str + "')";
    auto t = std::make_tuple(/* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                             /* Type */ type,
                             /* Mode */ mode,
                             /* SenderNodeId */ sender_node_id.c_str(),
                             /* TargetNodeId */ target_node_id.c_str(),
                             /* MessageId */ message_id.c_str()
                             // /* AdditionalContent */ target.c_str()  // TODO
    );
    log_(getInsertStatement(table, t));
  }
}

// TODO: * minhcast_broadcast
// static std::string createTableMinhcastBroadcast =
//     "CREATE TABLE minhcast_broadcast ( "
//     "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, "
//     "timestamp	INTEGER NOT NULL, "
//     "msg_id INTEGER NOT NULL,"
//     "own_level	INTEGER NOT NULL, "
//     "own_number     INTEGER  NOT NULL, "
//     "forward_up_limit	INTEGER NOT NULL,"
//     "forward_down_limit	INTEGER NOT NULL);";

void NatterLoggerNs3::logMinhcastBroadcast(solanet::UUID msg_id, uint32_t level, uint32_t number,
                                           uint32_t forward_up_limit, uint32_t forward_down_limit) {
  // log_(toSQL("INSERT INTO minhcast_broadcast VALUES (NULL, %lu, (SELECT id FROM "
  //            "message_ids WHERE message_id='%s'), %u, %u, %u, %u);",
  //            ns3::Simulator::Now().GetMicroSeconds(), solanet::uuidToString(msg_id).c_str(),
  //            level, number, forward_up_limit, forward_down_limit));
}

// * TopicMessage
TableDefinition kTopicMessage("TopicMessage",
                              {DatabaseColumnInfo{"Id"},
                               {"Timestamp_us", "%lu", true},
                               {"NodeId", "sql%u", true, "NatterNode(Id)"},
                               {"InitialSenderNodeId", "sql%u", true, "NatterNode(Id)"},
                               {"MessageId", "sql%u", true, "NatterMessage(Id)"},
                               {"Round", "%i", true},
                               {"Mode", "%i", true}});
static const std::string kCreateTopicMessage = getCreateTableStatement(kTopicMessage);
static bool topic_message_exists_ = false;

ViewDefinition kTopicMessageReplacements = {
    {"NodeId", "N1.ApplicationUuid AS NApplicationUuid, N1.Level AS NLevel, N1.Number AS NNumber, "
               "N1.Ip AS NIp, N1.Port AS NPort"},
    {"InitialSenderNodeId",
     "N2.ApplicationUuid AS ISNApplicationUuid, N2.Level AS ISNLevel, N2.Number AS ISNNumber, "
     "N2.Ip AS ISNIp, N2.Port AS ISNPort"},
    {"MessageId", "M.Uuid AS MessageUuid, M.Topic AS Topic"}};
static const std::string kCreateViewTopicMessage = getCreateViewStatement(
    kTopicMessage, kTopicMessageReplacements,
    {"LEFT JOIN NatterNode AS N1 ON TopicMessage.NodeId = N1.Id",
     "LEFT JOIN NatterNode AS N2 ON TopicMessage.InitialSenderNodeId = N2.Id",
     "LEFT JOIN NatterMessage AS M ON TopicMessage.MessageId = M.Id"});

void NatterLoggerNs3::logReceivedMessages(solanet::UUID node_uuid, solanet::UUID initial_sender,
                                          solanet::UUID message, uint32_t round) {
  if (!topic_message_exists_) {
    log_(kCreateTopicMessage);
    log_(kCreateViewTopicMessage);
    topic_message_exists_ = true;
  }

  // TODO: Messages should always identified by peer_uuid. Position can change, solanet::UUID not.
  // TODO: Node was a material flow or agv
  std::string node_id = "(SELECT Id FROM NatterNode WHERE ApplicationUuid='" +
                        solanet::uuidToString(node_uuid) + "')";
  std::string initial_sender_node_id = "(SELECT Id FROM NatterNode WHERE ApplicationUuid='" +
                                       solanet::uuidToString(initial_sender) + "')";
  std::string message_id =
      "(SELECT Id FROM NatterMessage WHERE Uuid='" + solanet::uuidToString(message) + "')";
  auto t = std::make_tuple(
      /* Timestamp_us */ ns3::Simulator::Now().GetMicroSeconds(),
      /* NodeId */ node_id.c_str(),
      /* InitialSenderNodeId */ initial_sender_node_id.c_str(),
      /* MessageId */ message_id.c_str(),
      /* Round */ round,
      /* Receive */ 1);
  log_(getInsertStatement(kTopicMessage, t));
}

// * TopicTraffic
TableDefinition kTopicTraffic("TopicTraffic", {DatabaseColumnInfo{"Id"},
                                               {"Timestamp_ms", "%lu", true},
                                               {"Topic", "%s", true},
                                               {"Ip", "%s", true},
                                               {"Mode", "%i", true}});
static const std::string kCreateTopicTraffic = getCreateTableStatement(kTopicTraffic);
static bool topic_traffic_exists_ = false;

void NatterLoggerNs3::logReceive(const std::string &topic, const std::string &ip) {
  if (!topic_traffic_exists_) {
    log_(kCreateTopicTraffic);
    topic_traffic_exists_ = true;
  }

  auto id = topic_ids_.at(topic);
  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Topic */ id,
      /* Ip */ ip.c_str(),
      /* Mode */ 0);
  log_(getInsertStatement(kTopicTraffic, t));
}

void NatterLoggerNs3::logSend(const std::string &topic, const std::string &ip) {
  if (!topic_traffic_exists_) {
    log_(kCreateTopicTraffic);
    topic_traffic_exists_ = true;
  }

  auto id = topic_ids_.at(topic);

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Topic */ id,
      /* Ip */ ip.c_str(),
      /* Mode */ 1);
  log_(getInsertStatement(kTopicTraffic, t));
}

// * Constructor & Other methods
NatterLoggerNs3::NatterLoggerNs3(LogDeviceApp log_device_application, LogFunction log,
                                 LogEvent log_event)
    : LoggerInterface("NOT-KNOWN-YET"),
      log_device_application_(log_device_application),
      log_(std::move(log)),
      log_event_(std::move(log_event)) {}

NatterLoggerNs3::~NatterLoggerNs3() {
  auto current_time = ns3::Simulator::Now().GetMilliSeconds();
  log_(toSQL("UPDATE DeviceApplication SET StopTime_ms=%lu WHERE ApplicationUuid='%s';",
             current_time, uuid_.c_str()));
}

void NatterLoggerNs3::logCritical(const std::string &msg) const {}
void NatterLoggerNs3::logWarning(const std::string &msg) const {}
void NatterLoggerNs3::logInfo(const std::string &msg) const {}
void NatterLoggerNs3::logDebug(const std::string &msg) const {}

void NatterLoggerNs3::logSendFullMsg(solanet::UUID msg_uuid, solanet::UUID uuid,
                                     solanet::UUID own_uuid) {
  logSendReceive(msg_uuid, own_uuid, uuid, MsgType::kFullMsg, Mode::kSend);
}

void NatterLoggerNs3::logReceiveFullMsg(solanet::UUID msg_uuid, solanet::UUID sender,
                                        solanet::UUID own_uuid) {
  logSendReceive(msg_uuid, sender, own_uuid, MsgType::kFullMsg, Mode::kReceive);
}

}  // namespace natter::logging

#undef TableDefinition
#undef ViewDefinition
