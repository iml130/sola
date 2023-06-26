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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_AUCTION_INITIATOR_STATE_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_AUCTION_INITIATOR_STATE_H_

#include <memory>

#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/logical/message/auction_based/bid_submission.h"
#include "cpps/logical/message/auction_based/winner_response.h"
#include "layered_precedence_graph.h"
#include "material_flow/model/material_flow.h"

namespace daisi::cpps::logical {

/// @brief Helper class for the IteratedAuctionDispositionInitiator to handle and store the state of
/// received bids and winner responses.
class AuctionInitiatorState {
public:
  /// @brief Helper struct to store information needed to send WinnerNotifications.
  struct Winner {
    std::string task_uuid;
    std::string winner_connection;
    daisi::util::Duration latest_finish_time;
  };

  explicit AuctionInitiatorState(std::shared_ptr<LayeredPrecedenceGraph> layered_precedence_graph);

  /// @brief Counting how many times bid submissions were successful or not.
  /// Throwing an exception if there were no bid submissions too many times.
  void countBidSubmissionProcessing();

  /// @brief Calculating winners for the different ability groups based on received bids.
  /// @return Vector of winners with information to send WinnerNotifications.
  std::vector<Winner> selectWinner();

  /// @brief Counting how many times winner responses were successful or not.
  /// Throwing an exception if there were no bid submissions too many times.
  void countWinnerResponseProcessing();

  /// @brief Processing each successful winner response and removing related bids.
  /// @return Tasks that got auctioned and can be set to scheduled.
  std::vector<daisi::material_flow::Task> processWinnerAcceptions();

  /// @brief Adding a received BidSubmission message to the internal state.
  /// @param bid_submission Received message
  void addBidSubmission(const BidSubmission &bid_submission);

  /// @brief Adding a received WinnerResponse message to the internal state if it was successful.
  /// @param winner_response Received message
  void addWinnerResponse(const WinnerResponse &winner_response);

  /// @brief Clearing all information from the internal state.
  void clearIterationInfo();

  /// @brief Clearing winner acceptions from the internal state.
  void clearWinnerAcceptions();

private:
  /// @brief Helper method for winner selection to remove bids for a given task from the vector of
  /// bid submissions.
  /// @param bid_submissions Vector to remove bids from.
  /// @param task_uuid Uuid of task that bids should get removed from.
  static void removeBidsForTask(std::vector<BidSubmission> &bid_submissions,
                                const std::string &task_uuid);

  /// @brief Helper method for winner selection to remove bids of a given winner from the vector of
  /// bid submissions.
  /// @param bid_submissions Vector to remove bids from.
  /// @param task_uuid Uuid of task that bids should get removed from.
  /// @param winner_connection Relevant connection string of the winner.
  static void removeBidsForWinner(std::vector<BidSubmission> &bid_submissions,
                                  const std::string &task_uuid,
                                  const std::string &winner_connection);

  /// @brief Helper method for winner selection to remove bids of winners that could also bid on the
  /// same task due to ability dependencies.
  /// @param bid_submissions Vector to remove bids from.
  /// @param ability_requirement Relevant ability
  static void removeBidsWhichMeetAbilityRequirement(
      std::vector<BidSubmission> &bid_submissions,
      const amr::AmrStaticAbility &ability_requirement);

  /// @brief Vector of all open and still valid BidSubmissions in this iteration.
  std::vector<BidSubmission> bid_submissions_;

  /// @brief Vector of all successful WinnerResponse messages that are still relevant in this
  /// iteration.
  std::vector<WinnerResponse> winner_acceptions_;

  /// @brief Access to task informaton, e.g., to set earliest start or latest finish constraints.
  std::shared_ptr<LayeredPrecedenceGraph> layered_precedence_graph_;

  /// @brief Counter for how many times no bid submissions were received.
  uint8_t no_bid_submissions_counter_ = 0;

  /// @brief Counter for how many times no winner acceptions were received.
  uint8_t no_winner_acceptions_counter_ = 0;
};

}  // namespace daisi::cpps::logical

#endif
