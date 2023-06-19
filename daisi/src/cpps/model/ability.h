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

#ifndef DAISI_MODEL_ABILITY_H_
#define DAISI_MODEL_ABILITY_H_

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "cpps/model/load_carrier_ability.h"

namespace daisi::cpps::amr {

using Ability = std::tuple<float, LoadCarrier>;

template <typename TupleT> bool equalAbility(const TupleT &t1, const TupleT &t2);
template <typename TupleT> bool lessAbility(const TupleT &t1, const TupleT &t2);
template <typename TupleT> bool lessOrEqualAbility(const TupleT &t1, const TupleT &t2);
template <typename TupleT> bool comparableAbility(const TupleT &t1, const TupleT &t2);
template <typename TupleT> std::ostream &printAbility(std::ostream &os, const TupleT &tp);

struct AbilityHasher {
  std::size_t operator()(const Ability &ability) const {
    using std::hash;
    using std::size_t;
    using std::string;

    std::string rep =
        std::to_string(std::get<0>(ability)) + "|" + std::get<1>(ability).getTypeAsString();
    std::size_t res = std::hash<std::string>()(rep);
    return res;
  }
};

}  // namespace daisi::cpps::amr

#endif
