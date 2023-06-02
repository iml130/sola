// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#include "minhton/utils/serializer_cereal.h"

#ifndef CPPCHECK_IGNORE
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#endif

#include "minhton/message/types_all.h"

namespace minhton::serializer {

std::string SerializerCereal::serialize(const minhton::MessageVariant &msg) {
  std::stringstream ss;
  cereal::BinaryOutputArchive archive(ss);
  archive(msg);
  return ss.str();
}

minhton::MessageVariant SerializerCereal::deserialize(const std::string &str) {
  minhton::MessageVariant msg;
  std::istringstream iss(str);
  cereal::BinaryInputArchive archive(iss);
  archive(msg);
  return msg;
}
}  // namespace minhton::serializer
