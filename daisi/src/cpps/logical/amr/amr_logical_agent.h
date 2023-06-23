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

#include "amr_logical_execution_state.h"
#include "cpps/amr/amr_description.h"
#include "cpps/amr/amr_topology.h"
#include "cpps/amr/message/amr_state.h"
#include "cpps/amr/message/serializer.h"
#include "cpps/logical/logical_agent.h"
#include "cpps/logical/order_management/order_management.h"
#include "ns3/socket.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

class AmrLogicalAgent : public LogicalAgent {
public:
  AmrLogicalAgent(uint32_t device_id, const AlgorithmConfig &config, bool first_node);

  ~AmrLogicalAgent() override = default;

  /// @brief Method called by the container on start. Initializing components such as Sola.
  virtual void init();

  void start() override;

  void notifyTaskAssigned();

  /// Get address from TCP socket listening on connections from AmrPhysicalAsset
  ns3::InetSocketAddress getServerAddress() const {
    ns3::Address addr;
    server_socket_->GetSockName(addr);
    return ns3::InetSocketAddress::ConvertFrom(addr);
  }

private:
  void initAlgorithms() override;

  /// @brief Method being called by sola when we receive a 1-to-1 message
  /// @param m received message
  void messageReceiveFunction(const sola::Message &msg) override;

  /// @brief Method being called by sola when we receive a message via a topic
  /// @param m received message
  void topicMessageReceiveFunction(const sola::TopicMessage &msg) override;

  /// @brief We always accept an incoming connection.
  bool connectionRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);

  /// @brief Once a connection request is received and accepted via the server socket, this method
  /// is called and the physical socket created.
  void newConnectionCreated(ns3::Ptr<ns3::Socket> socket, const ns3::Address &addr);

  void readFromPhysicalSocket(ns3::Ptr<ns3::Socket> socket);

  void processMessageAmrDescription(const AmrDescription &description);
  void processMessageAmrStatusUpdate(const AmrStatusUpdate &status_update);
  void processMessageAmrOrderUpdate(const AmrOrderUpdate &order_update);

  void sendTopologyToPhysical();
  void sendTaskToPhysical();

  void checkSendingNextTaskToPhysical();

  void logAmrInfos();
  void logPositionUpdate();
  void logOrderUpdate();

  void sendToPhysical(std::string payload);

  /// @brief make the AMR agent discoverable for findService queries
  void setServices();

  AmrDescription description_;
  bool description_set_ = false;  // to avoid overriding

  Topology topology_;

  AmrLogicalExecutionState execution_state_;

  /// @brief For TCP communication with the AmrPhysicalAsset.
  /// The AmrLogicalAgent is acting as the server according to the TCP model.
  /// It uses the socket to listen to connection requests.
  ns3::Ptr<ns3::Socket> server_socket_;

  /// @brief For TCP communication with the AmrPhysicalAsset.
  /// This socket is used to communicate with the physical, for both sending and receiving messages.
  ns3::Ptr<ns3::Socket> physical_socket_;

  std::shared_ptr<OrderManagement> order_management_;
};
}  // namespace daisi::cpps::logical

#endif
