// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef NATTER_NATTER_CHECK_H_
#define NATTER_NATTER_CHECK_H_

#include <stdexcept>

// Checks if (req)uirement is true. Otherwise, throws exception with msg
#define NATTER_CHECK(req, msg)     \
  if (!(req)) {                    \
    throw std::runtime_error(msg); \
  }

#endif  // NATTER_NATTER_CHECK_H_
