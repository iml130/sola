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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_ITERATED_AUCTION_DISPOSITION_INITIATOR_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_ITERATED_AUCTION_DISPOSITION_INITIATOR_H_

#include <memory>
#include <variant>

#include "auction_initiator_state.h"
#include "disposition_initiator.h"
#include "layered_precedence_graph.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

class IteratedAuctionDispositionInitiator : public DispositionInitiator {
public:
  explicit IteratedAuctionDispositionInitiator(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola);

  ~IteratedAuctionDispositionInitiator() = default;

  REGISTER_IMPLEMENTATION(BidSubmission);
  REGISTER_IMPLEMENTATION(WinnerResponse);

  virtual void addMaterialFlow(std::shared_ptr<material_flow::MFDLScheduler> scheduler);

private:
  /// @brief Preparing interaction by subscribing to required topics for each ability.
  /// @return Duration needed to prepare interaction, to time the following interactions
  /// accordingly.
  daisi::util::Duration prepareInteraction();

  void startIteration();
  void finishIteration();
  void bidProcessing();
  void winnerResponseProcessing();

  void taskAnnouncement();
  void notifyWinners(const std::vector<AuctionInitiatorState::Winner> &winners);

  void iterationNotification(const std::vector<daisi::material_flow::Task> &tasks);

  std::unordered_map<daisi::cpps::mrta::model::Ability, std::vector<daisi::material_flow::Task>,
                     daisi::cpps::mrta::model::AbilityHasher>
  getTaskAbilityMapping(const std::vector<daisi::material_flow::Task> &tasks);

  std::shared_ptr<LayeredPrecedenceGraph> layered_precedence_graph_;
  std::unique_ptr<AuctionInitiatorState> auction_initiator_state_;

  std::vector<daisi::cpps::mrta::model::Ability> available_abilities_;

  std::unordered_map<mrta::model::Ability, std::string, mrta::model::AbilityHasher>
      ability_topic_mapping_;

  /// @brief Storing all delays in one place. The unit is seconds.
  struct {
    daisi::util::Duration subscribe_topic = 0.1;

    daisi::util::Duration waiting_to_receive_bids = 0.7;

    daisi::util::Duration waiting_to_receive_winner_responses = 0.3;

  } delays_;
};

}  // namespace daisi::cpps::logical

#endif
