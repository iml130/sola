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
  NatterLoggerNs3(LogDeviceApp log_device_application, LogFunction log, LogEvent log_event);

  // natter-ns3 specific logging functions
  void logNewNetworkPeer(solanet::UUID uuid, const std::string &ip, uint16_t port, int level,
                         int number) final;

  void logNs3PeerConnection(uint64_t timestamp, bool active, solanet::UUID node_uuid,
                            solanet::UUID new_node_uuid);

  void logReceivedMessages(solanet::UUID node_uuid, solanet::UUID initial_sender,
                           solanet::UUID message, uint32_t round_) final;

  void logNatterEvent(uint16_t event_type, solanet::UUID event_id);

  // TODO Refactor to other class
  void setApplicationUUID(const solanet::UUID &app_uuid) final {
    LoggerInterface::uuid_ = solanet::uuidToString(app_uuid);
    log_device_application_(uuid_);
  }

  ~NatterLoggerNs3() override;

private:
  enum class Mode : uint16_t { kReceive, kSend };

  enum class MsgType : uint16_t { kUnknown, kFullMsg };

  void logCritical(const std::string &msg) const final;

  void logWarning(const std::string &msg) const final;

  void logInfo(const std::string &msg) const final;

  void logDebug(const std::string &msg) const final;

  void logNewPeer(const std::string &ip, uint16_t port, solanet::UUID uuid,
                  const std::string &topic) const final;

  void logRemovePeer(const std::string &ip, uint16_t port, solanet::UUID uuid,
                     const std::string &topic) const final;

  void logNewMessage(const std::string &topic, const std::string &msg,
                     solanet::UUID msg_uuid) final;

  void logSendFullMsg(solanet::UUID msg_uuid, solanet::UUID uuid, solanet::UUID own_uuid) final;

  void logReceiveFullMsg(solanet::UUID msg_uuid, solanet::UUID sender,
                         solanet::UUID own_uuid) final;

  void logSendReceive(solanet::UUID msg_uuid, solanet::UUID sender, solanet::UUID own_uuid,
                      MsgType type, Mode mode);

  void logMinhcastBroadcast(solanet::UUID msg_id, uint32_t level, uint32_t number,
                            uint32_t forward_up_limit, uint32_t forward_down_limit) final;

  // TODO Refactor to other class
  LogDeviceApp log_device_application_;
  LogFunction log_;
  LogEvent log_event_;
};

}  // namespace natter::logging

#endif  // NATTER_NS3_LOGGER_H_
