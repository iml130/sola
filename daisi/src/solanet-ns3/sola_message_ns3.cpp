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

#include "sola_message_ns3.h"

using namespace ns3;

namespace daisi::solanet_ns3 {

SolaMessageNs3::SolaMessageNs3() = default;

TypeId SolaMessageNs3::GetTypeId() {
  static TypeId tid =
      TypeId("ns3::SolaMessageNs3").SetParent<Header>().AddConstructor<SolaMessageNs3>();
  return tid;
}

TypeId SolaMessageNs3::GetInstanceTypeId() const { return GetTypeId(); }

void SolaMessageNs3::Print(std::ostream &os) const {
  // intentionally not implemented
}

uint32_t SolaMessageNs3::GetSerializedSize() const {
  return ip_.length() + std::to_string(port_).length() + payload_.length() + 3;
}

void SolaMessageNs3::Serialize(Buffer::Iterator start) const {
  std::stringstream stream;
  // We always prepend our own connection string we are listening on
  stream << ip_ << ";";
  stream << port_ << ";";
  stream << payload_ << ";";
  start.Write((uint8_t *)stream.str().c_str(), stream.str().size());
}

uint32_t SolaMessageNs3::Deserialize(Buffer::Iterator start) {
  uint32_t len = start.GetRemainingSize();

  // make sure to init the temp array with 0 values
  std::vector<char> temp_buffer;
  temp_buffer.resize(len);
  start.Read((uint8_t *)temp_buffer.data(), len);

  // Copying the buffer into msg in c-style. Some payload data may contain NULL characters within
  // the string...
  std::string msg;
  msg.resize(len);
  memset(msg.data(), 0, len);
  memcpy(msg.data(), temp_buffer.data(), len);

  // Deserialize into class members
  ip_ = msg.substr(0, msg.find(";"));
  msg = msg.substr(msg.find(";") + 1, msg.size());
  port_ = std::stoi(msg.substr(0, msg.find(";")));
  msg = msg.substr(msg.find(";") + 1, msg.size());
  msg.erase(msg.end() - 1);
  payload_ = msg;

  return this->GetSerializedSize();
}

void SolaMessageNs3::setPayload(const std::string &payload) { payload_ = payload; }
std::string SolaMessageNs3::getPayload() const { return payload_; }

void SolaMessageNs3::setIp(const std::string &ip) { ip_ = ip; }
std::string SolaMessageNs3::getIp() const { return ip_; }

void SolaMessageNs3::setPort(uint16_t port) { port_ = port; }
uint16_t SolaMessageNs3::getPort() const { return port_; }

}  // namespace daisi::solanet_ns3
