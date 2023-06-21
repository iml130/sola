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

/// @brief Helper struct for the IteratedAuctionDispositionParticipant, used inside of the
/// AuctionParticipantState, to store information related to exactly one task.
/// This includes the previously calculated bids (metrics) and insertion points.
struct AuctionParticipantTaskState {
  explicit AuctionParticipantTaskState(daisi::material_flow::Task task);

  daisi::material_flow::Task task;

  std::shared_ptr<AuctionBasedOrderManagement::InsertionPoint> insertion_point = nullptr;

  std::optional<MetricsComposition> metrics_composition = std::nullopt;
};

/// @brief Helper struct for the IteratedAuctionDispositionParticipant to store the state of open
/// auction processes, calculated bids (metrics) and insertion points.
/// Each AuctionParticipantState is only responsible for one auction process by one dedicated
/// initiator.
struct AuctionParticipantState {
  /// @brief Initiatoring the task states based on the given tasks.
  /// @param tasks Open tasks that initially got announced.
  explicit AuctionParticipantState(const std::vector<daisi::material_flow::Task> &tasks);

  /// @brief Storing the state of each open task. The key is the task uuid.
  std::unordered_map<std::string, AuctionParticipantTaskState> task_state_mapping;

  /// @brief Task uuid of the latest previously submitted task. If a task was submitted before, the
  /// initiator has still the relevant information and sending this bid again is unnecessary
  /// overhead.
  std::string previously_submitted;

  /// @brief Given the information from the task_state_mapping, the best possible task is picked
  /// depending on stored metrics.
  /// @return
  AuctionParticipantTaskState pickBest();

  void prune();

  bool hasEntries() const;
};

}  // namespace daisi::cpps::logical

#endif
