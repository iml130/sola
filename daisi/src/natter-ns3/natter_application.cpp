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

#include "logging/logger_manager.h"
#include "solanet/uuid_generator.h"
#include "utils/daisi_check.h"
#include "utils/sola_utils.h"

using namespace ns3;

namespace daisi::natter_ns3 {

TypeId NatterApplication::GetTypeId() {
  static TypeId tid =
      TypeId("NatterApp").SetParent<Application>().AddConstructor<NatterApplication>();
  return tid;
}

void NatterApplication::StartApplication() {
  DAISI_CHECK(mode_ == NatterMode::kMinhcast, "Only MINHCAST supported at the moment!");
  DAISI_CHECK(!natter_minhcast_, "natter already initialized");

  logger_ = daisi::global_logger_manager->createNatterLogger();
  std::vector<natter::logging::LoggerPtr> logger_list{logger_};

  natter_minhcast_ = std::make_unique<natter::minhcast::NatterMinhcast>(
      [](const natter::Message &) { /*nothing required here */ },
      [](const std::string & /*unused*/) {}, logger_list);
}

void NatterApplication::setMode(NatterMode mode) {
  DAISI_CHECK(mode == NatterMode::kMinhcast, "Only MINHCAST supported at the moment!");
  mode_ = mode;
}

void NatterApplication::setLevelNumber(std::pair<uint32_t, uint32_t> level_number) {
  DAISI_CHECK(logger_, "Logger not initialized");

  std::swap(level_number_, level_number);
  logSelfToDB(level_number_);
  logger_->logNatterEvent(1, solanet::generateUUID());
}

void NatterApplication::logSelfToDB(std::pair<uint32_t, uint32_t> level_number) {
  DAISI_CHECK(natter_minhcast_, "natter not initialized");
  DAISI_CHECK(logger_, "Logger not initialized");

  const auto [level, number] = level_number;
  const natter::NetworkInfoIPv4 net_info = natter_minhcast_->getNetworkInfo();

  logger_->logNewNetworkPeer(natter_minhcast_->getUUID(), net_info.ip, net_info.port, level,
                             number);
}

void NatterApplication::addPeer(const std::string &topic,
                                const natter::minhcast::NatterMinhcast::NodeInfo &info) {
  DAISI_CHECK(natter_minhcast_, "natter not initialized");
  DAISI_CHECK(logger_, "Logger not initialized");

  natter_minhcast_->addPeer(topic, info);
  logger_->logNs3PeerConnection(Simulator::Now().GetMicroSeconds(), true, getUUID(), info.uuid);
}

void NatterApplication::removePeer(const std::string & /*topic*/, const std::string & /*uuid*/) {
  throw std::runtime_error("not implemented yet");
}

solanet::UUID NatterApplication::getUUID() const {
  DAISI_CHECK(natter_minhcast_, "natter not initialized");
  return natter_minhcast_->getUUID();
}

void NatterApplication::publish(const std::string &topic, const std::string &msg) {
  DAISI_CHECK(natter_minhcast_, "natter not initialized");
  DAISI_CHECK(logger_, "Logger not initialized");

  solanet::UUID res = natter_minhcast_->publish(topic, msg);

  logger_->logNatterEvent(2, res);
}

void NatterApplication::subscribeTopic(const std::string &topic,
                                       const natter::minhcast::NatterMinhcast::NodeInfo &info) {
  DAISI_CHECK(natter_minhcast_, "natter not initialized");

  natter_minhcast_->subscribeTopic(topic, info);
}

void NatterApplication::unsubscribeTopic(const std::string &topic) {
  DAISI_CHECK(natter_minhcast_, "natter not initialized");
  natter_minhcast_->unsubscribeTopic(topic);
}

natter::NetworkInfoIPv4 NatterApplication::getNetworkInfo() const {
  DAISI_CHECK(natter_minhcast_, "natter not initialized");
  return natter_minhcast_->getNetworkInfo();
}
}  // namespace daisi::natter_ns3
