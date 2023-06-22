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

#ifndef DAISI_AGV_PHYSICAL_BASIC_H_
#define DAISI_AGV_PHYSICAL_BASIC_H_

#include "cpps/agv/topology.h"
#include "cpps/amr/amr_description.h"
#include "cpps/amr/physical/amr_mobility_model_ns3.h"
#include "ns3/socket.h"

namespace daisi::cpps {

class AGVPhysicalBasic {
public:
  AGVPhysicalBasic(const AmrDescription &data_model, const TopologyNs3 &topology,
                   int id_for_friendly,
                   ns3::Ptr<ns3::Socket> socket);  // TODO WORKAROUND: id_for_friendly

  void connect(ns3::InetSocketAddress endpoint);

private:
  void mobilityCallback();

  void readFromSocket(ns3::Ptr<ns3::Socket> socket);

  void sendPosUpdate();

  static constexpr uint32_t kUpdateFrequencyHz = 30;
  void sendFieldMessage(const std::string &content);

  ns3::EventId next_update_event_;

  // mechanical information
  AmrDescription amr_description_;
  ns3::Ptr<AmrMobilityModelNs3> mobility_;

  // global information
  Topology topology_;

  ns3::Ptr<ns3::Socket> socket_;

  ns3::Vector goal_;

  enum class State { kIdle, kMoving, kLoading, kUnloading } state_ = State::kIdle;

  std::string generatePosUpdate();
};

}  // namespace daisi::cpps

#endif  // DAISI_AGV_PHYSICAL_BASIC_H_
