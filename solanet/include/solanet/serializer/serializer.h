// Copyright The SOLA Contributors
//
// Licensed under the MIT License.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: MIT

#ifndef SOLANET_SERIALIZER_SERIALIZER_H_
#define SOLANET_SERIALIZER_SERIALIZER_H_

#ifndef CPPCHECK_IGNORE
#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#endif

#include <sstream>

namespace solanet::serializer {

struct BinarySerializer {};

template <typename T, typename SerializerType = BinarySerializer>
std::string serialize(const T &msg) {
  static_assert(std::is_class_v<SerializerType>);

  std::stringstream ss;

  if constexpr (std::is_same_v<SerializerType, BinarySerializer>) {
    cereal::BinaryOutputArchive archive(ss);
    archive(msg);
  } else {
    static_assert(std::is_void_v<SerializerType>);  // No valid serializer type selected
  }

  return ss.str();
}

template <typename T, typename SerializerType = BinarySerializer>
T deserialize(const std::string &msg) {
  static_assert(std::is_class_v<SerializerType>);

  T m;
  std::istringstream iss(msg);

  if constexpr (std::is_same_v<SerializerType, BinarySerializer>) {
    cereal::BinaryInputArchive a(iss);
    a(m);
  } else {
    static_assert(std::is_void_v<SerializerType>);  // No valid serializer type selected
  }

  return m;
}

}  // namespace solanet::serializer

#endif  // SOLANET_SERIALIZER_SERIALIZER_H_
