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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_AUCTION_PARTICIPANT_STATE_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_AUCTION_PARTICIPANT_STATE_H_

#include <memory>
#include <unordered_map>

#include "cpps/logical/message/auction_based/bid_submission.h"
#include "cpps/logical/message/auction_based/winner_response.h"
#include "cpps/logical/order_management/auction_based_order_management.h"
#include "material_flow/model/task.h"

namespace daisi::cpps::logical {

struct AuctionParticipantTaskState {
  AuctionParticipantTaskState() = default;

  AuctionParticipantTaskState(const daisi::material_flow::Task &task);

  std::shared_ptr<daisi::material_flow::Task> task = nullptr;

  std::shared_ptr<AuctionBasedOrderManagement::InsertionPoint> insertion_point = nullptr;

  std::optional<MetricsComposition> metrics_composition = std::nullopt;
};

struct AuctionParticipantState {
  AuctionParticipantState() = default;

  AuctionParticipantState(const std::vector<daisi::material_flow::Task> &tasks);

  std::unordered_map<std::string, AuctionParticipantTaskState> task_state_mapping;

  std::string previously_submitted;

  AuctionParticipantTaskState pickBest();
};

}  // namespace daisi::cpps::logical

#endif
