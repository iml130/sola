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

#include "auction_participant_state.h"

namespace daisi::cpps::logical {

AuctionParticipantTaskState::AuctionParticipantTaskState(const daisi::material_flow::Task &task)
    : task(std::make_shared<daisi::material_flow::Task>(task)) {}

AuctionParticipantState::AuctionParticipantState(
    const std::vector<daisi::material_flow::Task> &tasks) {
  for (const auto &task : tasks) {
    AuctionParticipantTaskState task_state(task);
    task_state_mapping[task.getUuid()] = task_state;
  }
}

AuctionParticipantTaskState AuctionParticipantState::pickBest() {
  std::vector<AuctionParticipantTaskState> task_states;
  for (const auto &entry : task_state_mapping) {
    task_states.push_back(entry.second);
  }

  auto task_state_comp = [](const auto &s1, const auto &s2) {
    if (s1.metrics_composition.has_value()) {
      if (s2.metrics_composition.has_value()) {
        return s1.metrics_composition.value() > s2.metrics_composition.value();
      }
      return false;
    }
    return true;
  };

  std::sort(task_states.begin(), task_states.end(), task_state_comp);

  return task_states.front();
}

}  // namespace daisi::cpps::logical