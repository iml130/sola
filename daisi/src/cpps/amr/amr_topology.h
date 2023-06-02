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

#ifndef DAISI_CPPS_AMR_AMR_TOPOLOGY_H_
#define DAISI_CPPS_AMR_AMR_TOPOLOGY_H_

#include <string>

#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {
class Topology {
public:
  Topology() = default;
  explicit Topology(const util::Dimensions &size);

  util::Dimensions getSize() const;
  bool isPointInTopology(const util::Dimensions &vector) const;
  bool isPositionInTopology(const util::Position &vector) const;

  bool isValid() const;

  double getWidth() const;
  double getHeight() const;

  SERIALIZE(uuid_, size_);

private:
  std::string uuid_;
  util::Dimensions size_;
};
}  // namespace daisi::cpps
#endif
