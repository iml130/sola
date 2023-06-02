// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/utils/timestamp_helper.h"

#include <chrono>

namespace minhton {
uint64_t getCurrentTime() {
  auto time_since_epoch = std::chrono::system_clock::now().time_since_epoch();
  auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch);
  return ms_since_epoch.count();
}
}  // namespace minhton
