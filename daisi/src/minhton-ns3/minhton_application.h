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

#ifndef DAISI_MINHTON_NS3_MINHTON_APPLICATION_H_
#define DAISI_MINHTON_NS3_MINHTON_APPLICATION_H_

#include "logging/logger_manager.h"
#include "minhton/utils/serializer_cereal.h"
#include "minhton_logger_ns3.h"
#include "minhton_node_ns3.h"
#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/socket.h"

namespace daisi::minhton_ns3 {

class MinhtonApplication final : public ns3::Application {
public:
  static ns3::TypeId GetTypeId();
  MinhtonApplication() = default;
  ~MinhtonApplication() final = default;

  ns3::Ptr<MinhtonNodeNs3> getMinhtonNode();

  void initializeNode(minhton::ConfigNode config);

  void StopApplication() final;

protected:
  void DoDispose() final;

private:
  void StartApplication() final;

  ns3::Ptr<MinhtonNodeNs3> minhton_node_;

  ns3::Ipv4Address local_ip_address_;

  uint16_t listening_port_ = 0;

  std::shared_ptr<minhton::MinhtonLoggerNs3> logger_;
};
}  // namespace daisi::minhton_ns3
#endif
