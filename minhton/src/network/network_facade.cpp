// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/network/network_facade.h"

#include <utility>

namespace minhton {

NetworkFacade::NetworkFacade(std::function<void(const MessageVariant &msg)> recv_fct,
                             const std::string &ip)
    : network_(
          ip, [this](auto &&message) { processMessage(std::forward<decltype(message)>(message)); }),
      recv_fct_(std::move(recv_fct)) {}

void NetworkFacade::send(const MessageVariant &msg) {
  MinhtonMessageHeader header =
      std::visit([](auto &&arg) -> MinhtonMessageHeader { return arg.getHeader(); }, msg);
  network_.send(
      {header.getTarget().getAddress(), header.getTarget().getPort(), serializer_.serialize(msg)});
}

std::string NetworkFacade::getIP() const { return network_.getIP(); }

uint16_t NetworkFacade::getPort() const { return network_.getPort(); }

void NetworkFacade::processMessage(const solanet::Message &msg) {
  auto deserialized_msg = serializer_.deserialize(msg.getMessage());
  recv_fct_(deserialized_msg);
}

}  // namespace minhton
