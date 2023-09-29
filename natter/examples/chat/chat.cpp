// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <cassert>
#include <iostream>

#include "natter/minhcast_level_number.h"
#include "natter/natter_minhcast.h"
#include "solanet/uuid.h"

static constexpr uint32_t kFanout = 2;

// See README.md for more information

/**
 * Ask user to enter others connection ID
 * @return others deserialized connection ID
 */
std::tuple<std::string, uint16_t, natter::minhcast::LevelNumber> connectionInput() {
  std::string connection_string;
  std::cout << "Enter others connection string: ";
  std::cin >> connection_string;

  std::string ip = connection_string.substr(0, connection_string.find(';'));
  connection_string =
      connection_string.substr(connection_string.find(';') + 1, connection_string.size());
  uint16_t port = std::stoi(connection_string.substr(0, connection_string.find(';')));
  connection_string =
      connection_string.substr(connection_string.find(';') + 1, connection_string.size());
  uint32_t level = std::stoi(connection_string.substr(0, connection_string.find(';')));
  uint32_t number = std::stoi(
      connection_string.substr(connection_string.find(';') + 1, connection_string.size()));

  return {ip, port, {level, number, kFanout}};
}

natter::minhcast::LevelNumber ownNodeData() {
  uint32_t level = 0;
  uint32_t number = 0;
  std::cout << "Enter your node level (with fanout " << kFanout << "): ";
  std::cin >> level;
  std::cout << std::endl << "Enter your node number (with fanout " << kFanout << "): ";
  std::cin >> number;
  std::cout << std::endl << "---" << std::endl;
  return {level, number, kFanout};
}

namespace natter::example {
class Application {
public:
  Application()
      : natter_(
            [](const natter::Message &msg) {
              std::cout << "[" << solanet::uuidToString(msg.sender_id) << "]: " << msg.content
                        << std::endl;
            },
            [](const std::string & /*missing_msg_id*/) { /* intentionally not implemented */ }) {}

  void run() {
    static const std::string topic = "myTopic";

    std::cout << "Starting chat" << std::endl;
    natter::minhcast::LevelNumber own_node_data = ownNodeData();
    const NetworkInfoIPv4 own_network = natter_.getNetworkInfo();
    std::cout << "Your connection ID: " << own_network.ip << ";" << own_network.port << ";"
              << std::get<0>(own_node_data) << ";" << std::get<1>(own_node_data) << std::endl;
    minhcast::MinhcastNodeInfo own_node{own_node_data, {}};  // Only position needed
    natter_.subscribeTopic(topic, own_node);

    std::cout << "Use :connect to connect to another peer" << std::endl;
    std::cout << "Use :quit to exit" << std::endl;
    std::cout << "------------" << std::endl;

    for (std::string line; std::getline(std::cin, line);) {
      std::cout << ">> ";
      if (line.length() == 0) continue;
      if (line == ":quit") break;
      if (line == ":connect") {
        auto [ip, port, tree_pos] = connectionInput();

        minhcast::MinhcastNodeInfo info{tree_pos, {ip, port}};
        natter_.addPeer(topic, info);
      } else {
        natter_.publish(topic, line);
      }
    }
  }

private:
  natter::minhcast::NatterMinhcast natter_;
};
}  // namespace natter::example

int main(int /*argc*/, char ** /*argv*/) {
  natter::example::Application app;
  app.run();
}
