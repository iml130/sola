// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <mutex>

#include "solanet/uuid.h"
#include "uuid/uuid.h"

namespace solanet {

UUID generateUUID() {
  static std::mutex uuid_mutex;
  uuid_t uuid;
  {
    std::scoped_lock lock(uuid_mutex);
    uuid_generate(uuid);
  }
  UUID solanet_uuid{};
  uuid_copy(solanet_uuid.data(), uuid);
  return solanet_uuid;
}

}  // namespace solanet
