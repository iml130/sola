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

#ifndef DAISI_TRANSPORT_ORDER_NS3_H_
#define DAISI_TRANSPORT_ORDER_NS3_H_

#include <vector>

#include "cpps/common/cpps_logger_ns3.h"
#include "cpps/common/uuid_generator.h"
#include "cpps/message/serializer.h"
#include "cpps/model/material_flow_model.h"
#include "cpps/negotiation/initiator/task_allocation_initiator.h"
#include "cpps/negotiation/mrta_configuration.h"
#include "ns3/mobility-model.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::cpps {

class TaskUpdate;

// Wrapper class
class TransportOrderApplicationNs3 {
public:
  // Constructor
  TransportOrderApplicationNs3(const MRTAConfig &mrta_config, uint32_t device_id);

  friend std::ostream &operator<<(std::ostream &os, const TransportOrderApplicationNs3 &to);

  void init();

  OrderStates getState() const;

  void completeYourself(const std::shared_ptr<ScenariofileParser::Table> &model_description,
                        const std::vector<ns3::Vector> &locations);
  void clearMaterialFlowInformation();

  uint getUuid();

  void logReceivedSolaMessage(const sola::Message &sola_msg, const Message &deserialized_msg);
  NegotiationTrafficLoggingInfo getSolaMessageLoggingInfo(const Message &deserialized_msg);

  std::shared_ptr<sola_ns3::SOLAWrapperNs3> getSOLA() const { return sola_; }

  bool isBusy() const;

  void setWaitingForStart();

private:
  void processMessageUpdate(const TaskUpdate &to_update);

  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;

  const std::shared_ptr<daisi::cpps::CppsLoggerNs3> logger_;

  const uint32_t device_id_ = 0;
  std::shared_ptr<MaterialFlowModel> material_flow_model_;

  MRTAConfig mrta_config_;
  std::unique_ptr<TaskAllocationInitiator> task_allocation_initiator_;

  bool first_node_;  // True, if we are the first node in the network

  std::string uuid_;

  bool waiting_for_start_;
};
}  // namespace daisi::cpps
#endif
