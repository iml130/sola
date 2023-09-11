// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_LOGGING_LOGGER_INTERFACE_H_
#define MINHTON_LOGGING_LOGGER_INTERFACE_H_

#include <string>
#include <vector>

#include "minhton/message/message_logging.h"
#include "solanet/uuid.h"

namespace minhton {

struct LoggerInfoSearchExact {
  uint64_t timestamp;
  uint64_t event_id;
  uint16_t status;
  uint32_t sender_level;
  uint32_t sender_number;
  uint32_t target_level;
  uint32_t target_number;
  uint32_t hop_level;
  uint32_t hop_number;
};

struct LoggerInfoAddNode {
  std::string position_uuid;
  uint32_t level;
  uint32_t number;
  uint16_t fanout;
  bool initialized;
};

struct LoggerInfoAddNeighbor {
  uint64_t event_id;
  std::string node_uuid;
  std::string neighbor_node_uuid;
  uint16_t relationship;
};

struct LoggerInfoAddEvent {
  uint64_t timestamp;
  uint8_t event_type;
  uint64_t event_id;
};

struct LoggerInfoNodeState {
  std::string position_uuid;
  uint64_t event_id;
};

struct LoggerInfoAddContent {
  uint64_t timestamp;
  std::string node_uuid;
  uint8_t content_status;
  std::string attribute_name;
  uint8_t content_type;
  std::string content_text;
};

struct LoggerPhysicalNodeInfo {
  std::string ip;
  uint16_t port;
};

struct LoggerInfoAddFindQuery {
  uint64_t timestamp;
  uint64_t event_id;
  std::string node_uuid;
  bool inquire_unknown;
  bool inquire_outdated;
  std::string query;
};

struct LoggerInfoAddFindQueryResult {
  uint64_t timestamp;
  uint64_t event_id;
  std::string node_uuid;
};

class LoggerInterface {
public:
  LoggerInterface(std::string uuid) : uuid_(std::move(uuid)) {}
  virtual ~LoggerInterface() = default;

  virtual void logCritical(const std::string &msg) const = 0;
  virtual void logWarning(const std::string &msg) const = 0;
  virtual void logInfo(const std::string &msg) const = 0;
  virtual void logDebug(const std::string &msg) const = 0;

  // Used for loggers which are initialized before node starts
  virtual void setApplicationUUID(const solanet::UUID &app_uuid) = 0;

  virtual void logNodeUninit(const LoggerInfoNodeState &info) = 0;
  virtual void logNodeRunning(const LoggerInfoNodeState &info) = 0;
  virtual void logNodeLeft(const LoggerInfoNodeState &info) = 0;
  virtual void logPhysicalNodeInfo(const LoggerPhysicalNodeInfo &info) = 0;
  virtual void logNode(const LoggerInfoAddNode &info) = 0;
  virtual void logNeighbor(const LoggerInfoAddNeighbor &info) = 0;
  virtual void logEvent(const LoggerInfoAddEvent &info) = 0;
  virtual void logSearchExactTest(const LoggerInfoSearchExact &info) = 0;
  virtual void logTraffic(const MessageLoggingInfo &info) = 0;
  virtual void logContent(const LoggerInfoAddContent &info) = 0;
  virtual void logFindQuery(const LoggerInfoAddFindQuery &info) = 0;
  virtual void logFindQueryResult(const LoggerInfoAddFindQueryResult &info) = 0;

protected:
  std::string uuid_;
};

}  // namespace minhton

#endif
