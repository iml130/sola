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
#include "cpps/amr/message/amr_state.h"
#include "cpps/amr/message/serializer.h"
#include "cpps/logical/logical_agent.h"
#include "cpps/logical/order_management/order_management.h"
#include "ns3/socket.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

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

class AmrLogicalAgent : public LogicalAgent {
public:
  AmrLogicalAgent(uint32_t device_id, const AlgorithmConfig &_config, const bool first_node);

  ~AmrLogicalAgent() = default;

  /// @brief Method called by the container on start. Initializing components such as Sola.
  virtual void init(ns3::Ptr<ns3::Socket> tcp_socket);

  virtual void start();

  // to initialize socket to physical AMR
  bool connectionRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);
  void newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);

private:
  virtual void initAlgorithms() override;

  /// @brief Method being called by sola when we receive a 1-to-1 message
  /// @param m received message
  virtual void messageReceiveFunction(const sola::Message &msg) override;

  /// @brief Method being called by sola when we receive a message via a topic
  /// @param m received message
  virtual void topicMessageReceiveFunction(const sola::TopicMessage &msg) override;

  // from amr socket
  AmrDescription description_;
  bool description_set_;  // to avoid overriding

  Topology topology_;

  daisi::util::Position current_position_;
  AmrState current_state_;

  void sendTopologyToPhysical();
  void sendTaskToPhysical();  // use mapping to functionalities first

  void readFromPhysicalSocket(ns3::Ptr<ns3::Socket> socket);

  void processMessageAmrDescription(const AmrDescription &description);
  void processMessageAmrStatusUpdate(const AmrStatusUpdate &status_update);
  void processMessageAmrOrderUpdate(const AmrOrderUpdate &order_update);

  // for TCP communication with the Amr Physical Asset
  ns3::Ptr<ns3::Socket> socket_to_physical_;
  ns3::Ptr<ns3::Socket> socket_of_physical_;

  std::shared_ptr<order_management::OrderManagement> order_management_;
};
}  // namespace daisi::cpps::logical

#endif
