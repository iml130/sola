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

#ifndef DAISI_STATIC_NETWORK_CALCULATION_H_
#define DAISI_STATIC_NETWORK_CALCULATION_H_

#include <cstdint>
#include <vector>

namespace daisi::natter_ns3 {
uint32_t calculateLevel(uint32_t node_id, uint16_t fanout);
uint32_t calculateNumber(uint32_t i, uint16_t fanout, uint32_t own_level);
std::vector<uint32_t> createLinearProjection(uint64_t number_of_nodes, uint16_t fanout,
                                             uint32_t own_index);
}  // namespace daisi::natter_ns3

#endif  // DAISI_STATIC_NETWORK_CALCULATION_H_
