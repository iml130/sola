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

#ifndef DAISI_CPPS_COMMON_CPPS_LOGGER_NS3_H_
#define DAISI_CPPS_COMMON_CPPS_LOGGER_NS3_H_

#include <ctime>

#include "SOLA/service.h"
#include "cpps/amr/message/amr_state.h"
#include "cpps/model/order_states.h"
#include "logging/definitions.h"
#include "material_flow/model/task.h"

namespace daisi::cpps {

struct AMRLoggingInfo {
  std::string friendly_name;
  std::string manufacturer;
  std::string model_name;
  uint32_t model_number;
  uint32_t serial_number;
  std::string ip_logical_core;
  uint16_t port_logical_core;
  std::string ip_logical_asset;
  uint16_t port_logical_asset;
  std::string ip_physical;
  uint16_t port_physical;
  std::string load_carrier_type;
  double max_weight;
  uint64_t load_time;
  uint64_t unload_time;
  double max_velocity;
  double min_velocity;
  double max_acceleration;
  double min_acceleration;
};

struct AMRPositionLoggingInfo {
  std::string uuid;
  double x;
  double y;
  double z;
  uint8_t state;
};

struct NegotiationTrafficLoggingInfo {
  std::string order;
  std::string sender_ip;
  uint16_t sender_port = 0;
  std::string target_ip;
  uint16_t target_port = 0;
  uint8_t message_type = 0;
  std::string content;
};

struct ExecutedOrderUtilityLoggingInfo {
  std::string order;
  std::string amr;
  double expected_start_time;

  double execution_duration;
  double execution_distance;
  double travel_to_pickup_duration;
  double travel_to_pickup_distance;
  double makespan;
  double delay;

  double delta_execution_duration;
  double delta_execution_distance;
  double delta_travel_to_pickup_duration;
  double delta_travel_to_pickup_distance;
  double delta_makespan;

  int queue_size;
  double quality;
  double costs;
  double utility;
};

struct MaterialFlowOrderUpdateLoggingInfo {
  std::string amr_uuid;
  AmrState amr_state = AmrState::kIdle;
  OrderStates order_state = OrderStates::kCreated;
  material_flow::Task task;
  uint8_t order_index = 0;
  util::Position position;
};

class CppsLoggerNs3 {
public:
  CppsLoggerNs3() = delete;
  explicit CppsLoggerNs3(LogDeviceApp log_device_application, LogFunction log);
  ~CppsLoggerNs3();

  // cpps specific logging functions
  void logAMR(const AMRLoggingInfo &amr_info);
  void logStation(const std::string &name, const std::string &type, ns3::Vector2D position,
                  const std::vector<ns3::Vector2D> &additionalPositions = {});
  void logTransportService(const sola::Service &service, bool active);
  void logService(const std::string &uuid, uint8_t type);
  void logPositionUpdate(const AMRPositionLoggingInfo &logging_info);
  void logNegotiationTraffic(const NegotiationTrafficLoggingInfo &logging_info);
  void logExecutedOrderCost(const ExecutedOrderUtilityLoggingInfo &logging_info);
  void logTopicMessage(const std::string &topic, const std::string &message_id,
                       const std::string &node, const std::string &message, bool receive);
  void logTopicEvent(const std::string &topic, const std::string &node, bool subscribe);
  void logCppsMessageTypes();

  void logMaterialFlow(const std::string &mf_uuid, const std::string &ip, uint16_t port,
                       uint8_t state);
  void logMaterialFlowOrder(const material_flow::Order &order, const std::string &task_uuid);
  void logMaterialFlowTask(const material_flow::Task &task, const std::string &material_flow_uuid);
  void logMaterialFlowOrderUpdate(const MaterialFlowOrderUpdateLoggingInfo &logging_info);

  // Used for loggers which are initialized before node starts
  // TODO Refactor to other class
  void setApplicationUUID(const std::string &app_uuid) {
    uuid_ = app_uuid;
    log_device_application_(uuid_);
  }

protected:
  // TODO Refactor to other class
  LogDeviceApp log_device_application_;
  LogFunction log_;

private:
  std::string uuid_;
};

}  // namespace daisi::cpps

#endif  // CPPS_LOGGER_NS3_H_
