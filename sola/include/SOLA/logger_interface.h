// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_LOGGER_INTERFACE_H_
#define SOLA_LOGGER_INTERFACE_H_

#include <memory>
#include <string>

#include "SOLA/message.h"

namespace sola {

class LoggerInterface;

using LoggerPtr = std::shared_ptr<LoggerInterface>;

/**
 * Interface for loggers. Inherit from this to implement your own logger.
 */
class LoggerInterface {
public:
  explicit LoggerInterface(std::string uuid) : uuid_(std::move(uuid)) {}
  virtual ~LoggerInterface() = default;

  virtual void logSubscribeTopic(const std::string &topic) const = 0;
  virtual void logUnsubscribeTopic(const std::string &topic) const = 0;

  virtual void logPublishTopicMessage(const sola::TopicMessage &msg) const = 0;
  virtual void logReceiveTopicMessage(const sola::TopicMessage &msg) const = 0;

  /// Log mapping between SOLA message ID and message ID of underlying event dissemination.
  /// Might not be required for all event dissemination implementations.
  virtual void logMessageIDMapping(const solanet::UUID &sola_msg_uuid,
                                   const solanet::UUID &ed_msg_uuid) const = 0;

  /**
   * Set the UUID used for identifying application nodes.
   * Used for loggers which are initialized before node starts.
   * @param app_uuid application uuid
   */
  virtual void setApplicationUUID(const std::string &app_uuid) = 0;

protected:
  std::string uuid_;
};
}  // namespace sola

#endif  // SOLA_LOGGER_INTERFACE_H_
