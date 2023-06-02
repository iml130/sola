// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_ENDPOINT_H_
#define SOLA_ENDPOINT_H_

#include <cstdint>
#include <string>
#include <utility>

namespace sola {
struct Endpoint {
  std::string ip{};
  uint16_t port{};
  Endpoint() = default;
  Endpoint(std::string ip, uint16_t port) : ip(std::move(ip)), port(port) {}

  explicit Endpoint(const std::string &endpoint)
      : ip(endpoint.substr(0, endpoint.find(":"))),
        port(std::stoi(endpoint.substr(endpoint.find(":") + 1, endpoint.size()))) {}
};
}  // namespace sola

#endif
