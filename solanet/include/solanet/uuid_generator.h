// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLANET_UUID_GENERATOR_H_
#define SOLANET_UUID_GENERATOR_H_

#include "solanet/uuid.h"

namespace solanet {

/// Generate a new UUID. This call is thread-safe
UUID generateUUID();
}  // namespace solanet

#endif  // SOLANET_UUID_GENERATOR_H_
