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

#ifndef DAISI_CPPS_PACKET_H_
#define DAISI_CPPS_PACKET_H_

#include <string>
#include <tuple>
#include <vector>

#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"

namespace daisi::cpps {

struct FieldMessageCpps {
  std::string payload;
  uint8_t type;
};

class CppsTCPMessage : public ns3::Header {
public:
  CppsTCPMessage();

  static ns3::TypeId GetTypeId();
  ns3::TypeId GetInstanceTypeId() const override;

  void Print(std::ostream &os) const override;
  uint32_t GetSerializedSize() const override;
  void Serialize(ns3::Buffer::Iterator start) const override;
  uint32_t Deserialize(ns3::Buffer::Iterator start) override;

  void addMessage(const FieldMessageCpps &msg);
  std::vector<FieldMessageCpps> getMessages() const;

private:
  std::vector<FieldMessageCpps> messages_;
};
}  // namespace daisi::cpps
#endif
