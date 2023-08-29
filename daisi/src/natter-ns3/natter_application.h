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

#ifndef DAISI_NATTER_NS3_NATTER_APPLICATION_H_
#define DAISI_NATTER_NS3_NATTER_APPLICATION_H_

#include "natter/natter.h"
#include "natter/natter_minhcast.h"
#include "natter_logger_ns3.h"
#include "natter_mode.h"
#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/simulator.h"
#include "ns3/socket.h"
#include "ns3/uinteger.h"

namespace daisi::natter_ns3 {

/// @brief Wrapper to run natter as a ns-3 application.
/// Only MINHCAST is supported at the moment!
class NatterApplication : public ns3::Application {
public:
  static ns3::TypeId GetTypeId();
  NatterApplication() = default;

  void setMode(NatterMode mode);

  void setLevelNumber(std::pair<uint32_t, uint32_t> level_number);
  std::pair<uint32_t, uint32_t> getLevelNumber() const { return level_number_; }

  // Natter functions wrapper
  void addPeer(const std::string &topic, solanet::UUID uuid, const std::string &ip, uint16_t port,
               uint32_t level, uint32_t number, uint32_t fanout);
  void removePeer(const std::string &topic, const std::string &uuid);

  solanet::UUID getUUID() const;

  void publish(const std::string &topic, const std::string &msg);
  void subscribeTopic(const std::string &topic, uint32_t own_level, uint32_t own_number,
                      uint32_t own_fanout);
  void unsubscribeTopic(const std::string &topic);

  natter::NetworkInfoIPv4 getNetworkInfo() const;

private:
  void StartApplication() final;

  void logSelfToDB(std::pair<uint32_t, uint32_t> level_number);

  std::unique_ptr<natter::minhcast::NatterMinhcast> natter_minhcast_;

  std::shared_ptr<natter::logging::NatterLoggerNs3> logger_;

  ns3::Ptr<ns3::Socket> socket_;

  std::pair<uint32_t, uint32_t> level_number_;

  NatterMode mode_ = NatterMode::kNone;
};
}  // namespace daisi::natter_ns3
#endif
