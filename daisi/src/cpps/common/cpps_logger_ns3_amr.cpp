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

#include "cpps_logger_ns3.h"
#include "ns3/simulator.h"

#define TableDefinition static const DatabaseTable
#define ViewDefinition static const std::unordered_map<std::string, std::string>

namespace daisi::cpps {

// * CppsAMRHistory
TableDefinition kAmrHistory("CppsAMRHistory",
                            {DatabaseColumnInfo{"Id"},
                             {"Timestamp_ms", "%u", true},
                             {"AmrId", "sql%u", true, "CppsAutonomousMobileRobot(Id)"},
                             {"PosX_m", "%f", true},
                             {"PosY_m", "%f", true},
                             {"State", "%u", true}});
static const std::string kCreateAmrHistory = getCreateTableStatement(kAmrHistory);
static bool amr_history_exists_ = false;

void CppsLoggerNs3::logPositionUpdate(const AmrPositionLoggingInfo &logging_info) {
  if (!amr_history_exists_) {
    log_(kCreateAmrHistory);
    amr_history_exists_ = true;
  }

  std::string amr_id = "(SELECT Id FROM CppsAutonomousMobileRobot WHERE ApplicationUuid='" +
                       logging_info.uuid + "')";
  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* AmrId */ amr_id.c_str(),
      /* PosX_m */ logging_info.x,
      /* PosY_m */ logging_info.y,
      /* State */ logging_info.state);
  log_(getInsertStatement(kAmrHistory, t));
  // logging_info.z_
}

// * CppsAutonomousMobileRobot
TableDefinition kAutonomousMobileRobot("CppsAutonomousMobileRobot",
                                       {DatabaseColumnInfo{"Id"},
                                        {"Timestamp_ms", "%u", true},
                                        {"ApplicationUuid", "%s", true,
                                         "DeviceApplication(ApplicationUuid)"},
                                        {"FriendlyName", "%s"},
                                        {"ModelName", "%s"},
                                        {"IpLogicalService", "%s", true},
                                        {"PortLogicalService", "%u", true},
                                        {"IpPhysicalAsset", "%s", true},
                                        {"PortPhysicalAsset", "%u", true},
                                        {"IpLocalAsset", "%s", true},
                                        {"PortLocalAsset", "%u", true},
                                        {"LoadTime_ms", "%u", true},
                                        {"UnloadTime_ms", "%u", true},
                                        {"MaxWeight_kg", "%f", true},
                                        {"MaxVelocity_mps", "%f", true},
                                        {"MinVelocity_mps", "%f", true},
                                        {"MaxAcceleration_mpss", "%f", true},
                                        {"MaxDeceleration_mpss", "%f", true}});
static const std::string kCreateAmr = getCreateTableStatement(kAutonomousMobileRobot);
static bool amr_exists_ = false;

void CppsLoggerNs3::logAMR(const AmrLoggingInfo &amr_info) {
  if (!amr_exists_) {
    log_(kCreateAmr);
    amr_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* ApplicationUuid */ uuid_.c_str(),
      /* FriendlyName */ amr_info.friendly_name.c_str(),
      /* ModelName */ amr_info.model_name.c_str(),
      /* IpLogicalService */ amr_info.ip_logical_core.c_str(),
      /* PortLogicalService */ amr_info.port_logical_core,
      /* IpPhysicalAsset */ amr_info.ip_physical.c_str(),
      /* PortPhysicalAsset */ amr_info.port_physical,
      /* IpLocalAsset */ amr_info.ip_logical_asset.c_str(),
      /* PortLocalAsset */ amr_info.port_logical_asset,
      /* LoadTime_ms */ amr_info.load_time,
      /* UnloadTime_ms */ amr_info.unload_time,
      /* MaxWeight_kg */ amr_info.max_weight,
      /* MaxVelocity_mps */ amr_info.max_velocity,
      /* MinVelocity_mps */ amr_info.min_velocity,
      /* MaxAcceleration_mpss */ amr_info.max_acceleration,
      /* MaxDeceleration_mpss */ amr_info.min_acceleration);
  log_(getInsertStatement(kAutonomousMobileRobot, t));
}

// * CppsService
TableDefinition kService("CppsService", {{"Uuid", "%s", true, "", true},
                                         {"StartTime_ms", "%u"},
                                         {"Type", "%u", true}});
static const std::string kCreateService = getCreateTableStatement(kService);
static bool service_exists_ = false;

void CppsLoggerNs3::logService(const std::string &uuid, uint8_t type) {
  if (!service_exists_) {
    log_(kCreateService);
    service_exists_ = true;
  }

  auto t = std::make_tuple(
      /* Uuid */ uuid.c_str(),
      /* StartTime_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* Type */ type);
  log_(getInsertStatement(kService, t));
}

// * CppsServiceTransport
TableDefinition kServiceTransport("CppsServiceTransport",
                                  {DatabaseColumnInfo{"Id"},
                                   {"Uuid", "%s", true},
                                   {"AmrId", "sql%u", true, "CppsAutonomousMobileRobot(Id)"},
                                   {"LoadCarrierType", "%s", true},
                                   {"MaxWeightPayload_kg", "%f", true}});
static const std::string kCreateServiceTransport = getCreateTableStatement(kServiceTransport);
static bool service_exists_transport_ = false;

void CppsLoggerNs3::logTransportService(const sola::Service &service, bool /*active*/) {
  if (!service_exists_transport_) {
    log_(kCreateServiceTransport);
    service_exists_transport_ = true;
  }

  auto amr_uuid = std::any_cast<std::string>(service.key_values.at("amruuid"));
  auto type = std::any_cast<std::string>(service.key_values.at("servicetype"));
  auto max_payload = std::any_cast<float>(service.key_values.at("maxpayload"));

  logService(service.uuid, 0);
  std::string uuid = service.uuid;
  std::string amr_id =
      "(SELECT Id FROM CppsAutonomousMobileRobot WHERE ApplicationUuid='" + amr_uuid + "')";
  auto t = std::make_tuple(
      /* Uuid */ uuid.c_str(),
      /* AmrId */ amr_id.c_str(),
      /* LoadCarrierType */ type.c_str(),  // TODO: Change to id based field?
      /* MaxWeightPayload_kg */ max_payload);
  log_(getInsertStatement(kServiceTransport, t));
}

}  // namespace daisi::cpps
