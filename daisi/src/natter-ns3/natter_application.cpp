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
      TypeId("NatterApp").SetParent<Application>().AddConstructor<NatterApplication>();
  return tid;
}

Ptr<NatterNodeNs3> NatterApplication::getNatterNode() { return natter_node_; }

void NatterApplication::DoDispose() {
  StopApplication();
  Application::DoDispose();
}

void NatterApplication::StartApplication() {
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

uint16_t NatterApplication::getPort() const {
  if (!natter_node_) throw std::runtime_error("natter not initialized");
  return natter_node_->getNetworkInfo().port;
}

std::string NatterApplication::getIP() const {
  if (!natter_node_) throw std::runtime_error("natter not initialized");
  return natter_node_->getNetworkInfo().ip;
}

void NatterApplication::logSelfToDB(std::pair<uint32_t, uint32_t> level_number) {
  const auto [level, number] = level_number;
  logger_->logNewNetworkPeer(natter_node_->getUUID(), getIP(), getPort(), level, number);
}
}  // namespace daisi::natter_ns3
