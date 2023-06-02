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

#ifndef SOLA_NS3_CPPS_BOUNDARIES_H_
#define SOLA_NS3_CPPS_BOUNDARIES_H_

#include <cstdint>

#include "ns3/vector.h"

struct Boundaries {
  ns3::Vector2D min;
  ns3::Vector2D max;
};

#endif
