// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_SOLA_CHECK_H_
#define SOLA_SOLA_CHECK_H_

#include <stdexcept>

// Checks if (req)uirement is true. Otherwise, throws exception with msg
#define SOLA_CHECK(req, msg)       \
  if (!(req)) {                    \
    throw std::runtime_error(msg); \
  }

#endif  // SOLA_SOLA_CHECK_H_
