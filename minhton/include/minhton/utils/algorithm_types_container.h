// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_UTILS_ALGORITHMS_TYPE_CONTAINER_H_
#define MINHTON_UTILS_ALGORITHMS_TYPE_CONTAINER_H_

#include "minhton/core/constants.h"

namespace minhton {

struct AlgorithmTypesContainer {
  JoinAlgorithms join;
  LeaveAlgorithms leave;
  SearchExactAlgorithms search_exact;
  ResponseAlgorithms response;
  BootstrapAlgorithms bootstrap;
};

}  // namespace minhton

#endif
