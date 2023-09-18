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

#include "framing_manager.h"

#include <arpa/inet.h>

#include <cstring>
#include <stdexcept>

#include "utils/daisi_check.h"

namespace daisi::network_tcp {
std::string FramingManager::frameMsg(const std::string &msg) {
  // Prefix msg with 4 byte packet length in network byte order
  const uint32_t size = htonl(msg.size());

  std::string data;
  data.resize(msg.size() + 4);

  std::memcpy(data.data(), &size, 4);
  std::memcpy(data.data() + 4, msg.data(), msg.size());
  return data;
}

uint32_t FramingManager::readPacket(const std::string &msg, uint32_t next_offset) {
  DAISI_CHECK(inflight_, "No packet currently in processing state");

  const size_t available = msg.size() - next_offset;

  const size_t read = std::min(available, inflight_->remaining_size);

  DAISI_CHECK(next_offset + read <= msg.size(), "Read out of bounds of msg");

  const uint32_t current_pos = inflight_->packet.size() - inflight_->remaining_size;

  std::memcpy(inflight_->packet.data() + current_pos, msg.data() + next_offset, read);
  inflight_->remaining_size -= read;

  if (inflight_->remaining_size == 0) {
    outstanding_packets_.push_back(std::move(inflight_->packet));
    inflight_.reset();
  }

  return next_offset + read;
}

uint32_t FramingManager::handleNewPacket(const std::string &msg, uint32_t next_offset) {
  DAISI_CHECK(!inflight_, "Previous packet not finished yet");

  const uint32_t current_packet_size = readPacketSize(msg, next_offset);

  InflightPacket packet;
  packet.remaining_size = current_packet_size;
  packet.packet.resize(current_packet_size);

  inflight_ = std::move(packet);

  return readPacket(msg, next_offset + 4);
}

void FramingManager::processNewData(const std::string &msg) {
  size_t next = 0;
  while (next < msg.size()) {
    if (inflight_) {
      next = readPacket(msg, next);
    } else {
      next = handleNewPacket(msg, next);
    }
  }
}

uint32_t FramingManager::readPacketSize(const std::string &msg, uint32_t next_offset) {
  DAISI_CHECK(next_offset + 4 <= msg.size(),
              "Read out of bounds: Message splitted in length delimiter. Not supported yet!");

  uint32_t current_packet_size = 0;
  std::memcpy(&current_packet_size, msg.data() + next_offset, 4);
  return ntohl(current_packet_size);
}

bool FramingManager::hasPackets() const { return !outstanding_packets_.empty(); }

std::string FramingManager::nextPacket() {
  DAISI_CHECK(hasPackets(), "No packets available");

  std::string msg = outstanding_packets_.front();
  outstanding_packets_.pop_front();
  return msg;
}

}  // namespace daisi::network_tcp
