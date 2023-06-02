// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_JOIN_MINHTON_JOIN_ALGORITHM_H_
#define MINHTON_ALGORITHMS_JOIN_MINHTON_JOIN_ALGORITHM_H_

#include <memory>
#include <optional>

#include "minhton/algorithms/find_end/minhton_find_end_algorithm.h"
#include "minhton/algorithms/join/join_algorithm_general.h"
#include "minhton/message/find_end.h"

namespace minhton {

class MinhtonJoinAlgorithm : public JoinAlgorithmGeneral {
public:
  explicit MinhtonJoinAlgorithm(std::shared_ptr<AccessContainer> access)
      : JoinAlgorithmGeneral(access), find_end_helper_(access, true){};

  /// This method will be called when we receive a JOIN message.
  ///
  /// Forwarding JOIN Messages according to the fill join algorithm.
  ///
  /// When this node is a fitting position to accept the entering node,
  /// it will call initialize the JoinAccept procedure.
  /// Otherwise it will further forward the message to the next node.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processJoin(incoming_message);
  /// \endcode
  ///
  /// \param msg the message we received and want to process
  ///
  /// \throw Algorithm Exception if join case couldn't be handled
  void processJoin(const MessageJoin &msg) override;

  /// Helper method for the perform accept child procedure.
  ///
  /// Sending UPDATE_ROUTING_TABLE_NEIGHBOR_CHILD messages with the entering_node as the node to
  /// inform to each of our routing table neighbors, because those nodes have the entering_node as a
  /// routing table neighbor child.
  ///
  /// Typical Usage:
  /// \code
  ///   this->performSendUpdateNeighborMessagesAboutEnteringNode(entering_node);
  /// \endcode
  ///
  /// \param entering_node the node who wants to enter the network
  ///
  uint32_t performSendUpdateNeighborMessagesAboutEnteringNode(
      minhton::NodeInfo entering_node) override;

private:
  MinhtonFindEndAlgorithm find_end_helper_;
};

}  // namespace minhton

#endif
