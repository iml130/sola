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

#ifndef DAISI_SOLA_NS3_SOLA_APPLICATION_H_
#define DAISI_SOLA_NS3_SOLA_APPLICATION_H_

#include "logging/logger_manager.h"
#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/simulator.h"
#include "ns3/socket.h"
#include "ns3/uinteger.h"
#include "sola_logger_ns3.h"
#include "sola_ns3_wrapper.h"

namespace daisi::sola_ns3 {

class SolaApplication : public ns3::Application {
public:
  static ns3::TypeId GetTypeId();
  SolaApplication() = default;

  // Start SOLA later as the application
  void startSOLA();

  std::string getIP() const;
  uint16_t getPort() const;

  void subscribeTopic(const std::string &topic);
  void publishTopic(const std::string &topic, const std::string &msg);

private:
  void DoDispose() final;
  void StopApplication() final;
  void StartApplication() final;  // Called from ns-3 without delay

  uint32_t id_ = 0;

  std::unique_ptr<SOLAWrapperNs3> sola_;

  ns3::Ipv4Address local_ip_address_;

  uint16_t listening_port_ = 0;
};
}  // namespace daisi::sola_ns3
#endif
