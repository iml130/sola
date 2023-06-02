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

#ifndef DAISI_CPPS_AMR_PHYSICAL_AMR_MOBILITY_STATUS_H_
#define DAISI_CPPS_AMR_PHYSICAL_AMR_MOBILITY_STATUS_H_

#include "utils/structure_helpers.h"

namespace daisi::cpps {

enum class AmrMobilityState {
  kAccelerating,
  kConstSpeedTraveling,
  kDecelerating,
  kStationary,
  kIdle
};

struct AmrMobilityStatus {
  AmrMobilityStatus() = default;

  AmrMobilityState state = AmrMobilityState::kIdle;
  util::Velocity velocity;
  util::Position position;
  util::Acceleration acceleration;
  util::Duration timestamp = 0.0;
};
}  // namespace daisi::cpps
#endif
