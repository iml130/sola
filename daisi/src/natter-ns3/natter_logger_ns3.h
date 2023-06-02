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

#ifndef DAISI_NATTER_NS3_NATTER_LOGGER_NS3_H_
#define DAISI_NATTER_NS3_NATTER_LOGGER_NS3_H_

#include <ctime>
#include <unordered_map>

#include "logging/definitions.h"
#include "natter/logger_interface.h"
#include "natter_mode.h"

namespace natter::logging {

class NatterLoggerNs3 : public LoggerInterface {
public:
  NatterLoggerNs3(LogDeviceApp log_device_application, LogFunction log, const LogEvent &log_event);

  // natter-ns3 specific logging functions
  void logNewNetworkPeer(UUID uuid, const std::string &ip, uint16_t port, int level,
                         int number) final;

  void logNs3PeerConnection(uint64_t timestamp, bool active, UUID node_uuid, UUID new_node_uuid);

  void logReceivedMessages(UUID node_uuid, UUID initial_sender, UUID message,
                           uint32_t round_) final;

  void logNatterEvent(uint16_t event_type, UUID event_id);

  // TODO Refactor to other class
  void setApplicationUUID(const UUID &app_uuid) final {
    LoggerInterface::uuid_ = uuidToString(app_uuid);
    log_device_application_(uuid_);
  }

  ~NatterLoggerNs3();

private:
  enum class Mode : uint16_t { kReceive, kSend };

  enum class MsgType : uint16_t { kUnknown, kFullMsg };

  void logCritical(const std::string &msg) const final;

  void logWarning(const std::string &msg) const final;

  void logInfo(const std::string &msg) const final;

  void logDebug(const std::string &msg) const final;

  void logNewPeer(const std::string &ip, uint16_t port, UUID uuid,
                  const std::string &topic) const final;

  void logRemovePeer(const std::string &ip, uint16_t port, UUID uuid,
                     const std::string &topic) const final;

  void logNewMessage(const std::string &topic, const std::string &msg, UUID msg_uuid) final;

  void logSendFullMsg(UUID msg_uuid, UUID uuid, UUID own_uuid) final;

  void logReceiveFullMsg(UUID msg_uuid, UUID sender, UUID own_uuid) final;

  void logSendReceive(UUID msg_uuid, UUID sender, UUID own_uuid, MsgType type, Mode mode);

  [[maybe_unused]] void logReceive(const std::string &topic, const std::string &ip);

  [[maybe_unused]] void logSend(const std::string &topic, const std::string &ip);

  void logMinhcastBroadcast(UUID msg_id, uint32_t level, uint32_t number, uint32_t forward_up_limit,
                            uint32_t forward_down_limit) final;

  // TODO Refactor to other class
  LogDeviceApp log_device_application_;
  LogFunction log_;
  LogEvent log_event_;

  const std::unordered_map<std::string, uint8_t> topic_ids_ = {{"topic30eurobox", 1},
                                                               {"topic60eurobox", 2},
                                                               {"topic30package", 3},
                                                               {"topic45package", 4},
                                                               {"defaulttopic", 5}};
};

}  // namespace natter::logging

#endif  // NATTER_NS3_LOGGER_H_
