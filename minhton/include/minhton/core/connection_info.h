// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_CONNECTION_INFO_H_
#define MINHTON_CORE_CONNECTION_INFO_H_

#include <functional>

#include "minhton/core/constants.h"
#include "minhton/core/node_info.h"

namespace minhton {

struct ConnectionInfo {
  minhton::NodeInfo node;
  NeighborRelationship relationship;
  std::string ip_old;
  uint16_t port_old = 0;
  uint16_t position = 0;
  minhton::NodeInfo ourself;
};

using NeighborCallbackFct = std::function<void(const ConnectionInfo &neighbor)>;

}  // namespace minhton

#endif
