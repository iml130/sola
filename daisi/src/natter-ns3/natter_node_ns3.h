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

#ifndef DAISI_NATTER_NS3_NODE_NS3_H_
#define DAISI_NATTER_NS3_NODE_NS3_H_

#include "natter/natter_minhcast.h"
#include "natter_logger_ns3.h"
#include "natter_mode.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "solanet/uuid.h"

namespace daisi::natter_ns3 {

// Wrapper class
class NatterNodeNs3 : public ns3::Object {
public:
  NatterNodeNs3() = default;
  NatterNodeNs3(std::shared_ptr<natter::logging::NatterLoggerNs3> logger, NatterMode mode);

  static ns3::TypeId GetTypeId();
  ns3::TypeId GetInstanceTypeId() const override;

  // Natter functions wrapper
  void addPeer(const std::string &topic, solanet::UUID uuid, const std::string &ip, uint16_t port,
               uint32_t level, uint32_t number, uint32_t fanout);
  void removePeer(const std::string &topic, const std::string &uuid);

  solanet::UUID getUUID() const;

  void publish(const std::string &topic, const std::string &msg);
  void subscribeTopic(const std::string &topic, uint32_t own_level, uint32_t own_number,
                      uint32_t own_fanout);
  void unsubscribeTopic(const std::string &topic);

  natter::NetworkInfoIPv4 getNetworkInfo() const {
    if (!natter_minhcast_) throw std::runtime_error("natter not initialized");
    return natter_minhcast_->getNetworkInfo();
  }

private:
  std::unique_ptr<natter::minhcast::NatterMinhcast> natter_minhcast_;
  std::shared_ptr<natter::logging::NatterLoggerNs3> logger_;
};
}  // namespace daisi::natter_ns3
#endif
