// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_NETWORK_INFO_IPV4_H_
#define NATTER_NETWORK_INFO_IPV4_H_

#include <cstdint>
#include <string>

namespace natter {
struct NetworkInfoIPv4 {
  std::string ip;
  uint16_t port = 0;

  bool operator==(const NetworkInfoIPv4 &other) const {
    return ip == other.ip && port == other.port;
  }
};
}  // namespace natter

#endif  // DAISI_NETWORK_INFO_IPV4_H_
