// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_LOGGER_H_
#define NATTER_LOGGER_H_

#include <cstdint>
#include <functional>
#include <vector>

#include "natter/logger_interface.h"
#include "natter/uuid.h"

namespace natter::logging {
/**
 * Enum class of all available log levels
 */
enum class LogLevel { kDebug, kInfo, kWarning, kCritical };

/**
 * Class holding a list of loggers to be called for logging
 */
class Logger {
public:
  /**
   * Add logger and use it for upcoming log-function calls
   * @param logger logger to add
   */
  void addLogger(LoggerPtr logger);

  /**
   * Log critical message
   * @param msg message to log
   */
  void logCritical(const std::string &msg) const;

  /**
   * Log warning message
   * @param msg message to log
   */
  void logWarning(const std::string &msg) const;

  /**
   * Log info message
   * @param msg message to log
   */
  void logInfo(const std::string &msg) const;

  /**
   * Log debug message
   * @param msg message to log
   */
  void logDebug(const std::string &msg) const;

  /**
   * Log new peer-topic connection
   * @param ip ip of new peer
   * @param port port of new peer
   * @param uuid uuid of new peer
   * @param topic topic the peer was added to
   */
  void logNewPeer(const std::string &ip, uint16_t port, UUID uuid, const std::string &topic) const;

  /**
   * Log removal of peer
   * @param ip ip of removed peer
   * @param port port of removed peer
   * @param uuid uuid of removed peer
   * @param topic topic the peer was removed from
   */
  void logRemovePeer(const std::string &ip, uint16_t port, UUID uuid,
                     const std::string &topic) const;

  /**
   * Log receive of application message
   * @param topic topic of message
   * @param msg message content
   * @param msg_uuid message uuid
   */
  void logNewMessage(const std::string &topic, const std::string &msg, UUID msg_uuid);

  /**
   * Log sending full message
   * @param msg_uuid message uuid
   * @param uuid target node uuid
   * @param own_uuid own node uuid
   */
  void logSendFullMsg(UUID msg_uuid, UUID uuid, UUID own_uuid);

  /**
   * Log receiving full message
   * @param msg_uuid message uuid
   * @param sender sender node uuid
   * @param own_uuid own node uuid
   */
  void logReceiveFullMsg(UUID msg_uuid, UUID sender, UUID own_uuid);

  void logMinhcastBroadcast(UUID msg_id, uint32_t level, uint32_t number, uint32_t forward_up_limit,
                            uint32_t forward_down_limit);

  void logNewNetworkPeer(UUID uuid, const std::string &ip, uint16_t port, int level, int number);
  void logReceivedMessages(UUID node_uuid, UUID initial_sender, UUID message, uint32_t round);

  /**
   * Returns current log level
   * @return current log level
   */
  LogLevel getLogLevel() const;

  /**
   * Sets \p logLevel as current log level
   * @param logLevel new log level
   */
  void setLogLevel(LogLevel logLevel);

  /**
   * Checks if logging string messages is enabled
   * @return true if basic logging is activated
   */
  bool isLoggingStrings() const;

  /**
   * Sets if string messages should be logged
   * @param log_strings true if string messages should be logged
   */
  void setLogggingStrings(bool log_strings);

private:
  /**
   * Calls \p_function for every registered logger (command-pattern)
   * @param function function to call for every logger
   */
  void log(const std::function<void(const LoggerPtr &logger)> &function) const;

  std::vector<LoggerPtr> logger_list_;

  LogLevel log_level_ = LogLevel::kWarning;

  bool log_strings_ = true;
};
}  // namespace natter::logging

#endif  // NATTER_LOGGER_H_
