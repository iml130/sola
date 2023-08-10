// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLA_SERVICE_H_
#define SOLA_SERVICE_H_

#include <any>
#include <map>
#include <string>

namespace sola {
struct Service {
  std::map<std::string, std::any> key_values;
  std::string friendly_name;
  std::string type;
  std::string uuid;
};
}  // namespace sola

#endif
