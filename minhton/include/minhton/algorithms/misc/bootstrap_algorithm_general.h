// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_JOIN_BOOTSTRAP_ALGORITHM_GENERAL_H_
#define MINHTON_ALGORITHMS_JOIN_BOOTSTRAP_ALGORITHM_GENERAL_H_

#include <memory>

#include "minhton/algorithms/misc/interface_bootstrap_algorithm.h"
#include "minhton/message/bootstrap_discover.h"
#include "minhton/message/bootstrap_response.h"
#include "minhton/message/join.h"
#include "minhton/message/message.h"

namespace minhton {

class BootstrapAlgorithmGeneral : public BootstrapAlgorithmInterface {
public:
  explicit BootstrapAlgorithmGeneral(std::shared_ptr<AccessContainer> access)
      : BootstrapAlgorithmInterface(access){};

  void process(const MessageVariant &msg) override;

  /// Initiating a join without knowing an address to join to,
  /// but only a multicast address.
  ///
  /// \param p_node_info multicast address to send bootstrap discover to
  void initiateJoin(const PhysicalNodeInfo &p_node_info) override;

  /// This method will be called when we receive a MessageBootstrapDiscover message.
  ///
  /// The UDP Multicast listeners creates and object and forwards it.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processBootstrapDiscover(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processBootstrapDiscover(const minhton::MessageBootstrapDiscover &msg);

  /// This method will be called when we receive a MessageBootstrapResponse message.
  ///
  /// Once, a discovery multicast message has been processed, this is the repsonse.
  /// The new node which tries to join receieves information regarding where to join at.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processBootstrapResponse(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processBootstrapResponse(const minhton::MessageBootstrapResponse &msg);

  /// This method will be called, after the bootstrap response timeout has been expired.
  ///
  /// Choosing the best node from all bootstrap response senders
  /// and sending a Join message to the best of those.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processBootstrapResponseTimeout();
  /// \endcode
  void processBootstrapResponseTimeout() override;

  /// Check after the bootstrap response timeout has been called
  /// whether the gathered responses are valid and the node has enough
  /// information to find a good target to send the first
  /// join message to.
  bool isBootstrapResponseValid() const override;

private:
  /// Storage of all senders of BootstrapResponse senders
  /// to choose the best sender from afterwards.
  std::vector<minhton::NodeInfo> bootstrap_response_senders_;
};

}  // namespace minhton

#endif
