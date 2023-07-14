// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_SEARCHEXACT_SEARCH_EXACT_ALGORITHM_GENERAL_H_
#define MINHTON_ALGORITHMS_SEARCHEXACT_SEARCH_EXACT_ALGORITHM_GENERAL_H_

#include <memory>

#include "minhton/algorithms/search_exact/interface_search_exact_algorithm.h"
#include "minhton/message/message.h"
#include "minhton/message/remove_neighbor.h"
#include "minhton/message/search_exact.h"
#include "minhton/message/search_exact_failure.h"

namespace minhton {

class SearchExactAlgorithmGeneral : public SearchExactAlgorithmInterface {
public:
  SearchExactAlgorithmGeneral(std::shared_ptr<AccessContainer> access)
      : SearchExactAlgorithmInterface(access){};

  ~SearchExactAlgorithmGeneral() override = default;

  void process(const MessageVariant &msg) override;

  /// The actual implementation of the search exact procedure.
  ///
  /// We do not do this in the processSearchExact method because we also locally
  /// need to call this method.
  ///
  /// Therefore processSearchExact simply calls this method.
  ///
  /// Typical Usage:
  /// \code
  ///   this->performSearchExact(destination_node, query);
  /// \endcode
  ///
  /// \param destination_node the desired destination node
  /// \param query the content intended for the \p destination_node
  void performSearchExact(const minhton::NodeInfo &destination,
                          std::shared_ptr<MessageSEVariant> query) override;

protected:
  virtual minhton::NodeInfo calcClosestRedirect(double dest_value) = 0;

  /// This method will be called when we receive a SEARCH_EXACT message.
  ///
  /// A node wants to forward a message to another node, but only knows the logical position in the
  /// tree.
  ///
  /// If we are the destination node we process the message via the recv-method.
  /// Otherwise we forward the message further.
  ///
  /// Typical Usage:
  /// \code
  ///   this->processSearchExact(incoming_message);
  /// \endcode
  ///
  /// \param msg the message we received and want to process
  void processSearchExact(const minhton::MessageSearchExact &msg);

  void notifyAboutFailure(const minhton::NodeInfo &destination,
                          std::shared_ptr<MessageSEVariant> query);
};

}  // namespace minhton

#endif
