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

#ifndef DAISI_CPPS_AMR_PHYSICAL_AMR_PHYSICAL_ASSET_H_
#define DAISI_CPPS_AMR_PHYSICAL_AMR_PHYSICAL_ASSET_H_

#include <functional>
#include <queue>
#include <random>
#include <vector>

#include "cpps/amr/message/amr_order_info.h"
#include "cpps/amr/message/amr_state.h"
#include "cpps/amr/physical/amr_asset_connector.h"
#include "cpps/amr/physical/amr_order.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/object.h"
#include "ns3/simulator.h"
#include "ns3/socket.h"
#include "ns3/vector.h"

namespace daisi::cpps {

/// @brief Manages communication with the corresponding logical agent and execution of transport
/// orders
class AmrPhysicalAsset {
public:
  AmrPhysicalAsset(AmrAssetConnector connector, const Topology &topology);
  explicit AmrPhysicalAsset(AmrAssetConnector connector);

  void init(const ns3::Ptr<ns3::Socket> &socket);

  void connect(const ns3::InetSocketAddress &endpoint);

  /// @brief When a functionality is finished, it will be deleted from the vector and the next one
  /// will be started.
  void updateFunctionality(const FunctionalityVariant &functionality);

private:
  // communication with Logical
  ns3::Ptr<ns3::Socket> socket_;
  /// @brief Get position and send it to corresponding logical agent
  void sendVehicleStatusUpdateNs3(bool force);
  /// @brief send current OrderState to corresponding logical agent
  void sendOrderUpdateNs3();
  /// @brief receive a task
  void readSocket(ns3::Ptr<ns3::Socket> socket);
  /// @brief send description to corresponding logical agent during registration
  void sendDescriptionNs3();
  void scheduleVehicleStatusUpdateNs3();
  void startVehicleStatusUpdates();
  void stopVehicleStatusUpdatesNs3();
  ns3::EventId next_update_event_;

  void processMessageOrderInfo(const AmrOrderInfo &order_info);
  void continueOrder();

  util::Position getPosition() const;

  AmrAssetConnector connector_;
  AmrOrder current_order_;
  std::queue<FunctionalityVariant> functionality_queue_;
  util::Position last_sent_position_;
  AmrState amr_state_ = AmrState::kIdle;
};
}  // namespace daisi::cpps
#endif
