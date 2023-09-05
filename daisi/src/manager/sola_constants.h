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

#ifndef DAISI_MANAGER_CONSTANTS_H_
#define DAISI_MANAGER_CONSTANTS_H_

#include <cstdint>

namespace daisi::constants {
constexpr uint32_t kNumberOfAppNodesPerSwitch = 48;  // deprecated
constexpr uint32_t kMaxNumberNodesPerCsmaSubnet = 48;
constexpr double kMaxPowerDBm = 28.0;
constexpr uint32_t kMaxNumberAmrsPerAp = 10;  // deprecated
constexpr uint32_t kMaxNumberNodesPerAp = 10;
}  // namespace daisi::constants

#endif
