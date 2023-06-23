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

#include "amr_static_ability.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace daisi::cpps::amr {

AmrStaticAbility::AmrStaticAbility(const LoadCarrier &load_carrier, float max_payload_weight_kg)
    : load_carrier_(load_carrier), max_payload_weight_kg_(max_payload_weight_kg) {}

const LoadCarrier &AmrStaticAbility::getLoadCarrier() const { return load_carrier_; }

float AmrStaticAbility::getMaxPayloadWeight() const { return max_payload_weight_kg_; }

bool AmrStaticAbility::isValid() const {
  return (load_carrier_.isValid() && max_payload_weight_kg_ > 0);
}

bool operator==(const AmrStaticAbility &a1, const AmrStaticAbility &a2) {
  return a1.getLoadCarrier() == a2.getLoadCarrier() &&
         a1.getMaxPayloadWeight() == a2.getMaxPayloadWeight();
}

bool operator!=(const AmrStaticAbility &a1, const AmrStaticAbility &a2) {
  return a1.getLoadCarrier() != a2.getLoadCarrier() ||
         a1.getMaxPayloadWeight() != a2.getMaxPayloadWeight();
}

bool operator<(const AmrStaticAbility &a1, const AmrStaticAbility &a2) {
  return a1.getLoadCarrier() < a2.getLoadCarrier() &&
         a1.getMaxPayloadWeight() < a2.getMaxPayloadWeight();
}

bool operator<=(const AmrStaticAbility &a1, const AmrStaticAbility &a2) {
  return a1.getLoadCarrier() <= a2.getLoadCarrier() &&
         a1.getMaxPayloadWeight() <= a2.getMaxPayloadWeight();
}

bool operator>(const AmrStaticAbility &a1, const AmrStaticAbility &a2) {
  return a1.getLoadCarrier() > a2.getLoadCarrier() &&
         a1.getMaxPayloadWeight() > a2.getMaxPayloadWeight();
}

bool operator>=(const AmrStaticAbility &a1, const AmrStaticAbility &a2) {
  return a1.getLoadCarrier() >= a2.getLoadCarrier() &&
         a1.getMaxPayloadWeight() >= a2.getMaxPayloadWeight();
}

std::ostream &operator<<(std::ostream &os, const AmrStaticAbility &a) {
  os << a.load_carrier_ << "|" << a.max_payload_weight_kg_;
  return os;
}

}  // namespace daisi::cpps::amr
