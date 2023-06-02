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

#ifndef DAISI_PATH_PLANNING_AGV_STATE_H_
#define DAISI_PATH_PLANNING_AGV_STATE_H_

namespace daisi::path_planning {
//! Possible state of an AGV as seen from a \c PickupStation
enum class AGVState {
  kWaitingInQueue,
  kLeavingQueue,
  kDriveToPickupOut,
  kNegotiatingDriveToDelivery,
  kNegotiatedDriveToDelivery,
  kDriveToDelivery,
  kUnloading,
  kUnloadingFinished,
  kNegotiatingDriveToPickup,
  kNegotiatedDriveToPickup,
  kDriveToPickupIn,
  kDriveToQueue,
};
}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_AGV_STATE_H_
