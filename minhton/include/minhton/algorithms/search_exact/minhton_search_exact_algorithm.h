// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_ALGORITHMS_SEARCHEXACT_MINHTON_SEARCH_EXACT_ALGORITHM_H_
#define MINHTON_ALGORITHMS_SEARCHEXACT_MINHTON_SEARCH_EXACT_ALGORITHM_H_

#include <memory>

#include "minhton/algorithms/search_exact/search_exact_algorithm_general.h"
#include "minhton/message/message.h"
#include "minhton/message/search_exact.h"

namespace minhton {

class MinhtonSearchExactAlgorithm : public SearchExactAlgorithmGeneral {
public:
  explicit MinhtonSearchExactAlgorithm(std::shared_ptr<AccessContainer> access)
      : SearchExactAlgorithmGeneral(access){};

  ~MinhtonSearchExactAlgorithm() override = default;

  ///
  /// Helper method for Search Exact procedure,
  /// to calculate the closest node to the destination,
  /// by considering all of our routing information
  /// and by using the tree mapper.
  ///
  /// Typical usage:
  /// \code
  ///   this->routing_info_.calcClosestRedirect(dest_value, prohibited_node);
  /// \endcode
  ///
  /// \param dest_value double by the tree mapper of the destination horizontal value
  ///
  /// \returns NodeInfo object of the closest redirect we know
  ///           or uninitialized object if no node is found on the correct side.
  ///
  minhton::NodeInfo calcClosestRedirect(double dest_value) override;
};

}  // namespace minhton

#endif
