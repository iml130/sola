// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_LOGGING_LOGGER_H_
#define MINHTON_LOGGING_LOGGER_H_

#include <functional>
#include <memory>
#include <vector>

#include "minhton/logging/logger_interface.h"

namespace minhton {

enum class LogLevel { kDebug, kInfo, kWarning, kCritical };

class Logger {
public:
  using LoggerPtr = std::shared_ptr<LoggerInterface>;

  void addLogger(LoggerPtr logger);

  void logCritical(const std::string &msg) const;
  void logWarning(const std::string &msg) const;
  void logInfo(const std::string &msg) const;
  void logDebug(const std::string &msg) const;

  void logNodeUninit(const LoggerInfoNodeState &info);
  void logNodeRunning(const LoggerInfoNodeState &info);
  void logNodeLeft(const LoggerInfoNodeState &info);
  void logPhysicalNodeInfo(const LoggerPhysicalNodeInfo &info);
  void logNode(const LoggerInfoAddNode &info);
  void logNeighbor(const LoggerInfoAddNeighbor &info);
  void logEvent(const LoggerInfoAddEvent &info);
  void logSearchExactTest(const LoggerInfoSearchExact &info);
  void logTraffic(const MessageLoggingInfo &info);
  void logContent(const LoggerInfoAddContent &info);
  void logFindQuery(const LoggerInfoAddFindQuery &info);
  void logFindQueryResult(const LoggerInfoAddFindQueryResult &info);

  LogLevel getLogLevel() const;

  void setLogLevel(LogLevel logLevel);

  static LogLevel logLevelFromString(const std::string &level);

private:
  // Command pattern
  template <typename LogFunction> void logCommand(LogFunction function) const;

  std::vector<LoggerPtr> logger_list_;

  LogLevel log_level_ = LogLevel::kDebug;
};

}  // namespace minhton

#endif
