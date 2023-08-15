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

#include "packet.h"

#include <cassert>
#include <iomanip>

using namespace ns3;

namespace daisi::cpps {

CppsTCPMessage::CppsTCPMessage() = default;

TypeId CppsTCPMessage::GetTypeId() {
  static TypeId tid =
      TypeId("ns3::CppsTCPMessage").SetParent<Header>().AddConstructor<CppsTCPMessage>();
  return tid;
}

TypeId CppsTCPMessage::GetInstanceTypeId() const { return GetTypeId(); }

void CppsTCPMessage::Print(std::ostream &) const {
  // intentionally not implemented
}

uint32_t CppsTCPMessage::GetSerializedSize() const {
  uint32_t size = 0;
  for (auto &msg : messages_) {
    size += msg.payload.length() + sizeof(msg.type) + 4;
  }
  return size;
}

void CppsTCPMessage::Serialize(Buffer::Iterator start) const {
  std::string message{};
  for (const FieldMessageCpps &msg : messages_) {
    std::string payload = msg.payload;
    payload.insert(0, std::to_string(msg.type));
    assert(payload.length() < 10000);
    std::stringstream stream;
    stream << std::setw(4) << std::setfill('0') << std::to_string(payload.length());
    message += stream.str();
    message += payload;
  }

  start.Write(reinterpret_cast<const uint8_t *>(message.c_str()), message.size());
}

uint32_t CppsTCPMessage::Deserialize(Buffer::Iterator start) {
  uint32_t len = start.GetRemainingSize();

  // make sure to init the temp array with 0 values
  std::vector<char> temp_buffer;
  temp_buffer.resize(len);
  start.Read(reinterpret_cast<uint8_t *>(temp_buffer.data()), len);

  // Copying the buffer into payload_ in c-style. Some payload data may contain NULL characters
  // within the string...
  std::string message;
  message.resize(len);
  memset(message.data(), 0, len);
  memcpy(message.data(), temp_buffer.data(), len);
  while (!message.empty()) {
    uint32_t length = std::stoi(message.substr(0, 4));
    message = message.substr(4, message.length());

    std::string msg = message.substr(0, length);
    message = message.substr(length, message.length());

    FieldMessageCpps m;
    m.type = msg.at(0) - 48;
    m.payload = msg.substr(1, msg.size());
    messages_.push_back(m);
  }

  return this->GetSerializedSize();
}

void CppsTCPMessage::addMessage(const FieldMessageCpps &msg) { messages_.push_back(msg); }

std::vector<FieldMessageCpps> CppsTCPMessage::getMessages() const { return messages_; }

}  // namespace daisi::cpps
