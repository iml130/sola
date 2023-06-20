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

#ifndef DAISI_CPPS_AMR_MODEL_AMR_LOAD_CARRIER_H_
#define DAISI_CPPS_AMR_MODEL_AMR_LOAD_CARRIER_H_

#include <cstdint>
#include <string>
#include <unordered_map>

#include "solanet/serializer/serialize.h"

namespace daisi::cpps::amr {

// TODO: add documentation
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

  static std::unordered_map<std::string, Types> string_to_type_;

  static Types getTypeFromString(const std::string &type_name);
};

}  // namespace daisi::cpps::amr
#endif
