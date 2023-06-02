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

#include "cpps/model/ability.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace daisi::cpps::mrta::model {

template <typename TupleT, std::size_t... Is>
bool equalAbilityImpl(const TupleT &t1, const TupleT &t2, std::index_sequence<Is...> /*unused*/) {
  size_t index = 0;
  std::vector<bool> res;
  auto compare = [&index, &res](const auto &x1, const auto &x2) { res.push_back((x1 == x2)); };

  (compare(std::get<Is>(t1), std::get<Is>(t2)), ...);
  return std::all_of(res.begin(), res.end(), [](bool v) { return v; });
}

template <typename TupleT, std::size_t... Is>
bool lessOrEqualAbilityImpl(const TupleT &t1, const TupleT &t2,
                            std::index_sequence<Is...> /*unused*/) {
  size_t index = 0;
  std::vector<bool> res;
  auto compare = [&index, &res](const auto &x1, const auto &x2) { res.push_back((x1 <= x2)); };

  (compare(std::get<Is>(t1), std::get<Is>(t2)), ...);
  return std::all_of(res.begin(), res.end(), [](bool v) { return v; });
}

template <typename TupleT, std::size_t... Is>
bool lessAbilityImpl(const TupleT &t1, const TupleT &t2, std::index_sequence<Is...> /*unused*/) {
  size_t index = 0;
  std::vector<bool> res;
  auto compare = [&index, &res](const auto &x1, const auto &x2) { res.push_back((x1 < x2)); };

  (compare(std::get<Is>(t1), std::get<Is>(t2)), ...);
  return std::all_of(res.begin(), res.end(), [](bool v) { return v; });
}

template <typename TupleT, std::size_t... Is>
bool comparableAbilityImpl(const TupleT &t1, const TupleT &t2,
                           std::index_sequence<Is...> /*unused*/) {
  size_t index = 0;
  std::vector<bool> res;
  auto compare = [&index, &res](const auto &x1, const auto &x2) {
    // x1 and x2 are comparable
    // if x1 <= x2 or x1 > x2
    res.push_back((x1 <= x2) || (x1 > x2));
  };

  (compare(std::get<Is>(t1), std::get<Is>(t2)), ...);
  return std::all_of(res.begin(), res.end(), [](bool v) { return v; });
}

template <typename TupleT, std::size_t... Is>
std::ostream &printAbilityImpl(std::ostream &os, const TupleT &tp,
                               std::index_sequence<Is...> /*unused*/) {
  auto print_elem = [&os](const auto &x, size_t id) {
    // if (id > 0) os << ", ";
    os << x;
  };

  // os << "(";
  (print_elem(std::get<Is>(tp), Is), ...);
  // os << ")";
  return os;
}

template <typename TupleT> bool equalAbility(const TupleT &t1, const TupleT &t2) {
  return equalAbilityImpl(t1, t2, std::make_index_sequence<std::tuple_size<Ability>::value>{});
}

template <typename TupleT> bool lessAbility(const TupleT &t1, const TupleT &t2) {
  return lessAbilityImpl(t1, t2, std::make_index_sequence<std::tuple_size<Ability>::value>{});
}

template <typename TupleT> bool lessOrEqualAbility(const TupleT &t1, const TupleT &t2) {
  return lessOrEqualAbilityImpl(t1, t2,
                                std::make_index_sequence<std::tuple_size<Ability>::value>{});
}

template <typename TupleT> bool comparableAbility(const TupleT &t1, const TupleT &t2) {
  return comparableAbilityImpl(t1, t2, std::make_index_sequence<std::tuple_size<Ability>::value>{});
}

template <typename TupleT> std::ostream &printAbility(std::ostream &os, const TupleT &tp) {
  return printAbilityImpl(os, tp, std::make_index_sequence<std::tuple_size<Ability>::value>{});
}

template bool equalAbility<mrta::model::Ability>(const mrta::model::Ability &t1,
                                                 const mrta::model::Ability &t2);
template bool lessAbility<mrta::model::Ability>(const mrta::model::Ability &t1,
                                                const mrta::model::Ability &t2);
template bool lessOrEqualAbility<mrta::model::Ability>(const mrta::model::Ability &t1,
                                                       const mrta::model::Ability &t2);
template bool comparableAbility<mrta::model::Ability>(const mrta::model::Ability &t1,
                                                      const mrta::model::Ability &t2);
template std::ostream &printAbility<mrta::model::Ability>(std::ostream &os,
                                                          const mrta::model::Ability &tp);

}  // namespace daisi::cpps::mrta::model
