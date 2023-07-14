// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_MESSAGE_BOOTSTRAP_DISCOVER_H_
#define MINHTON_MESSAGE_BOOTSTRAP_DISCOVER_H_

#include "minhton/message/message.h"
#include "minhton/message/serialize.h"

namespace minhton {
/// A node wants to join the network but does not have access to a node's network information to
/// send the initial Join message to. Therefore a Bootstrap Discover gets send through a Multicast
/// to discover a node's network information.
class MessageBootstrapDiscover : public MinhtonMessage<MessageBootstrapDiscover> {
public:
  explicit MessageBootstrapDiscover(MinhtonMessageHeader header,
                                    std::string discovery_msg = "Hello");

  std::string getDiscoveryMessage() const;

  SERIALIZE(header_, discovery_msg_);

  MessageBootstrapDiscover() = default;

protected:
  friend MinhtonMessage;

  /// The header contains always required fields like the sender and target
  MinhtonMessageHeader header_;

  /// Checks if the message was constructed with all of the necessary information
  bool validateImpl() const;

private:
  std::string discovery_msg_;
};
}  // namespace minhton

#endif
