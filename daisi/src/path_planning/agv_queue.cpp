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

#include "agv_queue.h"

namespace daisi::path_planning {

std::string AGVQueue::next() {
  std::string next = queue_occupancy_.front();
  queue_occupancy_.pop();
  return next;
}

void AGVQueue::add(const std::string &agv_id) { queue_occupancy_.push(agv_id); }

bool AGVQueue::hasAGV() const { return !queue_occupancy_.empty(); }

size_t AGVQueue::numberAGV() const { return queue_occupancy_.size(); }

}  // namespace daisi::path_planning
