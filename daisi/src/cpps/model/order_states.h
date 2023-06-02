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

#ifndef DAISI_model_task_STATES_NS3_H_
#define DAISI_model_task_STATES_NS3_H_

namespace daisi::cpps {

enum class OrderStates {
  kCreated = 0,
  kQueued = 1,
  kStarted = 2,
  kGoToPickUpLocation = 3,
  kReachedPickUpLocation = 4,
  kLoad = 5,
  kLoaded = 6,
  kGoToDeliveryLocation = 7,
  kReachedDeliveryLocation = 8,
  kUnload = 9,
  kUnloaded = 10,
  kFinished = 11,
  kError = 12,
  kInvalid = 13,
};

}  // namespace daisi::cpps

#endif
