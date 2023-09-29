// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_MINTHON_FIND_END_ALGORITHM_H_
#define MINHTON_ALGORITHMS_MINTHON_FIND_END_ALGORITHM_H_

#include <memory>

#include "minhton/algorithms/algorithm_interface.h"
#include "minhton/message/find_end.h"
#include "minhton/message/types_all.h"

namespace minhton {

class MinhtonFindEndAlgorithm : public AlgorithmInterface {
public:
  MinhtonFindEndAlgorithm(std::shared_ptr<AccessContainer> access, bool join)
      : AlgorithmInterface(access), join_(join){};

  void process(const MessageVariant &) override { /* nothing required */
  }

  /// Helper method to send a message for forwarding the join / leave request to another node
  ///
  /// \param target Node to forward the join / leave request to
  /// \param request_origin The node which wishes to join / leave the network
  /// \param search_progress The current step of the join / leave algorithm
  /// \param hop_count Counter for how many times the request has been forwarded
  void forwardRequest(minhton::NodeInfo target, minhton::NodeInfo request_origin,
                      SearchProgress search_progress, uint16_t hop_count);

  /// Helper method to jump to the most beneficial adjacent node (during a join / leave)
  ///
  /// \param request_origin The new node that wants to join or the existing node that wants to leave
  /// the network.
  /// \param hop_count Counter for how many times the request has been forwarded
  void forwardToAdjacentNode(minhton::NodeInfo request_origin, uint16_t hop_count);

  /// Helper method to check if the current node is the wanted join / leave replacement node
  /// position. Only use after it is known which level is the last one!
  ///
  /// \returns true if we are at the final position, else false
  bool isCorrectParent() const;

  /// Helper method to do the search for the first null node on one side
  ///
  /// \param request_origin The new node that wants to join or the existing node that wants to leave
  /// the network.
  /// \param left_side Set to true to only search on the left side, else on the right side
  /// \param hop_count Counter for how many times the request has been forwarded
  void searchEndOnLevel(minhton::NodeInfo request_origin, bool left_side, uint16_t hop_count);

  /// Helper method to get a node which is either the direct parent of a node or somewhere close to
  /// where the actual parent is, if the parent isn't in the routing table
  ///
  /// \param child Node we are searching a parent (or a node close to it) for
  /// \returns NodeInfo Parent or a node close to it
  minhton::NodeInfo findReachableNodeClosestToParent(minhton::NodeInfo child) const;

  /// Helper method to check if the current level is completely filled
  ///
  /// \param request_origin The new node that wants to join or the existing node that wants to leave
  /// the network.
  /// \param hop_count Counter for how many times the request has been forwarded
  void checkRight(minhton::NodeInfo request_origin, uint16_t hop_count);

  /// Helper method to check if the passed node is the last one that can exists on a level and is
  /// initialized
  ///
  /// \param node Node which should be checked
  /// \returns true if the condition is fulfilled
  static bool isRightmostPossibleNode(minhton::NodeInfo node);

  /// Helper method which makes a decision about the horiziontal direction to follow
  /// based on routing tables (only call once when the first null node is reached)
  ///
  /// \param request_origin The new node that wants to join or the existing node that wants to leave
  /// the network.
  /// \returns true when found correct position, false when concurrent steps are needed
  /// \param hop_count Counter for how many times the request has been forwarded
  bool decideNextStep(minhton::NodeInfo request_origin, uint16_t hop_count);

private:
  /// Set to true if the Find End Algorithm should operate in the mode for the join procedure, or
  /// set to false for usage with the leave procedure
  bool join_;
};

}  // namespace minhton

#endif
