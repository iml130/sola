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

#include "network_tcp/framing_manager.h"

#include <arpa/inet.h>

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <random>

using namespace daisi::network_tcp;

TEST_CASE("Frame message", "[frame_message]") {
  const std::string a = "ABCDEF";

  const std::string framed = FramingManager::frameMsg(a);

  REQUIRE(framed.size() == 10);
  REQUIRE(framed.substr(4) == a);

  const std::string prefix = framed.substr(0, 4);
  const uint32_t *x = (const uint32_t *)(prefix.data());
  REQUIRE(*x == htonl(6));
}

TEST_CASE("Frame empty message", "[frame_empty_message]") {
  const std::string a = "";

  const std::string framed = FramingManager::frameMsg(a);

  REQUIRE(framed.size() == 4);

  const std::string prefix = framed.substr(0);
  const uint32_t *x = (const uint32_t *)(prefix.data());
  REQUIRE(*x == htonl(0));
}

TEST_CASE("Process full messages", "[process_full_messages]") {
  FramingManager manager;
  REQUIRE(!manager.hasPackets());

  std::string a = "ABCDEF";
  std::string framed1 = FramingManager::frameMsg(a);

  manager.processNewData(framed1);

  REQUIRE(manager.hasPackets());
  REQUIRE(manager.nextPacket() == a);
  REQUIRE(!manager.hasPackets());

  std::string b = "XYZ";
  std::string framed2 = FramingManager::frameMsg(b);

  manager.processNewData(framed2);
  REQUIRE(manager.hasPackets());
  REQUIRE(manager.nextPacket() == b);
  REQUIRE(!manager.hasPackets());

  manager.processNewData(framed2);
  manager.processNewData(framed1);
  REQUIRE(manager.hasPackets());
  REQUIRE(manager.nextPacket() == b);
  REQUIRE(manager.hasPackets());
  REQUIRE(manager.nextPacket() == a);
  REQUIRE(!manager.hasPackets());
}

TEST_CASE("Process splitted messages", "[process_splitted_messages]") {
  FramingManager manager;
  REQUIRE(!manager.hasPackets());

  std::string a = "ABCDEF";
  std::string framed1 = FramingManager::frameMsg(a);

  std::string sub1 = framed1.substr(0, 6);
  std::string sub2 = framed1.substr(6, 2);
  std::string sub3 = framed1.substr(8);

  manager.processNewData(sub1);
  REQUIRE(!manager.hasPackets());

  manager.processNewData(sub2);
  REQUIRE(!manager.hasPackets());

  manager.processNewData(sub3);
  REQUIRE(manager.hasPackets());
  REQUIRE(manager.nextPacket() == a);
  REQUIRE(!manager.hasPackets());

  std::string b = "XYZ";
  std::string framed2 = FramingManager::frameMsg(b);

  std::string sub4 = framed2.substr(0, 5);
  std::string sub5 = framed2.substr(5);

  manager.processNewData(sub4);
  REQUIRE(!manager.hasPackets());
  REQUIRE_THROWS(manager.nextPacket());

  manager.processNewData(sub5);
  REQUIRE(manager.hasPackets());
  REQUIRE(manager.nextPacket() == b);
  REQUIRE(!manager.hasPackets());

  manager.processNewData(framed1 + sub4);
  REQUIRE(manager.hasPackets());
  REQUIRE(manager.nextPacket() == a);
  REQUIRE(!manager.hasPackets());

  manager.processNewData(sub5);
  REQUIRE(manager.hasPackets());
  REQUIRE(manager.nextPacket() == b);
  REQUIRE(!manager.hasPackets());
}
