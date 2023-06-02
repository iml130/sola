// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_CORE_NETWORK_FACADE_H_
#define NATTER_CORE_NETWORK_FACADE_H_

#include "natter/network_info_ipv4.h"
#include "solanet/network_udp/network_udp.h"
#include "solanet/serializer/serializer.h"

namespace natter::core {
/**
 * Facade to abstract serialization and networking
 */
template <typename T> class NetworkFacade {
public:
  explicit NetworkFacade(std::function<void(const T &)> recv_fct)
      : network_(
            [this](auto &&message) { processMessage(std::forward<decltype(message)>(message)); }),
        recv_fct_(std::move(recv_fct)) {}

  void send(const NetworkInfoIPv4 &net_info, const T &message) {
    network_.send({net_info.ip, net_info.port, solanet::serializer::serialize<T>(message)});
  }

  NetworkInfoIPv4 getNetworkInfo() const { return {network_.getIP(), network_.getPort()}; }

private:
  void processMessage(const solanet::Message &msg) {
    recv_fct_(solanet::serializer::deserialize<T>(msg.getMessage()));
  }
  solanet::Network network_;
  std::function<void(const T &)> recv_fct_;
};
}  // namespace natter::core

#endif  // NATTER_CORE_NETWORK_FACADE_H_
