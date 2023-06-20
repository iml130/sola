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

#include "auction_initiator_state.h"

namespace daisi::cpps::logical {

void AuctionInitiatorState::addBidSubmission(const BidSubmission &bid_submission) {
  bid_submissions_.push_back(bid_submission);
}

void AuctionInitiatorState::addWinnerResponse(const WinnerResponse &winner_response) {
  if (winner_response.doesAccept()) {
    winner_acceptions_.push_back(winner_response);
  } else {
    removeBidsForWinner(bid_submissions_, winner_response.getTaskUuid(),
                        winner_response.getParticipantConnection());
  }
}

void AuctionInitiatorState::removeBidsForTask(std::vector<BidSubmission> &bid_submissions,
                                              const std::string &task_uuid) {
  bid_submissions.erase(
      std::remove_if(bid_submissions.begin(), bid_submissions.end(),
                     [&task_uuid](const auto &bid) { return bid.getTaskUuid() == task_uuid; }),
      bid_submissions.end());
}

void AuctionInitiatorState::removeBidsForWinner(std::vector<BidSubmission> &bid_submissions,
                                                const std::string &task_uuid,
                                                const std::string &winner_connection) {
  bid_submissions.erase(std::remove_if(bid_submissions.begin(), bid_submissions.end(),
                                       [&task_uuid, &winner_connection](const auto &bid) {
                                         return bid.getTaskUuid() == task_uuid &&
                                                bid.getParticipantConnection() == winner_connection;
                                       }),
                        bid_submissions.end());
}

void AuctionInitiatorState::removeBidsWhichMeetAbilityRequirement(
    std::vector<BidSubmission> &bid_submissions, const amr::AmrStaticAbility &ability_requirement) {
  bid_submissions.erase(std::remove_if(bid_submissions.begin(), bid_submissions.end(),
                                       [&ability_requirement](const auto &bid) {
                                         return ability_requirement <= bid.getParticipantAbility();
                                       }),
                        bid_submissions.end());
}

std::vector<daisi::material_flow::Task> AuctionInitiatorState::processWinnerAcceptions() {
  std::vector<daisi::material_flow::Task> auctioned_tasks;

  for (const auto &winner : winner_acceptions_) {
    auto auctioned_task = layered_precedence_graph_->getTask(winner.getTaskUuid());
    auctioned_tasks.push_back(auctioned_task);

    removeBidsForTask(bid_submissions_, winner.getTaskUuid());
  }

  winner_acceptions_.clear();
  return auctioned_tasks;
}

void AuctionInitiatorState::countWinnerResponseProcessing() {
  if (!winner_acceptions_.empty()) {
    no_winner_acceptions_counter_ = 0;
  } else {
    if (++no_winner_acceptions_counter_ >= 100) {
      throw std::runtime_error("No winner acceptions.");
    }
  }
}

void AuctionInitiatorState::countBidSubmissionProcessing() {
  if (!bid_submissions_.empty()) {
    no_bid_submissions_counter_ = 0;
  } else {
    if (++no_bid_submissions_counter_ >= 5) {
      throw std::runtime_error("No bid submissions.");
    }
  }
}

std::vector<AuctionInitiatorState::Winner> AuctionInitiatorState::selectWinner() {
  if (bid_submissions_.empty()) {
    return {};
  }

  std::vector<AuctionInitiatorState::Winner> winners;
  auto temp_bids = bid_submissions_;

  while (!temp_bids.empty()) {
    std::sort(temp_bids.begin(), temp_bids.end(),
              [](const auto &b1, const auto &b2) { return b1 > b2; });

    auto best_bid = temp_bids.front();
    auto task_uuid = best_bid.getTaskUuid();

    removeBidsForTask(bid_submissions_, task_uuid);

    if (!layered_precedence_graph_->isFreeTaskScheduled(task_uuid)) {
      daisi::util::Duration latest_finish_time =
          best_bid.getMetricsComposition().getCurrentMetrics().getMakespan();
      layered_precedence_graph_->setLatestFinishTime(task_uuid, latest_finish_time);

      layered_precedence_graph_->setTaskScheduled(task_uuid);

      winners.push_back({task_uuid, best_bid.getParticipantConnection(), latest_finish_time});
    } else {
      throw std::runtime_error("Received bid for scheduled task.");
    }
  }

  return winners;
}

}  // namespace daisi::cpps::logical
