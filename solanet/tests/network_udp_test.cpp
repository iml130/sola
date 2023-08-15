// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "solanet/network_udp/network_udp.h"

#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

/**
 * System test for "real" network interface (no mocking)
 */

using namespace solanet;
using namespace std::chrono_literals;

void checkMessage(const std::string &sender_ip, const std::string &message_send,
                  const Message &second) {
  REQUIRE(sender_ip == second.getIp());
  REQUIRE(message_send == second.getMessage());
}

// Basic check if IP string contains four number blocks with numbers <= 255
void checkValidityIPv4(std::string ip) {
  int first = std::stoi(ip.substr(0, ip.find('.')));
  ip = ip.substr(ip.find('.') + 1, ip.size());
  int second = std::stoi(ip.substr(0, ip.find('.')));
  ip = ip.substr(ip.find('.') + 1, ip.size());
  int third = std::stoi(ip.substr(0, ip.find('.')));
  ip = ip.substr(ip.find('.') + 1, ip.size());
  REQUIRE(ip.find('.') == ip.npos);
  REQUIRE(ip.length() <= 3);
  int fourth = std::stoi(ip);
  REQUIRE(first <= 255);
  REQUIRE(second <= 255);
  REQUIRE(third <= 255);
  REQUIRE(fourth <= 255);
}

TEST_CASE("[NETWORK_UDP] Automatically fetch IP", "Automatically fetch IP") {
  Network network1("", [&](const Message &) {});
  checkValidityIPv4(network1.getIP());

  Network network2("", [&](const Message &) {});
  checkValidityIPv4(network2.getIP());

  REQUIRE(network1.getPort() != 0);
  REQUIRE(network2.getPort() != 0);
  REQUIRE(network1.getPort() != network2.getPort());
}

TEST_CASE("[NETWORK_UDP] Single send/receive", "Single send/receive") {
  std::vector<Message> received_msgs1, received_msgs2;
  Network network1("127.0.0.1", [&](const Message &msg) { received_msgs1.push_back(msg); });
  Network network2("127.0.0.1", [&](const Message &msg) { received_msgs2.push_back(msg); });

  // Send from network1 to network2 and vice versa
  Message m1("127.0.0.1", network2.getPort(), "From network1 to network2");
  Message m2("127.0.0.1", network1.getPort(), "From network2 to network1");
  network1.send(m1);
  network2.send(m2);

  std::this_thread::sleep_for(1s);
  REQUIRE(received_msgs1.size() == 1);
  checkMessage("127.0.0.1", m1.getMessage(), received_msgs2[0]);
  REQUIRE(received_msgs2.size() == 1);
  checkMessage("127.0.0.1", m2.getMessage(), received_msgs1[0]);
}

TEST_CASE("[NETWORK_UDP] Multiple send/receive", "Multiple send/receive") {
  std::vector<Message> received_msgs1;
  Network network1("127.0.0.1", [&](const Message &msg) { received_msgs1.push_back(msg); });
  Network network2("127.0.0.1", [&](const Message &) {});

  // Send from network2 to network 1
  constexpr int message_count = 50;
  for (int i = 0; i < message_count; i++) {
    Message m("127.0.0.1", network1.getPort(), std::to_string(i));
    network2.send(m);
  }

  std::this_thread::sleep_for(2s);
  REQUIRE(received_msgs1.size() == message_count);
  for (int i = 0; i < message_count; i++) {
    checkMessage("127.0.0.1", std::to_string(i), received_msgs1[i]);
  }
}
