// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <uuid/uuid.h>

#include <cstdint>
#include <random>

#include "minhton/utils/uuid.h"

namespace minhton {

minhton::UUID generateUUID() {
  uuid_t uuid;
  uuid_generate(uuid);
  minhton::UUID minhton_uuid{};
  uuid_copy(minhton_uuid.data(), uuid);
  return minhton_uuid;
}

uint64_t generateEventId() {
  // TODO Refactor. We shouldn't open a new random_device on every call
  // but at least once for every MINHTON instance. And maybe randomness is needed
  // for the algorithms in the future
  std::uniform_int_distribution<uint64_t> distribution;
  std::random_device dev;
  uint64_t temp_value = distribution(dev);
  /// TODO actually I dont really like this workaround, but this magic makes it happen...
  while (temp_value > (INT64_MAX)) {
    temp_value = (distribution(dev) >> 2);
  }
  return temp_value;
};
}  // namespace minhton
