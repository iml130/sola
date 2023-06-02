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

#ifndef DAISI_PATH_PLANNING_AGV_QUEUE_H_
#define DAISI_PATH_PLANNING_AGV_QUEUE_H_

#include <queue>

#include "ns3/vector.h"

namespace daisi::path_planning {

//! Single-point parking queue with unlimited space
class AGVQueue {
public:
  AGVQueue() = default;

  /**
   * Get and remove the next AGV from the queue
   * @return ID of the next AGV in the queue
   */
  [[nodiscard]] std::string next();

  /**
   * Add AGV to the end of the queue
   * @param agv_id ID of the AGV
   */
  void add(const std::string &agv_id);

  [[nodiscard]] bool hasAGV() const;

  [[nodiscard]] size_t numberAGV() const;

private:
  std::queue<std::string> queue_occupancy_;
};
}  // namespace daisi::path_planning

#endif  // DAISI_PATH_PLANNING_AGV_QUEUE_H_
