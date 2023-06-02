// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_UTILS_UTILS_H_
#define NATTER_UTILS_UTILS_H_

#include "natter/uuid.h"

namespace natter::utils {

/**
 * Creates random UUID
 * @return uuid
 */
UUID generateUUID();
}  // namespace natter::utils

#endif  // NATTER_UTILS_UTILS_H_
