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

#ifndef DAISI_NETWORK_TCP_FRAMING_MANAGER_H_
#define DAISI_NETWORK_TCP_FRAMING_MANAGER_H_

#include <cstdint>
#include <deque>
#include <optional>
#include <string>

namespace daisi::network_tcp {

class FramingManager {
public:
  void processNewData(const std::string &msg);

  bool hasPackets() const;

  std::string nextPacket();

  static std::string frameMsg(const std::string &msg);

private:
  /// Extract length prefix from \p msg, starting at \p next_offset
  static uint32_t readPacketSize(const std::string &msg, uint32_t next_offset);

  /// Process a new packet contained in \p msg, starting at \p next_offset
  uint32_t handleNewPacket(const std::string &msg, uint32_t next_offset);

  /// Process content of \p msg into the current inflight/processing packet.
  /// \p next_offset should contain the index of the first PAYLOAD byte (not the length prefix)
  uint32_t readPacket(const std::string &msg, uint32_t next_offset);

  std::deque<std::string> outstanding_packets_;

  struct InflightPacket {
    std::string packet;  /// payload
    size_t remaining_size;
  };

  std::optional<InflightPacket> inflight_;  /// The current inflight packet
};
}  // namespace daisi::network_tcp

#endif  // DAISI_NETWORK_TCP_FRAMING_MANAGER_H_
