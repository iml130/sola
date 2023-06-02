// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef MINHTON_UTILS_SERIALIZER_CEREAL_H_
#define MINHTON_UTILS_SERIALIZER_CEREAL_H_

#include <cassert>
#include <unordered_map>
#include <variant>

#include "minhton/algorithms/esearch/find_query.h"
#include "minhton/algorithms/esearch/node_data.h"
#include "minhton/core/constants.h"
#include "minhton/utils/serializer.h"

namespace minhton::serializer {
///
/// A serializer which converts a MessageVariant into a binary format and vice versa.
///
class SerializerCereal final : public ISerializer {
public:
  ~SerializerCereal() override = default;

  std::string serialize(const minhton::MessageVariant &msg) final;

  minhton::MessageVariant deserialize(const std::string &str) final;
};
}  // namespace minhton::serializer
#endif
