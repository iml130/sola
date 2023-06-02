// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "logger.h"

#include <algorithm>
#include <utility>

namespace natter::logging {
void Logger::addLogger(std::shared_ptr<LoggerInterface> logger) {
  auto it = std::find_if(logger_list_.begin(), logger_list_.end(),
                         [&](auto logger_) { return logger_.get() == logger.get(); });
  if (it == logger_list_.end()) logger_list_.push_back(std::move(logger));
}

void Logger::logCritical(const std::string &msg) const {
  if (log_level_ <= LogLevel::kCritical)
    log([&](const LoggerPtr &logger) { logger->logCritical(msg); });
}

void Logger::logWarning(const std::string &msg) const {
  if (log_level_ <= LogLevel::kWarning)
    log([&](const LoggerPtr &logger) { logger->logWarning(msg); });
}

void Logger::logInfo(const std::string &msg) const {
  if (log_level_ <= LogLevel::kInfo) log([&](const LoggerPtr &logger) { logger->logInfo(msg); });
}

void Logger::logDebug(const std::string &msg) const {
  if (log_level_ <= LogLevel::kDebug) log([&](const LoggerPtr &logger) { logger->logDebug(msg); });
}

void Logger::logNewPeer(const std::string &ip, uint16_t port, UUID uuid,
                        const std::string &topic) const {
  if (log_strings_)
    log([&](const LoggerPtr &logger) { logger->logNewPeer(ip, port, uuid, topic); });
}

void Logger::logRemovePeer(const std::string &ip, uint16_t port, UUID uuid,
                           const std::string &topic) const {
  if (log_strings_)
    log([&](const LoggerPtr &logger) { logger->logRemovePeer(ip, port, uuid, topic); });
}

void Logger::logNewMessage(const std::string &topic, const std::string &msg, UUID msg_uuid) {
  if (log_strings_)
    log([&](const LoggerPtr &logger) { logger->logNewMessage(topic, msg, msg_uuid); });
}

void Logger::logSendFullMsg(UUID msg_uuid, UUID uuid, UUID own_uuid) {
  if (log_strings_)
    log([&](const LoggerPtr &logger) { logger->logSendFullMsg(msg_uuid, uuid, own_uuid); });
}

void Logger::logReceiveFullMsg(UUID msg_uuid, UUID sender, UUID own_uuid) {
  if (log_strings_)
    log([&](const LoggerPtr &logger) { logger->logReceiveFullMsg(msg_uuid, sender, own_uuid); });
}

void Logger::logMinhcastBroadcast(UUID msg_id, uint32_t level, uint32_t number,
                                  uint32_t forward_up_limit, uint32_t forward_down_limit) {
  if (log_strings_)
    log([&](const LoggerPtr &logger) {
      logger->logMinhcastBroadcast(msg_id, level, number, forward_up_limit, forward_down_limit);
    });
}

void Logger::logNewNetworkPeer(UUID uuid, const std::string &ip, uint16_t port, int level,
                               int number) {
  if (log_strings_)
    log([&](const LoggerPtr &logger) { logger->logNewNetworkPeer(uuid, ip, port, level, number); });
}

void Logger::logReceivedMessages(UUID node_uuid, UUID initial_sender, UUID message,
                                 uint32_t round) {
  if (log_strings_)
    log([&](const LoggerPtr &logger) {
      logger->logReceivedMessages(node_uuid, initial_sender, message, round);
    });
}

void Logger::log(const std::function<void(const LoggerPtr &)> &function) const {
  for (const auto &logger : logger_list_)
    function(logger);
}

LogLevel Logger::getLogLevel() const { return log_level_; }

void Logger::setLogLevel(LogLevel logLevel) { this->log_level_ = logLevel; }

bool Logger::isLoggingStrings() const { return log_strings_; }

void Logger::setLogggingStrings(bool log_strings) { log_strings_ = log_strings; }
}  // namespace natter::logging
