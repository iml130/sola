// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include <uuid/uuid.h>

#include "utils.h"

namespace natter::utils {

UUID generateUUID() {
  uuid_t uuid;
  uuid_generate(uuid);
  UUID natter_uuid{};
  uuid_copy(natter_uuid.data(), uuid);
  return natter_uuid;
}

}  // namespace natter::utils
