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

#ifndef DAISI_LOGGING_LOGGER_MANAGER_NS3_H_
#define DAISI_LOGGING_LOGGER_MANAGER_NS3_H_

#include <cstdint>
#include <memory>
#include <string>

#include "cpps/common/cpps_logger_ns3.h"
#include "definitions.h"
#include "minhton-ns3/minhton_logger_ns3.h"
#include "natter-ns3/natter_logger_ns3.h"
// #include "path_planning/path_planning_logger_ns_3.h"
#include "sola-ns3/sola_logger_ns3.h"
#include "sqlite/sqlite_helper.h"

namespace daisi {

class LoggerManager;

// Global logger manager that is initialized from Manager
inline std::unique_ptr<LoggerManager> global_logger_manager;

struct LoggerInfoTestSetup {
  uint64_t number_of_nodes;
  uint32_t fanout;
  std::string message;
};

class LoggerManager {
public:
  LoggerManager(const std::string &path, const std::string &name);
  ~LoggerManager();

  void setFailed(const char *exception);
  void logTestSetup(const LoggerInfoTestSetup &info);
  void updateTestSetupTime();
  void updateTestSetupEventCount(const uint64_t &count);

  void logDevice(uint32_t id);
  void logDeviceApplication(const std::string &application_uuid,
                            const std::string &application_name, uint32_t device_id);
  void logEvent(const std::string &event_uuid, uint16_t event_type,
                const std::string &application_uuid);

  void logMinhtonConfigFile(const std::string &file_path);

  std::shared_ptr<minhton::MinhtonLoggerNs3> createMinhtonLogger(
      uint32_t device_id, const std::string &app_name_postfix = "");
  std::shared_ptr<natter::logging::NatterLoggerNs3> createNatterLogger(uint32_t device_id);
  std::shared_ptr<daisi::cpps::CppsLoggerNs3> createAMRLogger(uint32_t device_id);
  std::shared_ptr<daisi::cpps::CppsLoggerNs3> createTOLogger(uint32_t device_id);
  // std::shared_ptr<daisi::path_planning::PathPlanningLoggerNs3> createPathPlanningLogger(
  //     uint32_t device_id, const std::string &device_type = "");
  std::shared_ptr<sola_ns3::SolaLoggerNs3> createSolaLogger(uint32_t device_id);

private:
  daisi::SQLiteHelper sqlite_helper_;
  const uint16_t minhton_event_type_base_ = 0;
  const uint16_t natter_event_type_base_ = 256;
  bool minhton_exists_ = false;
  bool natter_exists_ = false;
};
}  // namespace daisi

#endif
