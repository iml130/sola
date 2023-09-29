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

#ifndef DAISI_CPPS_AMR_MODEL_AMR_STATIC_ABILITY_H_
#define DAISI_CPPS_AMR_MODEL_AMR_STATIC_ABILITY_H_

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "amr_load_carrier.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps::amr {

// TODO: add documentation
class AmrStaticAbility {
public:
  AmrStaticAbility() = default;
  AmrStaticAbility(const LoadCarrier &load_carrier, float max_payload_weight_kg);

  const LoadCarrier &getLoadCarrier() const;
  float getMaxPayloadWeight() const;
  bool isValid() const;

  friend bool operator==(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator!=(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator<(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator<=(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator>(const AmrStaticAbility &a1, const AmrStaticAbility &a2);
  friend bool operator>=(const AmrStaticAbility &a1, const AmrStaticAbility &a2);

  friend std::ostream &operator<<(std::ostream &os, const AmrStaticAbility &a);

  SERIALIZE(load_carrier_, max_payload_weight_kg_);

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

}  // namespace daisi::cpps::amr

#endif
