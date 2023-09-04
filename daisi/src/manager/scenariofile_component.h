// Copyright 2023 The SOLA authors
//
// This file is part of DAISI.
//
// DAISI is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation; version 2.
//
// DAISI is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with DAISI. If not, see
// <https://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef DAISI_CPPS_MANAGER_SCENARIOFILE_COMPONENT_H_
#define DAISI_CPPS_MANAGER_SCENARIOFILE_COMPONENT_H_

#include <yaml-cpp/yaml.h>

#include <optional>
#include <variant>

#define STRING_NAME(VAR_NAME) #VAR_NAME
#define SERIALIZE_VAR(VAR) serializeType(VAR, STRING_NAME(VAR), node);

namespace daisi {

template <typename T> void serializeType(T &t, const std::string &key, YAML::Node node) {
  try {
    if constexpr (std::is_fundamental<T>::value || std::is_same<T, std::string>::value) {
      // Can directly fetch this
      t = node[key].as<T>();
    } else {
      // Require to call our own parse method
      // For the future: If we could use reflections or recursive macros we could write
      // YAML::convert<>::decode methods more or less automatically and always
      // use the if-case
      t.parse(node[key]);
    }
  } catch (YAML::TypedBadConversion<T> &e) {
    throw std::runtime_error("Trouble reading key '" + key + "' from YAML.");
  }
}

template <typename T>
void serializeType(std::vector<T> &t, const std::string &key, YAML::Node node) {
  for (const auto &n : node[key]) {
    T inner;
    inner.parse(n);
    t.push_back(inner);
  }
}

template <typename T>
void serializeType(std::optional<T> &t, const std::string &key, const YAML::Node &node) {
  if (!node[key].IsDefined()) {
    t = std::nullopt;
    return;
  }

  try {
    if constexpr (std::is_fundamental<T>::value || std::is_same<T, std::string>::value) {
      t = node[key].as<T>();
    } else {
      t.emplace();
      t->parse(node[key]);
    }
  } catch (YAML::TypedBadConversion<T> &e) {
    throw std::runtime_error("Wrong data type!");
  }
}

/// @brief Deserialize data from node into variant with type T, if \p type matches
/// T.typeName()
/// @tparam T The type to parse if \p type matches \p req_type
/// @param node the YAML node to read data from
/// @param var variant to which the deserialized object is put
/// @param type the actual type
/// @return true if data was deserialzied
template <typename T, typename... U>
bool deserializeIf(YAML::Node node, std::variant<U...> &var, const std::string &type) {
  if (type == T::typeName()) {
    var.template emplace<T>();

    // Do parsing of the actual object
    std::get<T>(var).parse(node);

    return true;
  }
  return false;
}

}  // namespace daisi

#endif
