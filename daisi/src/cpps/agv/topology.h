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

#ifndef SOLA_NS3_CPPS_TOPOLOGY_H_
#define SOLA_NS3_CPPS_TOPOLOGY_H_

#include <string>

#include "ns3/vector.h"

namespace daisi::cpps {
class TopologyNs3 {
public:
  TopologyNs3() = default;
  explicit TopologyNs3(const ns3::Vector &size);

  ns3::Vector getSize() const;
  bool isPointInToplogy(const ns3::Vector &vector) const;

  bool isValid() const;

  double getWidth() const;
  double getHeight() const;

private:
  std::string uuid_;
  ns3::Vector size_;
};
};  // namespace daisi::cpps
#endif
