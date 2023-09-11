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

#include "static_network_calculation.h"

#include <cmath>

namespace daisi::natter_ns3 {
uint32_t calculateLevel(uint32_t node_id, uint16_t fanout) {
  uint32_t level = 0;
  while (node_id != 0) {
    node_id = std::ceil(node_id / (float)fanout) - 1;
    ++level;
  }
  return level;
}

uint32_t calculateNumber(uint32_t i, uint16_t fanout, uint32_t own_level) {
  if (own_level == 0) return 0;
  uint32_t nodes_above = (pow(fanout, own_level) - 1) / (fanout - 1);
  return i - nodes_above;
}

std::vector<uint32_t> createLinearProjection(uint64_t number_of_nodes, uint16_t fanout,
                                             uint32_t own_index) {
  uint32_t i = 1;
  std::vector<uint32_t> proj{};
  while (i <= std::ceil((float)fanout / 2)) {
    uint32_t child = own_index * fanout + i;
    if (child < number_of_nodes) {
      auto other = createLinearProjection(number_of_nodes, fanout, child);
      proj.insert(proj.end(), other.begin(), other.end());
    }
    i = i + 1;
  }
  proj.push_back(own_index);
  while (i <= fanout) {
    uint32_t child = own_index * fanout + i;
    if (child < number_of_nodes) {
      auto other = createLinearProjection(number_of_nodes, fanout, child);
      proj.insert(proj.end(), other.begin(), other.end());
    }
    i = i + 1;
  }

  return proj;
}

}  // namespace daisi::natter_ns3
