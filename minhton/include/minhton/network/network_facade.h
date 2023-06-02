// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_NETWORK_FACADE_H_
#define MINHTON_NETWORK_FACADE_H_

#include "minhton/message/message.h"
#include "minhton/message/types_all.h"
#include "minhton/utils/serializer_cereal.h"
#include "solanet/network_udp/network_udp.h"

namespace minhton {
/**
 * Facade to abstract serialization and networking
 */
class NetworkFacade {
public:
  NetworkFacade(std::function<void(const MessageVariant &msg)> recv_fct, const std::string &ip);
  void send(const MessageVariant &msg);

  std::string getIP() const;
  uint16_t getPort() const;

private:
  void processMessage(const solanet::Message &msg);

  minhton::serializer::SerializerCereal serializer_;

  solanet::Network network_;
  std::function<void(const MessageVariant &msg)> recv_fct_;
};
}  // namespace minhton

#endif  // NATTER_CORE_NETWORK_FACADE_H_
