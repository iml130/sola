// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_LEAVE_MINHTON_LEAVE_ALGORITHM_H_
#define MINHTON_ALGORITHMS_LEAVE_MINHTON_LEAVE_ALGORITHM_H_

#include <memory>

#include "minhton/algorithms/find_end/minhton_find_end_algorithm.h"
#include "minhton/algorithms/leave/leave_algorithm_general.h"
#include "minhton/message/find_end.h"
#include "minhton/message/types_all.h"

namespace minhton {

class MinhtonLeaveAlgorithm : public LeaveAlgorithmGeneral {
public:
  explicit MinhtonLeaveAlgorithm(std::shared_ptr<AccessContainer> access)
      : LeaveAlgorithmGeneral(access), find_end_helper_(access, false){};

  /// Helper method to quickly check if we will do a leave without replacement
  /// for the FSM. This must be done in accordance with the actual Leave implementation.
  ///
  /// This method checks the routing tables to say whether the current node is the last one of the
  /// network or it (maybe) is not.
  ///
  /// Only works for complete trees (nodes are added from left to right without gaps).
  ///
  /// \returns true when the current node is definitely the last one of the network, or false when
  /// it is not the last one or it cannot be determined yet
  bool canLeaveWithoutReplacement() override;

  /// We decide ourselves that we want to leave the network
  /// and initiate the leave procedure.
  ///
  /// This method gets called when we receive a leave signal.
  ///
  /// Typical Usage:
  /// \code
  ///   node->initiateSelfDeparture();
  /// \endcode
  ///
  void initiateSelfDeparture() override;

protected:
  /// This method will be called when we receive a FIND_REPLACEMENT message.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processFindReplacement(incoming_message);
  /// \endcode
  ///
  /// \param message the message we received and want to process
  void processFindReplacement(const minhton::MessageFindReplacement &message) override;

  ///
  /// Implementation of the FindReplacement algorithm.
  ///
  /// This variant of the method is called only once from the node which wants to leave.
  /// The method initiates the main logic for the leave process by calling the methods responsible
  /// for redirecting the request of finding a replacement node.
  ///
  /// In case the current node (that wants to leave) is also the parent of the last node in the
  /// network, we send a message to the last node in this method.
  ///
  void performFindReplacement();

  ///
  /// Implementation of the FindReplacement algorithm.
  ///
  /// This variant of the method may be called multiple times during the redirection of the request
  /// for finding a replacement.
  ///
  /// But it is only called once per reached node, since the logic in this method is responsible for
  /// calling other methods based on the SearchProgress of the msg. And those other methods
  /// redirect accordingly to another node, ideally coming closer to the last node of the network or
  /// gathering information to make it possible to find it.
  ///
  /// \param msg the message we received and read to proceed with the initiated leave procedure
  ///
  void performFindReplacement(const MessageFindReplacement &msg);

private:
  MinhtonFindEndAlgorithm find_end_helper_;
};
}  // namespace minhton

#endif
