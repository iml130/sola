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

#ifndef DAISI_SOLANET_NS3_SOLA_MESSAGE_NS3_H_
#define DAISI_SOLANET_NS3_SOLA_MESSAGE_NS3_H_

#include <string>
#include <tuple>
#include <vector>

#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"

namespace daisi::solanet_ns3 {

class SolaMessageNs3 : public ns3::Header {
public:
  SolaMessageNs3();

  static ns3::TypeId GetTypeId();
  ns3::TypeId GetInstanceTypeId() const;

  void Print(std::ostream &os) const;
  uint32_t GetSerializedSize() const;
  void Serialize(ns3::Buffer::Iterator start) const;
  uint32_t Deserialize(ns3::Buffer::Iterator start);

  void setPayload(const std::string &payload);
  std::string getPayload() const;

  void setIp(const std::string &ip);
  std::string getIp() const;

  void setPort(uint16_t port);
  uint16_t getPort() const;

private:
  std::string payload_;
  std::string ip_;
  uint16_t port_ = 0;
};
}  // namespace daisi::solanet_ns3
#endif
