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

#ifndef DAISI_CPPS_AGV_AGV_LOGICAL_NS3_H_
#define DAISI_CPPS_AGV_AGV_LOGICAL_NS3_H_

#include <list>

#include "cpps/agv/topology.h"
#include "cpps/amr/amr_description.h"
#include "cpps/amr/amr_topology.h"
#include "cpps/amr/message/amr_order_info.h"
#include "cpps/amr/message/amr_order_update.h"
#include "cpps/amr/message/amr_status_update.h"
#include "cpps/common/agv_description.h"
#include "cpps/common/cpps_logger_ns3.h"
#include "cpps/model/ability.h"
#include "cpps/model/kinematics.h"
#include "cpps/model/task.h"
#include "cpps/negotiation/mrta_configuration.h"
#include "cpps/negotiation/participant/task_allocation_participant.h"
#include "cpps/negotiation/task_management/task_management.h"
#include "logging/logger_manager.h"
#include "ns3/application.h"
#include "ns3/mobility-module.h"
#include "ns3/socket.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::cpps {

class AgvLogicalNs3 {
public:
  AgvLogicalNs3(TopologyNs3 topology, const MRTAConfig &mrta_config, int id_for_friendly,
                uint32_t device_id);

  void init(ns3::Ptr<ns3::Socket> socket);

  void checkOrderQueue();

  void sendOrderToAGV();

  void readFromSocket(ns3::Ptr<ns3::Socket> socket);

  bool connectionRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);
  void newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);

  void logAGV() const;

  void processMessageDescription(const AmrDescription &description);
  void processMessageUpdate(const AmrStatusUpdate &payload);
  void processOrderStatusUpdate(const AmrOrderUpdate &payload);

  void logReceivedSolaMessage(const sola::Message &sola_msg, const Message &deserialized_msg);
  daisi::cpps::NegotiationTrafficLoggingInfo getSolaMessageLoggingInfo(
      const Message &deserialized_msg);

  void createParticipant();

  std::shared_ptr<sola_ns3::SOLAWrapperNs3> getSOLA() const { return sola_; }

private:
  std::string agv_logical_asset_ip_;
  uint16_t agv_logical_asset_port_ = 0;
  std::string agv_physical_ip_;
  uint16_t agv_physical_port_ = 0;

  const uint32_t device_id_ = 0;
  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;
  sola::Service service_;
  ns3::Ptr<ns3::Socket> socket_;
  ns3::Ptr<ns3::Socket> socket_agv_;
  std::shared_ptr<daisi::cpps::CppsLoggerNs3> logger_;

  std::unique_ptr<TaskAllocationParticipant> task_allocation_participant_;
  std::unique_ptr<TaskManagement> task_management_;

  // mechanical information
  AgvDataModel data_model_;

  std::shared_ptr<Kinematics> kinematics_;
  std::shared_ptr<mrta::model::Ability> ability_;
  std::shared_ptr<ns3::Vector> last_position_;

  AmrState last_agv_state_ = AmrState::kIdle;

  bool service_received_ = false;  // currently we can only handle a single service per node

  // global information
  TopologyNs3 topology_;

  uint8_t update_counter_ = 0;

  std::string uuid_ = "";

  MRTAConfig mrta_config_;

  bool first_node_;  // True, if we are the first node in the network
};
}  // namespace daisi::cpps

#endif
