// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_CORE_DEFINITIONS_H_
#define MINHTON_CORE_DEFINITIONS_H_

#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "minhton/algorithms/esearch/node_data.h"  // TODO: Move needed parts into this header

namespace minhton {
using Entry = std::tuple<std::string, std::variant<int, float, bool, std::string>,
                         minhton::NodeData::ValueType>;
using FindResult = std::vector<std::vector<Entry>>;
}  // namespace minhton

#endif
