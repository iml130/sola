// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLANET_NETWORK_UDP_MESSAGE_H_
#define SOLANET_NETWORK_UDP_MESSAGE_H_

#include <string>
#include <utility>

namespace solanet {
class Message {
public:
  // Creates empty message
  Message() = default;

  Message(std::string ip, uint16_t port, std::string message)
      : ip_(std::move(ip)), port_(port), message_(std::move(message)){};

  Message(const std::string &endpoint, std::string message)
      : ip_(endpoint.substr(0, endpoint.find(":"))),
        port_(std::stoi(endpoint.substr(endpoint.find(":") + 1, endpoint.size()))),
        message_(std::move(message)) {}

  /**
   * Check if message is treated as empty
   * @return true if message is empty
   */
  bool isEmpty() const { return ip_.empty() || message_.empty(); }

  /**
   * Returns message content
   * @return message content
   */
  std::string getMessage() const { return message_; }

  /**
   * Returns IPv4 destination address
   * @return IPv4 destination address
   */
  std::string getIp() const { return ip_; }

  /**
   * Returns UDP destination port
   * @return UDP destination port
   */
  uint16_t getPort() const { return port_; }

private:
  std::string ip_{};
  uint16_t port_ = 0;
  std::string message_{};
};
}  // namespace solanet

#endif  // SOLANET_NETWORK_UDP_MESSAGE_H_
