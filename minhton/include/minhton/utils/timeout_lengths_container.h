// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_UTILS_TIMEOUTS_LENGTHS_CONTAINER_H_
#define MINHTON_UTILS_TIMEOUTS_LENGTHS_CONTAINER_H_

#include <cstdint>

namespace minhton {

struct TimeoutLengthsContainer {
  uint64_t bootstrap_response;
  uint64_t join_response;
  uint64_t join_accept_ack_response;
  uint64_t replacement_offer_response;
  uint64_t replacement_ack_response;
  uint64_t dsn_aggregation;
  uint64_t inquiry_aggregation;
};

}  // namespace minhton

#endif
