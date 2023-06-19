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

#ifndef DAISI_CPPS_AMR_AMR_STATIC_ABILITY_H_
#define DAISI_CPPS_AMR_AMR_STATIC_ABILITY_H_

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "amr_load_carrier.h"

namespace daisi::cpps::amr {

class AmrStaticAbility {
public:
  AmrStaticAbility() = default;
  AmrStaticAbility(const LoadCarrier &load_carrier, float max_payload_weight_kg);

  const LoadCarrier &getLoadCarrier() const;
  float getMaxPayloadWeight() const;

  friend bool operator==(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator!=(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator<(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator<=(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator>(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator>=(const AmrStaticAbility &a1, const AmrStaticAbility &a2);

private:
  LoadCarrier load_carrier_;
  float max_payload_weight_kg_;
};

struct AmrStaticAbilityHasher {
  std::size_t operator()(const AmrStaticAbility &ability) const {
    std::string rep = ability.getLoadCarrier().getTypeAsString() + "|" +
                      std::to_string(ability.getMaxPayloadWeight());

    std::size_t res = std::hash<std::string>()(rep);
    return res;
  }
};

// --------------------------------------------------------------------------------------------

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

    std::string rep =
        std::to_string(std::get<0>(ability)) + "|" + std::get<1>(ability).getTypeAsString();
    std::size_t res = std::hash<std::string>()(rep);
    return res;
  }
};

}  // namespace daisi::cpps::amr

#endif