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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_AUCTION_STATE_HELPER_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_AUCTION_STATE_HELPER_H_

#include <memory>

#include "cpps/logical/message/auction_based/bid_submission.h"
#include "cpps/logical/message/auction_based/winner_response.h"
#include "cpps/model/ability.h"
#include "layered_precedence_graph.h"
#include "material_flow/model/material_flow.h"

namespace daisi::cpps::logical {

class AuctionStateHelper {
public:
  struct Winner {
    std::string task_uuid;
    std::string winner_connection;
    daisi::util::Duration latest_finish_time;
  };

  AuctionStateHelper() = default;

  void countBidSubmissionProcessing();
  std::vector<Winner> selectWinner();

  void countWinnerResponseProcessing();
  std::vector<daisi::material_flow::Task> processWinnerAcceptions();

  void addBidSubmission(const BidSubmission &bid_submission);
  void addWinnerResponse(const WinnerResponse &winner_response);

  void clearIterationInfo();
  void clearWinnerAcceptions();

private:
  static void removeBidsForTask(std::vector<BidSubmission> &bid_submissions,
                                const std::string &task_uuid);
  static void removeBidsForWinner(std::vector<BidSubmission> &bid_submissions,
                                  const std::string &task_uuid,
                                  const std::string &winner_connection);
  static void removeBidsWhichMeetAbilityRequirement(
      std::vector<BidSubmission> &bid_submissions,
      const daisi::cpps::mrta::model::Ability &ability_requirement);

  std::vector<BidSubmission> bid_submissions_;
  std::vector<WinnerResponse> winner_acceptions_;

  std::shared_ptr<LayeredPrecedenceGraph> layered_precedence_graph_;

  uint8_t no_winner_acceptions_counter_ = 0;
  uint8_t no_bid_submissions_counter = 0;
};

}  // namespace daisi::cpps::logical

#endif
