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

#ifndef DAISI_MINHTON_NS3_MINHTON_LOGGER_NS3_H_
#define DAISI_MINHTON_NS3_MINHTON_LOGGER_NS3_H_

#include <functional>
#include <string>

#include "logging/definitions.h"
#include "minhton/core/node_info.h"
#include "minhton/logging/logger_interface.h"
#include "minhton/message/message.h"
#include "solanet/uuid.h"

namespace minhton {

class MinhtonLoggerNs3 : public LoggerInterface {
public:
  MinhtonLoggerNs3(LogDeviceApp log_device_application, LogFunction log, LogEvent log_event);
  ~MinhtonLoggerNs3() override;

  void logCritical(const std::string &msg) const final;
  void logWarning(const std::string &msg) const final;
  void logInfo(const std::string &msg) const final;
  void logDebug(const std::string &msg) const final;

  void logNodeUninit(const LoggerInfoNodeState &info) final;
  void logNodeRunning(const LoggerInfoNodeState &info) final;
  void logNodeLeft(const LoggerInfoNodeState &info) final;
  void logPhysicalNodeInfo(const LoggerPhysicalNodeInfo &info) final;
  void logNode(const LoggerInfoAddNode &info) final;
  void logNeighbor(const LoggerInfoAddNeighbor &info) final;
  void logEvent(const LoggerInfoAddEvent &info) final;
  void logSearchExactTest(const LoggerInfoSearchExact &info) final;
  void logTraffic(const MessageLoggingInfo &info) final;
  void logContent(const LoggerInfoAddContent &info) final;
  void logFindQuery(const LoggerInfoAddFindQuery &info) final;
  void logFindQueryResult(const LoggerInfoAddFindQueryResult &info) final;

  // TODO Refactor to other class
  void setApplicationUUID(const solanet::UUID &app_uuid) final {
    LoggerInterface::uuid_ = solanet::uuidToString(app_uuid);
    log_device_application_(uuid_);
  }

private:
  // TODO Refactor to other class
  LogDeviceApp log_device_application_;
  LogFunction log_;
  LogEvent log_event_;

  void logMinhtonMessageTypes();
  void logMinhtonNodeStates();
  void logMinhtonRelationships();
};

}  // namespace minhton

#endif  // MINHTON_NS3_LOGGER_H_
