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

// * MaterialFlow Tasks
TableDefinition kMaterialFlowTask("MaterialFlowTask",
                                  {
                                      DatabaseColumnInfo{"Id"},
                                      {"TaskUuid", "%s", true},
                                      {"TaskName", "%s", true},
                                      {"MaterialFlowId", "sql%u", false, "MaterialFlow(Id)"},
                                      {"FollowUpTaskUuids", "%s", true},
                                      {"LoadCarrierRequirement", "%s", false},
                                      {"PayloadRequirement_kg", "%f", false},
                                  });
static const std::string kCreateMaterialFlowTask = getCreateTableStatement(kMaterialFlowTask);
static bool material_flow_task_exists_ = false;

void CppsLoggerNs3::logMaterialFlowTask(const material_flow::Task &task,
                                        const std::string &material_flow_uuid) {
  if (!material_flow_task_exists_) {
    log_(kCreateMaterialFlowTask);
    material_flow_task_exists_ = true;
  }

  std::string material_flow_id =
      "(SELECT Id FROM MaterialFlow WHERE Uuid='" + material_flow_uuid + "')";

  std::string follow_up_tasks = "";
  for (const auto &follow_up : task.getFollowUpTasks()) {
    follow_up_tasks += follow_up + ",";
  }

  auto ability = task.getAbilityRequirement();

  auto t = std::make_tuple(
      /* TaskUuid */ task.getUuid().c_str(),
      /* TaskName */ task.getName().c_str(),
      /* MaterialFlowId */ material_flow_id.c_str(),
      /* FollowUpTaskUuids */ follow_up_tasks.c_str(),
      /* LoadCarrierRequirement */ ability.getLoadCarrier().getTypeAsString().c_str(),
      /* PayloadRequirement_kg */ ability.getMaxPayloadWeight());
  log_(getInsertStatement(kMaterialFlowTask, t));
}

// * MaterialFlow Orders
TableDefinition kMaterialFlowOrder("MaterialFlowOrder",
                                   {
                                       DatabaseColumnInfo{"Id"},
                                       {"OrderUuid", "%s", true},
                                       {"TaskUuid", "%s", true,
                                        "MaterialFlowTask(TaskUuid)"},  // add foreign key
                                       {"Type", "%s", true},
                                       {"Step1_Name", "%s", true},
                                       {"Step1_Parameters", "%s", false},
                                       // {"Step1_StationId", "%s", false}, // TODO
                                       {"Step2_Name", "%s", false},
                                       {"Step2_Parameters", "%s", false},
                                       // {"Step2_StationId", "%s", false}, // TODO
                                   });
static const std::string kCreateMaterialFlowOrder = getCreateTableStatement(kMaterialFlowOrder);
static bool material_flow_order_exists_ = false;

void CppsLoggerNs3::logMaterialFlowOrder(const material_flow::Order &order,
                                         const std::string &task_uuid) {
  // TODO: use station ids

  if (!material_flow_order_exists_) {
    log_(kCreateMaterialFlowOrder);
    material_flow_order_exists_ = true;
  }

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
    step1_parameter = "todo";

    step2_name = to->getDeliveryTransportOrderStep().getName();
    step2_parameter = "todo";
  } else if (auto mo = std::get_if<material_flow::MoveOrder>(&order)) {
    order_uuid = mo->getUuid();
    type = "move";

    step1_name = mo->getMoveOrderStep().getName();
    step1_parameter = "todo";
  } else if (auto ao = std::get_if<material_flow::ActionOrder>(&order)) {
    order_uuid = ao->getUuid();
    type = "action";

    step1_name = ao->getActionOrderStep().getName();
    step1_parameter = "todo";

  } else {
    throw std::runtime_error("Order type not supported.");
  }

  auto t = std::make_tuple(
      /* OrderUuid */ order_uuid.c_str(),
      /* TaskUuid */ task_uuid.c_str(),
      /* Type */ type.c_str(),
      /* Step1_Name */ step1_name.c_str(),
      /* Step1_Parameters */ step1_parameter.c_str(),
      /* Step2_Name */ step2_name.c_str(),
      /* Step2_Parameters*/ step2_parameter.c_str());
  log_(getInsertStatement(kMaterialFlowOrder, t));
}

// * TransportOrderHistory
TableDefinition kTransportOrderHistory("TransportOrderHistory",
                                         {DatabaseColumnInfo{"Id"},
                                          {"TransportOrderId", "sql%u", true, "TransportOrder(Id)"},
                                          {"Timestamp_ms", "%u", true},
                                          {"State", "%u", true},
                                          {"PosX_m", "%f", true},
                                          {"PosY_m", "%f", true},
                                          {"AmrId", "sql%u", false, "AutonomousMobileRobot(Id)"}
                                          /*{"TransportServiceId", "%u", false,
                                           "TransportService(Id)"}*/});
static const std::string kCreateTransportOrderHistory =
    getCreateTableStatement(kTransportOrderHistory);
static bool transport_order_history_exists_ = false;

// void CppsLoggerNs3::logTransportOrderUpdate(const material_flow::Task &task,
//                                             const std::string &assigned_amr) {
//   if (!transport_order_history_exists_) {
//     log_(kCreateTransportOrderHistory);
//     transport_order_history_exists_ = true;
//   }

//   ns3::Vector pos = {0, 0, 0};  // order.getCurrentPosition();
//   // auto order_state = order.getOrderState();
//   OrderStates order_state = OrderStates::kCreated;

//   std::string transport_order_id =
//       "(SELECT Id FROM TransportOrder WHERE OrderUuid='" + task.getUuid() + "')";

//   auto table = kTransportOrderHistory;
//   if (!assigned_amr.empty()) {
//     std::string amr_id =
//         "(SELECT Id FROM AutonomousMobileRobot WHERE ApplicationUuid='" + assigned_amr + "')";
//     auto t = std::make_tuple(
//         /* TransportOrderId */ transport_order_id.c_str(),
//         /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
//         /* State */ order_state,
//         /* PosX_m */ pos.x,
//         /* PosY_m */ pos.y,
//         /* AmrId */ amr_id.c_str()
//         // /* TransportServiceId */ 1  // TODO
//     );
//     log_(getInsertStatement(table, t));
//   } else {
//     table.columns[6] = {"AmrId", "NULL"};
//     auto t = std::make_tuple(
//         /* TransportOrderId */ transport_order_id.c_str(),
//         /* Timestamp_ms */ ns3::Simulator::Now().GetMilliSeconds(),
//         /* State */ order_state,
//         /* PosX_m */ pos.x,
//         /* PosY_m */ pos.y
//         // /* TransportServiceId */ 1  // TODO
//     );
//     log_(getInsertStatement(table, t));
//   }
// }

}  // namespace daisi::cpps
