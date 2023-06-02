// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_MISC_RESPONSE_ALGORITHM_GENERAL_H_
#define MINHTON_ALGORITHMS_MISC_RESPONSE_ALGORITHM_GENERAL_H_

#include <memory>

#include "minhton/algorithms/misc/interface_response_algorithm.h"
#include "minhton/message/get_neighbors.h"
#include "minhton/message/inform_about_neighbors.h"
#include "minhton/message/message.h"
#include "minhton/message/remove_neighbor.h"
#include "minhton/message/update_neighbors.h"

namespace minhton {

class ResponseAlgorithmGeneral : public ResponseAlgorithmInterface {
public:
  explicit ResponseAlgorithmGeneral(std::shared_ptr<AccessContainer> access)
      : ResponseAlgorithmInterface(access){};
  void process(const MessageVariant &msg) override;

private:
  /// This method will be called when we receive a UPDATE_NEIGHBORS message.
  ///
  /// A node wants to inform us about a change in our routing information.
  /// We either got a new neighbor or a neighbor got replaced by a different one.
  /// We will update our neighbors accordingly by using appropriate routing information methods.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processUpdateNeighbors(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processUpdateNeighbors(const MessageUpdateNeighbors &msg);

  /// This method will be called when we receive a REMOVE_NEIGHBOR message.
  ///
  /// A node wants to inform us about a change in our routing information.
  /// We remove the given neighbor by its logical position.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processRemoveNeighbor(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processRemoveNeighbor(const MessageRemoveNeighbor &msg);

  /// This method will be called when we receive a REMOVE_AND_UPDATE_NEIGHBOR message.
  ///
  /// A node wants to inform us about a change in our routing information, affecting both removed
  /// and updated positions.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processRemoveAndUpdateNeighbors(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processRemoveAndUpdateNeighbors(const MessageRemoveAndUpdateNeighbors &msg);

  /// This method will be called when we receive a GET_NEIGHBORS message.
  ///
  /// A node wants to know information about some of our neighbors.
  /// We answer by sending a INFORM_ABOUT_NEIGHBORS message back.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processGetNeighbors(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processGetNeighbors(const MessageGetNeighbors &msg);

  void waitForAcks(uint32_t number, std::function<void()> cb) final;

  void processRemoveNeighborAck();

  /// This method will be called when we receive a INFORM_ABOUT_NEIGHBORS message.
  ///
  /// A node sent us information about some of their neighbors
  /// after we asked for this information previously.
  ///
  /// Now we can handle this information. At the moment we only procede
  /// with an interruped join accept procedure.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processInformAboutNeighbors(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processInformAboutNeighbors(const MessageInformAboutNeighbors &msg);
};

}  // namespace minhton

#endif
