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

AuctionParticipantTaskState::AuctionParticipantTaskState(daisi::material_flow::Task task)
    : task_(std::move(task)) {}

void AuctionParticipantTaskState::setInformation(
    const MetricsComposition &metrics_composition,
    std::shared_ptr<AuctionBasedOrderManagement::InsertionPoint> insertion_point) {
  metrics_composition_ = metrics_composition;
  insertion_point_ = insertion_point;
}

void AuctionParticipantTaskState::removeInformation() {
  insertion_point_.reset();
  metrics_composition_.reset();
}

const material_flow::Task &AuctionParticipantTaskState::getTask() const { return task_; }

const MetricsComposition &AuctionParticipantTaskState::getMetricsComposition() const {
  if (!metrics_composition_.has_value()) {
    throw std::runtime_error("MetricsComposition does not have a value. ");
  }
  return metrics_composition_.value();
}

const std::shared_ptr<AuctionBasedOrderManagement::InsertionPoint>
AuctionParticipantTaskState::getInsertionPoint() const {
  return insertion_point_;
}

bool AuctionParticipantTaskState::isValid() const {
  return insertion_point_ && metrics_composition_.has_value();
}

AuctionParticipantState::AuctionParticipantState(
    const std::vector<daisi::material_flow::Task> &tasks) {
  for (const auto &task : tasks) {
    AuctionParticipantTaskState task_state(task);
    task_state_mapping.emplace(task.getUuid(), task_state);
  }
}

AuctionParticipantTaskState AuctionParticipantState::pickBest() {
  if (task_state_mapping.empty()) {
    throw std::runtime_error(
        "If the task state mapping is empty, this method should never be called.");
  }

  if (!checkAllTaskStatesValid()) {
    throw std::runtime_error("Some task states are not valid. Pruning beforehand is necessary.");
  }

  std::vector<AuctionParticipantTaskState> task_states;
  for (const auto &entry : task_state_mapping) {
    task_states.push_back(entry.second);
  }

  auto task_state_comp = [](const auto &s1, const auto &s2) {
    return s1.getMetricsComposition() > s2.getMetricsComposition();
  };

  std::sort(task_states.begin(), task_states.end(), task_state_comp);

  return task_states.front();
}

void AuctionParticipantState::prune() {
  for (auto it = task_state_mapping.begin(); it != task_state_mapping.end();) {
    if (!it->second.isValid()) {
      it = task_state_mapping.erase(it);
    } else {
      it++;
    }
  }
}

bool AuctionParticipantState::checkAllTaskStatesValid() {
  return std::all_of(task_state_mapping.begin(), task_state_mapping.end(),
                     [](const auto pair) { return pair.second.isValid(); });
}

bool AuctionParticipantState::hasEntries() const { return !task_state_mapping.empty(); }

}  // namespace daisi::cpps::logical
