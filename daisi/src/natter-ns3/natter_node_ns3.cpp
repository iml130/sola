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

#include "natter_node_ns3.h"

#include <cassert>

#include "solanet/uuid.h"

using namespace ns3;
using namespace std::placeholders;

namespace daisi::natter_ns3 {

NatterNodeNs3::NatterNodeNs3(std::shared_ptr<natter::logging::NatterLoggerNs3> logger,
                             NatterMode mode)
    : logger_(std::move(logger)) {
  std::vector<natter::logging::LoggerPtr> logger_list{logger_};
  if (mode == NatterMode::kMinhcast) {
    natter_minhcast_ = std::make_unique<natter::minhcast::NatterMinhcast>(
        [&](const natter::Message &) { /*nothing required here */ },
        [](const std::string & /*unused*/) {}, logger_list);
  } else {
    throw std::runtime_error("Invalid natter mode");
  }
}

TypeId NatterNodeNs3::GetTypeId() {
  static TypeId tid =
      TypeId("ns3::NatterNodeNs3").SetParent<Object>().AddConstructor<NatterNodeNs3>();
  return tid;
}

TypeId NatterNodeNs3::GetInstanceTypeId() const { return GetTypeId(); }

void NatterNodeNs3::addPeer(const std::string &topic, solanet::UUID uuid, const std::string &ip,
                            uint16_t port, uint32_t level, uint32_t number, uint32_t fanout) {
  if (natter_minhcast_)
    natter_minhcast_->addPeer(topic, {{level, number, fanout}, {ip, port}, uuid});
  logger_->logNs3PeerConnection(Simulator::Now().GetMicroSeconds(), true, getUUID(), uuid);
}

void NatterNodeNs3::removePeer(const std::string & /*topic*/, const std::string & /*uuid*/) {
  throw std::runtime_error("not implemented yet");
}

solanet::UUID NatterNodeNs3::getUUID() const {
  if (natter_minhcast_) return natter_minhcast_->getUUID();
  return solanet::UUID{};
}

void NatterNodeNs3::publish(const std::string &topic, const std::string &msg) {
  solanet::UUID res;
  if (natter_minhcast_) res = natter_minhcast_->publish(topic, msg);

  logger_->logNatterEvent(2, res);
}

void NatterNodeNs3::subscribeTopic(const std::string &topic, uint32_t own_level,
                                   uint32_t own_number, uint32_t own_fanout) {
  if (natter_minhcast_)
    natter_minhcast_->subscribeTopic(topic, {{own_level, own_number, own_fanout}});
}

void NatterNodeNs3::unsubscribeTopic(const std::string &topic) {
  if (natter_minhcast_) natter_minhcast_->unsubscribeTopic(topic);
}

}  // namespace daisi::natter_ns3
