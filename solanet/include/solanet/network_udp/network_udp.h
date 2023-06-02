// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLANET_NETWORK_UDP_NETWORK_H_
#define SOLANET_NETWORK_UDP_NETWORK_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "message.h"

namespace solanet {
class Network {
public:
  /**
   * Create network interface
   * @param callback function, which is called asynchronously when a new message is received
   * @param listening_port UDP port to listen on
   */
  explicit Network(const std::function<void(const Message &)> &callback);

  Network(const std::string &ip, const std::function<void(const Message &)> &callback);

  ~Network();

  // Forbid copy/move operations
  Network(const Network &) = delete;
  Network &operator=(const Network &) = delete;
  Network(const Network &&) = delete;
  Network &operator=(Network &&) = delete;

  /**
   * Send message
   * @param msg message to send
   */
  void send(const Message &msg);

  uint16_t getPort() const;

  std::string getIP() const;

private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;
};
}  // namespace solanet

#endif  // SOLANET_NETWORK_UDP_NETWORK_H_
