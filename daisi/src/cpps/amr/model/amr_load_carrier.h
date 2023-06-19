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

#ifndef DAISI_MODEL_LOAD_CARRIER_ABILITY_NS3_H_
#define DAISI_MODEL_LOAD_CARRIER_ABILITY_NS3_H_

#include <cstdint>
#include <string>
#include <unordered_map>

#include "cpps/message/serialize.h"

namespace daisi::cpps::amr {

class LoadCarrier {
public:
  enum Types : uint8_t {
    kEuroPalett,
    kEuroBox,
    kPackage,
    kNoLoadCarrierType,
  };

  LoadCarrier() = default;
  explicit LoadCarrier(const Types &type);
  explicit LoadCarrier(const std::string &type_name);

  friend bool operator==(const LoadCarrier &l1, const LoadCarrier &l2);
  friend bool operator!=(const LoadCarrier &l1, const LoadCarrier &l2);
  friend bool operator<(const LoadCarrier &l1, const LoadCarrier &l2);
  friend bool operator<=(const LoadCarrier &l1, const LoadCarrier &l2);
  friend bool operator>(const LoadCarrier &l1, const LoadCarrier &l2);
  friend bool operator>=(const LoadCarrier &l1, const LoadCarrier &l2);
  friend std::ostream &operator<<(std::ostream &os, const LoadCarrier &l);

  std::string getTypeAsString() const;

  SERIALIZE(type_);

private:
  Types type_ = Types::kNoLoadCarrierType;

  static Types getTypeFromString(const std::string &type_name);

  static std::unordered_map<std::string, Types> string_to_type_;
};

bool comparable(const LoadCarrier &l1, const LoadCarrier &l2);

}  // namespace daisi::cpps::amr
#endif
