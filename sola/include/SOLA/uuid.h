// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_UUID_H_
#define SOLA_UUID_H_

#include <string>

namespace sola {
struct UUID {
  std::string id;           // 64 byte uuid
  std::string description;  // name
};
}  // namespace sola

#endif
