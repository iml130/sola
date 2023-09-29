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

#ifndef DAISI_CPPS_AMR_AMR_PHYSICAL_PROPERTIES_H_
#define DAISI_CPPS_AMR_AMR_PHYSICAL_PROPERTIES_H_

#include <vector>

#include "solanet/serializer/serialize.h"

namespace daisi::cpps {
class AmrPhysicalProperties {
public:
  AmrPhysicalProperties() = default;
  AmrPhysicalProperties(double weight_kg, std::vector<double> dimensions_m)
      : dimensions_m_(std::move(dimensions_m)), weight_kg_(weight_kg) {}

  /// Get weight in kilograms
  double getWeight() const { return weight_kg_; }

  // Get dimensions in meters
  std::vector<double> getDimensions() const { return dimensions_m_; }

  SERIALIZE(dimensions_m_, weight_kg_);

private:
  std::vector<double> dimensions_m_;
  double weight_kg_ = -1;
};
}  // namespace daisi::cpps
#endif
