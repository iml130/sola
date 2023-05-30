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

#ifndef DAISI_CPPS_LOGICAL_AMR_AMR_LOGICAL_AGENT_H_
#define DAISI_CPPS_LOGICAL_AMR_AMR_LOGICAL_AGENT_H_

#include <memory>
#include <string>

#include "cpps/amr/amr_description.h"
#include "cpps/amr/amr_topology.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {

struct AlgorithmConfig {};

enum class SocketType : int {
  k_tcp = 0,
  k_udp = 1,
};

struct SocketConfig {
  std::string ip_;
  int port_;
  SocketType type_;
};

struct SocketInfo {
  SocketConfig local_config_;
  SocketConfig remote_config_;
};

class AmrLogicalAgent {
public:
  AmrLogicalAgent(uint32_t device_id, const AlgorithmConfig &_config, const bool _first_node);

  void init(const SocketInfo &_amr_socket_info, const SocketInfo &_sola_socket_info);

private:
  void processMessage(const std::string &_payload);
  void sendTopologyToPhysical();

  void sendOrdersToPhysical();

  // TODO continue when socket connector is finished
  void readFromAmrSocket(const std::string &_payload);
  void readFromSolaSocket(const std::string &_payload);

  // logger_connector_ (atm ns3::Logger)

  // amr_socket_connector_ (atm ns3::Socket)
  // sola_socket_connector_(atm ns3::Socket)

  // std::shared_ptr<SOLAWrapperN> sola_ (atm SOLAWrapperNs3)

  // from amr socket
  AmrDescription description_;

  // AmrTopology topology_ (from constructor or sola_socket_connector_) (to be sent to
  // amr_socket_connector_)

  // std::unique_ptr<DispositionParticipant> disposition_participant_ (previously called
  // task_allocation_participant_)

  // from regular position updates
  std::unique_ptr<daisi::util::Position> current_position_;
  // AmrState current_state_;

  // use friendly name / serial number from description_ instead of uuid_

  // members only used for initialization
  AlgorithmConfig config_;
  bool first_node_;  // for now
};
}  // namespace daisi::cpps
