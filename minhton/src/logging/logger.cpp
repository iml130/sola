// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/logging/logger.h"

namespace minhton {

void Logger::addLogger(LoggerPtr logger) {
  auto it = std::find_if(logger_list_.begin(), logger_list_.end(),
                         [&](auto logger_) { return logger_.get() == logger.get(); });
  if (it == logger_list_.end()) logger_list_.push_back(std::move(logger));
}

void Logger::logCritical(const std::string &msg) const {
  if (log_level_ >= LogLevel::kCritical) {
    logCommand([&](const LoggerPtr &logger) { logger->logCritical(msg); });
  }
}

void Logger::logWarning(const std::string &msg) const {
  if (log_level_ >= LogLevel::kWarning) {
    logCommand([&](const LoggerPtr &logger) { logger->logWarning(msg); });
  }
}

void Logger::logInfo(const std::string &msg) const {
  if (log_level_ >= LogLevel::kInfo) {
    logCommand([&](const LoggerPtr &logger) { logger->logInfo(msg); });
  }
}

void Logger::logDebug(const std::string &msg) const {
  if (log_level_ >= LogLevel::kDebug) {
    logCommand([&](const LoggerPtr &logger) { logger->logDebug(msg); });
  }
}

void Logger::logPhysicalNodeInfo(const LoggerPhysicalNodeInfo &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logPhysicalNodeInfo(info); });
}

void Logger::logNode(const LoggerInfoAddNode &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logNode(info); });
}

void Logger::logNodeUninit(const LoggerInfoNodeState &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logNodeUninit(info); });
}

void Logger::logNodeRunning(const LoggerInfoNodeState &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logNodeRunning(info); });
}

void Logger::logNodeLeft(const LoggerInfoNodeState &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logNodeLeft(info); });
}

void Logger::logNeighbor(const LoggerInfoAddNeighbor &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logNeighbor(info); });
}

void Logger::logEvent(const LoggerInfoAddEvent &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logEvent(info); });
}

void Logger::logSearchExactTest(const LoggerInfoSearchExact &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logSearchExactTest(info); });
}

void Logger::logTraffic(const MessageLoggingInfo &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logTraffic(info); });
}

void Logger::logContent(const LoggerInfoAddContent &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logContent(info); });
}

void Logger::logFindQuery(const LoggerInfoAddFindQuery &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logFindQuery(info); });
}

void Logger::logFindQueryResult(const LoggerInfoAddFindQueryResult &info) {
  logCommand([&](const LoggerPtr &logger) { logger->logFindQueryResult(info); });
}

void Logger::logCommand(const std::function<void(const LoggerPtr &)> &function) const {
  for (const auto &logger : logger_list_) {
    function(logger);
  }
}

LogLevel Logger::getLogLevel() const { return log_level_; }

void Logger::setLogLevel(LogLevel logLevel) { this->log_level_ = logLevel; }

LogLevel Logger::logLevelFromString(const std::string &level) {
  if (level == "debug") {
    return LogLevel::kDebug;
  }
  if (level == "info") {
    return LogLevel::kInfo;
  }
  if (level == "warning") {
    return LogLevel::kWarning;
  }
  if (level == "critical") {
    return LogLevel::kCritical;
  }
  return LogLevel::kDebug;
}

}  // namespace minhton
