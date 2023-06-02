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

#include "natter_application.h"

#include "../src/utils/utils.h"  // WORKAROUND: private header
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::natter_ns3 {

TypeId NatterApplication::GetTypeId() {
  static TypeId tid =
      TypeId("NatterApp")
          .SetParent<Application>()
          .AddConstructor<NatterApplication>()
          .AddAttribute("LocalIpAddress", "LocalIpAddress", Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&NatterApplication::local_ip_address_),
                        MakeIpv4AddressChecker())
          .AddAttribute("ListeningPort", "ListeningPort", UintegerValue(0),
                        MakeUintegerAccessor(&NatterApplication::listening_port_),
                        MakeUintegerChecker<uint16_t>());
  return tid;
}

Ptr<NatterNodeNs3> NatterApplication::getNatterNode() { return natter_node_; }

void NatterApplication::DoDispose() {
  StopApplication();
  Application::DoDispose();
}

void NatterApplication::StartApplication() {
  SolaNetworkUtils::get().registerNode(local_ip_address_, GetNode(), listening_port_);
  SolaNetworkUtils::get().createSockets(getIpv4AddressString(local_ip_address_));

  logger_ = daisi::global_logger_manager->createNatterLogger(GetNode()->GetId());

  natter_node_ = Create<NatterNodeNs3>(logger_, mode_);
}

void NatterApplication::StopApplication() {}

void NatterApplication::setMode(NatterMode mode) { mode_ = mode; }

void NatterApplication::setLevelNumber(std::pair<uint32_t, uint32_t> level_number) {
  std::swap(level_number_, level_number);
  logSelfToDB(level_number_);
  logger_->logNatterEvent(1, natter::utils::generateUUID());
}

uint16_t NatterApplication::getPort() const { return listening_port_; }

std::string NatterApplication::getIP() const {
  std::stringstream ss;
  local_ip_address_.Print(ss);
  return ss.str();
}

void NatterApplication::logSelfToDB(std::pair<uint32_t, uint32_t> level_number) {
  const auto [level, number] = level_number;
  std::stringstream ss;
  local_ip_address_.Print(ss);
  logger_->logNewNetworkPeer(natter_node_->getUUID(), ss.str(), listening_port_, level, number);
}
}  // namespace daisi::natter_ns3
