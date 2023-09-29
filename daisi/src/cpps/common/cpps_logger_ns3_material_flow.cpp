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

// * CPPS MaterialFlow Tasks
TableDefinition kMaterialFlowTask("CppsMaterialFlowTask",
                                  {
                                      DatabaseColumnInfo{"Id"},
                                      {"TaskUuid", "%s", true},
                                      {"TaskName", "%s", true},
                                      {"MaterialFlowId", "sql%u", true, "CppsMaterialFlow(Id)"},
                                      {"FollowUpTaskUuids", "%s", true},
                                      {"LoadCarrierRequirement", "%s", true},
                                      {"PayloadRequirement_kg", "%f", true},
                                  });
static const std::string kCreateMaterialFlowTask = getCreateTableStatement(kMaterialFlowTask);

void CppsLoggerNs3::logMaterialFlowTask(const material_flow::Task &task,
                                        const std::string &material_flow_uuid) {
  static bool material_flow_task_exists = false;
  if (!material_flow_task_exists) {
    log_(kCreateMaterialFlowTask);
    material_flow_task_exists = true;
  }

  std::string material_flow_id =
      "(SELECT Id FROM CppsMaterialFlow WHERE Uuid='" + material_flow_uuid + "')";

  std::string follow_up_tasks = "";
  for (const auto &follow_up : task.getFollowUpTaskUuids()) {
    follow_up_tasks += follow_up + ",";
  }

  auto ability = task.getAbilityRequirement();
  std::string load_carrier_type = ability.getLoadCarrier().getTypeAsString();

  auto t = std::make_tuple(
      /* TaskUuid */ task.getUuid().c_str(),
      /* TaskName */ task.getName().c_str(),
      /* MaterialFlowId */ material_flow_id.c_str(),
      /* FollowUpTaskUuids */ follow_up_tasks.c_str(),
      /* LoadCarrierRequirement */ load_carrier_type.c_str(),
      /* PayloadRequirement_kg */ ability.getMaxPayloadWeight());
  log_(getInsertStatement(kMaterialFlowTask, t));
}

// * CppsMaterialFlow Orders
TableDefinition kMaterialFlowOrder("CppsMaterialFlowOrder",
                                   {
                                       DatabaseColumnInfo{"Id"},
                                       {"OrderUuid", "%s", true},
                                       {"TaskId", "sql%u", true, "CppsMaterialFlowTask(Id)"},
                                       {"Type", "%s", true},
                                       {"Step1_Name", "%s", true},
                                       {"Step1_Parameters", "%s", false},
                                       // {"Step1_StationId", "%s", false}, // TODO
                                       {"Step2_Name", "%s", false},
                                       {"Step2_Parameters", "%s", false},
                                       // {"Step2_StationId", "%s", false}, // TODO
                                   });
static const std::string kCreateMaterialFlowOrder = getCreateTableStatement(kMaterialFlowOrder);

std::string parametersToString(const std::unordered_map<std::string, std::string> &parameters) {
  std::string s;

  for (const auto &entry : parameters) {
    s += "{ " + entry.first + ": " + entry.second + "} ";
  }

  return s;
}

void CppsLoggerNs3::logMaterialFlowOrder(const material_flow::Order &order,
                                         const std::string &task_uuid) {
  static bool material_flow_order_exists = false;

  // TODO: use station ids

  if (!material_flow_order_exists) {
    log_(kCreateMaterialFlowOrder);
    material_flow_order_exists = true;
  }

  std::string task_id = "(SELECT Id FROM CppsMaterialFlowTask WHERE TaskUuid='" + task_uuid + "')";

  std::string order_uuid = "";
  std::string type = "";
  std::string step1_name = "";
  std::string step1_parameter = "";
  std::string step2_name = "";
  std::string step2_parameter = "";

  if (auto to = std::get_if<material_flow::TransportOrder>(&order)) {
    order_uuid = to->getUuid();
    type = "transport";

    if (to->getPickupTransportOrderSteps().size() > 1) {
      throw std::runtime_error("More than one pickup TO is not supported yet.");
    }

    step1_name = to->getPickupTransportOrderSteps()[0].getName();
    step1_parameter = parametersToString(to->getPickupTransportOrderSteps()[0].getParameters());

    step2_name = to->getDeliveryTransportOrderStep().getName();
    step2_parameter = parametersToString(to->getDeliveryTransportOrderStep().getParameters());
  } else if (auto mo = std::get_if<material_flow::MoveOrder>(&order)) {
    order_uuid = mo->getUuid();
    type = "move";

    step1_name = mo->getMoveOrderStep().getName();
    step1_parameter = parametersToString(mo->getMoveOrderStep().getParameters());
  } else if (auto ao = std::get_if<material_flow::ActionOrder>(&order)) {
    order_uuid = ao->getUuid();
    type = "action";

    step1_name = ao->getActionOrderStep().getName();
    step1_parameter = parametersToString(ao->getActionOrderStep().getParameters());

  } else {
    throw std::runtime_error("Order type not supported.");
  }

  auto t = std::make_tuple(
      /* OrderUuid */ order_uuid.c_str(),
      /* TaskId */ task_id.c_str(),
      /* Type */ type.c_str(),
      /* Step1_Name */ step1_name.c_str(),
      /* Step1_Parameters */ step1_parameter.c_str(),
      /* Step2_Name */ step2_name.c_str(),
      /* Step2_Parameters*/ step2_parameter.c_str());
  log_(getInsertStatement(kMaterialFlowOrder, t));
}

// * CppsMaterialFlowOrderHistory
TableDefinition kMaterialFlowOrderHistory(
    "CppsMaterialFlowOrderHistory",
    {
        DatabaseColumnInfo{"Id"},
        {"MaterialFlowOrderId", "sql%u", true, "CppsMaterialFlowOrder(Id)"},
        {"MaterialFlowTaskId", "sql%u", true, "CppsMaterialFlowTask(Id)"},
        {"AmrId", "sql%u", false, "CppsAutonomousMobileRobot(Id)"},
        {"Timestamp_ms", "%u", true},
        {"State", "%u", true},
        {"PosX_m", "%f", true},
        {"PosY_m", "%f", true},
    });
static const std::string kCreateMaterialFlowOrderHistory =
    getCreateTableStatement(kMaterialFlowOrderHistory);

void CppsLoggerNs3::logMaterialFlowOrderUpdate(
    const cpps::logical::MaterialFlowUpdate &logging_info) {
  static bool material_flow_order_history_exists = false;

  if (!material_flow_order_history_exists) {
    log_(kCreateMaterialFlowOrderHistory);
    material_flow_order_history_exists = true;
  }

  std::string amr_id = "(SELECT Id FROM CppsAutonomousMobileRobot WHERE ApplicationUuid='" +
                       logging_info.amr_uuid + "')";

  std::string task_id =
      "(SELECT Id FROM CppsMaterialFlowTask WHERE TaskUuid='" + logging_info.task.getUuid() + "')";

  std::string order_uuid;
  std::visit([&order_uuid](const auto &order) { order_uuid = order.getUuid(); },
             logging_info.task.getOrders()[logging_info.order_index]);

  std::string order_id =
      "(SELECT Id FROM CppsMaterialFlowOrder WHERE OrderUuid='" + order_uuid + "')";

  auto t = std::make_tuple(
      /* MaterialFlowOrderId */ order_id.c_str(),
      /* MaterialFlowTaskId */ task_id.c_str(),
      /* AmrId */ amr_id.c_str(),
      /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
      /* State */ logging_info.order_state,
      /* PosX_m */ logging_info.position.x,
      /* PosY_m */ logging_info.position.y);
  log_(getInsertStatement(kMaterialFlowOrderHistory, t));
}

}  // namespace daisi::cpps
