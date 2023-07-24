// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_LOGGER_INTERFACE_H_
#define NATTER_LOGGER_INTERFACE_H_

#include <memory>
#include <string>
#include <vector>

#include "solanet/uuid.h"

namespace natter::logging {

class LoggerInterface;

using LoggerPtr = std::shared_ptr<LoggerInterface>;

/**
 * Interface for loggers. Inherit from this to implement your own logger.
 */
class LoggerInterface {
public:
  LoggerInterface(std::string uuid) : uuid_(std::move(uuid)) {}
  virtual ~LoggerInterface() = default;

  /**
   * Log critical message
   * @param msg message
   */
  virtual void logCritical(const std::string &msg) const = 0;

  /**
   * Log warning message
   * @param msg message
   */
  virtual void logWarning(const std::string &msg) const = 0;

  /**
   * Log info message
   * @param msg message
   */
  virtual void logInfo(const std::string &msg) const = 0;

  /**
   * Log debug message
   * @param msg message
   */
  virtual void logDebug(const std::string &msg) const = 0;

  /**
   * Log new peer-topic connection
   * @param ip ip of new peer
   * @param port port of new peer
   * @param uuid uuid of new peer
   * @param topic topic the peer was added to
   */
  virtual void logNewPeer(const std::string &ip, uint16_t port, solanet::UUID uuid,
                          const std::string &topic) const = 0;

  /**
   * Log removal of peer
   * @param ip ip of removed peer
   * @param port port of removed peer
   * @param uuid uuid of removed peer
   * @param topic topic the peer was removed from
   */
  virtual void logRemovePeer(const std::string &ip, uint16_t port, solanet::UUID uuid,
                             const std::string &topic) const = 0;

  /**
   * Log receive of application message
   * @param msg message content
   * @param msg_uuid message uuid
   */
  virtual void logNewMessage(const std::string &topic, const std::string &msg,
                             solanet::UUID msg_uuid) = 0;

  /**
   * Log sending full message
   * @param msg_uuid message uuid
   * @param uuid target node uuid
   * @param own_uuid own node uuid
   */
  virtual void logSendFullMsg(solanet::UUID msg_uuid, solanet::UUID uuid,
                              solanet::UUID own_uuid) = 0;

  /**
   * Log receiving full message
   * @param msg_uuid message uuid
   * @param sender sender node uuid
   * @param own_uuid own node uuid
   */
  virtual void logReceiveFullMsg(solanet::UUID msg_uuid, solanet::UUID sender,
                                 solanet::UUID own_uuid) = 0;

  virtual void logMinhcastBroadcast(solanet::UUID msg_id, uint32_t level, uint32_t number,
                                    uint32_t forward_up_limit, uint32_t forward_down_limit) = 0;

  virtual void logNewNetworkPeer(solanet::UUID uuid, const std::string &ip, uint16_t port,
                                 int level, int number) = 0;

  virtual void logReceivedMessages(solanet::UUID node_uuid, solanet::UUID initial_sender,
                                   solanet::UUID message, uint32_t round) = 0;

  /**
   * Set the UUID used for identifying application nodes.
   * Used for loggers which are initialized before node starts.
   * @param app_uuid application uuid
   */
  virtual void setApplicationUUID(const solanet::UUID &app_uuid) = 0;

protected:
  std::string uuid_;
};
}  // namespace natter::logging

#endif  // NATTER_LOGGER_INTERFACE_H_
