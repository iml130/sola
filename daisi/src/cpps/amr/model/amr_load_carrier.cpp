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

#include "amr_load_carrier.h"

#include <stdexcept>

namespace daisi::cpps::amr {

LoadCarrier::LoadCarrier(const Types &type) : type_(type) {}

LoadCarrier::LoadCarrier(const std::string &type_name)
    : type_(LoadCarrier::getTypeFromString(type_name)) {}

bool operator==(const LoadCarrier &l1, const LoadCarrier &l2) { return l1.type_ == l2.type_; }

bool operator!=(const LoadCarrier &l1, const LoadCarrier &l2) { return l1.type_ != l2.type_; }

bool operator<(const LoadCarrier &l1, const LoadCarrier &l2) { return false; }

bool operator<=(const LoadCarrier &l1, const LoadCarrier &l2) { return (l1 == l2) || (l1 < l2); };

bool operator>(const LoadCarrier &l1, const LoadCarrier &l2) { return false; }

bool operator>=(const LoadCarrier &l1, const LoadCarrier &l2) { return (l1 == l2) || (l1 > l2); };

std::ostream &operator<<(std::ostream &os, const LoadCarrier &l) {
  os << l.getTypeAsString();
  return os;
}

std::string LoadCarrier::getTypeAsString() const {
  for (auto const &entry : LoadCarrier::string_to_type_) {
    if (entry.second == type_) {
      return entry.first;
    }
  }
  return "invalid";
}

bool LoadCarrier::isValid() const { return type_ != LoadCarrier::Types::kNoLoadCarrierType; }

LoadCarrier::Types LoadCarrier::getTypeFromString(const std::string &type_name) {
  if (LoadCarrier::string_to_type_.find(type_name) != LoadCarrier::string_to_type_.end()) {
    return LoadCarrier::string_to_type_.at(type_name);
  }
  throw std::invalid_argument("Invalid string for LoadCarrier Type");
}

std::unordered_map<std::string, LoadCarrier::Types> LoadCarrier::string_to_type_ = {
    {"eurobox", LoadCarrier::Types::kEuroBox},
    {"pallet", LoadCarrier::Types::kEuroPalett},
    {"package", LoadCarrier::Types::kPackage},
};

}  // namespace daisi::cpps::amr
