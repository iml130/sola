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

#include "logger_manager.h"

#include <ctime>
#include <fstream>
#include <set>

#include "ns3/simulator.h"

#define TableDefinition static const DatabaseTable
#define ViewDefinition static const std::unordered_map<std::string, std::string>

namespace daisi {

// * Device
TableDefinition kDevice("Device", {{"Id", "%lu", true, "", true}});
const std::string kCreateDevice = getCreateTableStatement(kDevice);

void LoggerManager::logDevice(uint32_t id) {
  static std::set<uint32_t> cached_ids;
  const auto res = cached_ids.insert(id);
  const bool inserted = std::get<1>(res);
  if (inserted) {
    auto t = std::make_tuple(/* Id */ id);
    sqlite_helper_.execute(getInsertStatement(kDevice, t));
  }
}

// * DeviceApplication
TableDefinition kDeviceApplication("DeviceApplication",
                                   {DatabaseColumnInfo{"ApplicationUuid", "%s", true, "", true},
                                    {"ApplicationName", "%s", true},
                                    {"DeviceUuid", "%lu", true},
                                    {"StartTime_ms", "%lu", true},
                                    {"StopTime_ms", "%lu"}});
const std::string kCreateDeviceApplication = getCreateTableStatement(kDeviceApplication);

void LoggerManager::logDeviceApplication(const std::string &application_uuid,
                                         const std::string &application_name, uint32_t device_id) {
  logDevice(device_id);
  auto t = std::make_tuple(/* ApplicationUuid */ application_uuid.c_str(),
                           /* ApplicationName */ application_name.c_str(),
                           /* DeviceUuid */ device_id,
                           /* StartTime_ms */ ns3::Simulator::Now().GetMilliSeconds());
  auto table = kDeviceApplication;
  table.columns[4] = {"StopTime_ms", "NULL"};
  sqlite_helper_.execute(getInsertStatement(table, t));
}

// * Event
TableDefinition kEvent("Event",
                       {DatabaseColumnInfo{"Id"},
                        {"Uuid", "%s", true},
                        {"Timestamp_ms", "%lu", true},
                        {"Type", "%u", true},
                        {"ApplicationUuid", "%s", true, "DeviceApplication(ApplicationUuid)"}});
const std::string kCreateEvent = getCreateTableStatement(kEvent);

void LoggerManager::logEvent(const std::string &event_uuid, uint16_t event_type,
                             const std::string &application_uuid) {
  auto t = std::make_tuple(/* Uuid */ event_uuid.c_str(),
                           /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
                           /* Type */ event_type,
                           /* ApplicationUuid */ application_uuid.c_str());
  sqlite_helper_.execute(getInsertStatement(kEvent, t));
}

// * General
TableDefinition kGeneral("General", {{"StartTime_ut", "%lu", true},
                                     {"StopTime_ut", "%lu", true},
                                     {"NumberOfNodes", "%lu", true},
                                     {"NumberOfEvents", "%lu", true},
                                     {"Fanout", "%u"},
                                     {"Exception", "%s"},
                                     {"Config", "%s"}});
const std::string kCreateGeneral = getCreateTableStatement(kGeneral);
std::vector<std::string> general_updates_;

void LoggerManager::setFailed(const char *exception) {
  sqlite_helper_.execute(toSQL("UPDATE General SET Exception='%s';", exception));
  sqlite_helper_.setFailed();
}

void LoggerManager::logTestSetup(const LoggerInfoTestSetup &info) {
  auto time_start = std::time(nullptr);

  auto t = std::make_tuple(
      /* StartTime_ut */ time_start,
      /* StopTime_ut */ 0,
      /* NumberOfNodes */ info.number_of_nodes,
      /* NumberOfEvents */ 0,
      /* Fanout */ info.fanout,
      /* Exception */ "",
      /* Config */ info.message.c_str());
  sqlite_helper_.execute(getInsertStatement(kGeneral, t));

  for (auto general_update : general_updates_) {
    sqlite_helper_.execute(general_update);
  }
  general_updates_.clear();
}

void LoggerManager::updateTestSetupTime() {
  auto current_time = std::time(nullptr);
  sqlite_helper_.execute(toSQL("UPDATE General SET StopTime_ut=%lu;", current_time));
}

void LoggerManager::updateTestSetupEventCount(const uint64_t &count) {
  sqlite_helper_.execute(toSQL("UPDATE General SET NumberOfEvents=%lu;", count));
}

static std::string readMinhtonConfigFileContent(std::string path) {
  char *minhton_path_ptr = std::getenv("MINHTONDIR");
  if (minhton_path_ptr == nullptr)
    throw std::runtime_error("MINHTONDIR environment variable not specified!");
  std::string minhton_path(minhton_path_ptr);

  if (path.front() == '/') {
    path.erase(0);
  }

  if (minhton_path.back() != '/') {
    minhton_path.push_back('/');
  }

  std::string abs_path = minhton_path + path;

  // https://stackoverflow.com/a/2602258
  std::ifstream file(abs_path);
  std::stringstream stream;
  stream << file.rdbuf();
  return stream.str();
}

void LoggerManager::logMinhtonConfigFile(const std::string &file_path) {
  std::string content = readMinhtonConfigFileContent(file_path);
  general_updates_.push_back(
      toSQL("UPDATE General SET Config=Config||'\n\n### %s ###\n%s\n### end of file ###';",
            file_path.c_str(), content.c_str()));
}

// * Constructor & Other methods
LoggerManager::LoggerManager(const std::string &path, const std::string &name)
    : sqlite_helper_(path, name) {
  sqlite_helper_.execute(kCreateDevice);
  sqlite_helper_.execute(kCreateDeviceApplication);
  sqlite_helper_.execute(kCreateGeneral);
  sqlite_helper_.execute(kCreateEvent);
}

LoggerManager::~LoggerManager() {
  // Create combined view for nodes
  if (minhton_exists_ && natter_exists_) {
    sqlite_helper_.execute(
        "CREATE VIEW IF NOT EXISTS viewNode AS SELECT Level, Number, Ip, Port, Timestamp_ms, "
        "'Minhton' AS Type FROM viewMinhtonNodeState WHERE State = 'RUNNING' UNION SELECT Level, "
        "Number, Ip, Port, Timestamp_ms, 'Natter' AS Type FROM NatterNode ORDER BY Timestamp_ms;");
  } else if (minhton_exists_) {
    sqlite_helper_.execute("CREATE VIEW IF NOT EXISTS viewNode AS SELECT Level, Number, Ip, Port, "
                           "Timestamp_ms, 'Minhton' AS Type FROM viewMinhtonNodeState WHERE State "
                           "= 'RUNNING' ORDER BY Timestamp_ms;");
  } else if (natter_exists_) {
    sqlite_helper_.execute("CREATE VIEW IF NOT EXISTS viewNode AS SELECT Level, Number, Ip, Port, "
                           "Timestamp_ms, 'Natter' AS Type FROM NatterNode ORDER BY Timestamp_ms;");
  }
}

// * Create specific loggers
std::shared_ptr<minhton::MinhtonLoggerNs3> LoggerManager::createMinhtonLogger(
    uint32_t device_id, const std::string &app_name_postfix) {
  minhton_exists_ = true;
  std::string app_name = "Minhton";
  if (!app_name_postfix.empty()) {
    app_name += "+" + app_name_postfix;
  }
  auto log_dev_app = [this, device_id, app_name](const std::string &application_uuid) {
    this->logDeviceApplication(application_uuid, app_name, device_id);
  };

  auto log_event = [this](const std::string &event_uuid, uint16_t event_type,
                          const std::string &application_uuid) {
    this->logEvent(event_uuid, minhton_event_type_base_ + event_type, application_uuid);
  };

  return std::make_shared<minhton::MinhtonLoggerNs3>(
      log_dev_app, [this](const std::string &sql) -> void { this->sqlite_helper_.execute(sql); },
      log_event);
}

std::shared_ptr<natter::logging::NatterLoggerNs3> LoggerManager::createNatterLogger(
    uint32_t device_id) {
  natter_exists_ = true;
  auto log_dev_app = [this, device_id](const std::string &application_uuid) {
    this->logDeviceApplication(application_uuid, "Natter", device_id);
  };

  auto log_event = [this](const std::string &event_uuid, uint16_t event_type,
                          const std::string &application_uuid) {
    this->logEvent(event_uuid, natter_event_type_base_ + event_type, application_uuid);
  };

  return std::make_shared<natter::logging::NatterLoggerNs3>(
      log_dev_app, [this](const std::string &sql) -> void { this->sqlite_helper_.execute(sql); },
      log_event);
}

std::shared_ptr<daisi::cpps::CppsLoggerNs3> LoggerManager::createAMRLogger(uint32_t device_id) {
  auto log_dev_app = [this, device_id](const std::string &application_uuid) {
    this->logDeviceApplication(application_uuid, "AMR", device_id);
  };

  return std::make_shared<daisi::cpps::CppsLoggerNs3>(
      log_dev_app, [this](const std::string &sql) -> void { this->sqlite_helper_.execute(sql); });
}

std::shared_ptr<daisi::cpps::CppsLoggerNs3> LoggerManager::createTOLogger(uint32_t device_id) {
  auto log_dev_app = [this, device_id](const std::string &application_uuid) {
    this->logDeviceApplication(application_uuid, "TO", device_id);
  };

  return std::make_shared<daisi::cpps::CppsLoggerNs3>(
      log_dev_app, [this](const std::string &sql) -> void { this->sqlite_helper_.execute(sql); });
}

// std::shared_ptr<daisi::path_planning::PathPlanningLoggerNs3>
// LoggerManager::createPathPlanningLogger(uint32_t device_id, const std::string &device_type) {
//   auto log_dev_app = [this, device_id, device_type](const std::string &application_uuid) {
//     const std::string app_name =
//         device_type.empty() ? "PathPlanning" : "PathPlanning " + device_type;
//     this->logDeviceApplication(application_uuid, app_name, device_id);
//   };

//   return std::make_shared<daisi::path_planning::PathPlanningLoggerNs3>(
//       log_dev_app, [this](const std::string &sql) -> void { this->sqlite_helper_.execute(sql);
//       });
// }

std::shared_ptr<sola_ns3::SolaLoggerNs3> LoggerManager::createSolaLogger(uint32_t device_id) {
  auto log_dev_app = [this, device_id](const std::string &application_uuid) {
    this->logDeviceApplication(application_uuid, "Sola", device_id);
  };

  return std::make_shared<sola_ns3::SolaLoggerNs3>(
      log_dev_app, [this](const std::string &sql) -> void { this->sqlite_helper_.execute(sql); });
}

}  // namespace daisi

#undef TableDefinition
#undef ViewDefinition
