// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <iostream>
#include <string>
#include <thread>

#include "solanet/network_udp/message.h"
#include "solanet/network_udp/network_udp.h"

// called when a message is received
void receiving(const solanet::Message &msg) {
  std::cout << "RECEIVED FROM " << msg.getIp() << " MESSAGE '" << msg.getMessage() << "'"
            << std::endl;
}

int main() {
  // Start solanet instance. It will listen on all IPs and a randomly selected free port
  solanet::Network network(receiving);
  std::cout << "STARTED NETWORK ON " << network.getIP() << ":" << network.getPort() << std::endl;

  // Send Hello-world message to ourself
  solanet::Message msg(network.getIP(), network.getPort(), "Hello World!");
  network.send(msg);

  // Delay, waiting to receive the message
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(2s);
}
