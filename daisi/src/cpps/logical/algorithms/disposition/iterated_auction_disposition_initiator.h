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

/// @brief The initiator class for auction-based disposition, based on the TePSSI algorithm from
/// Nunes, E., McIntire, M., & Gini, M. (2017). Decentralized multi-robot allocation of tasks with
/// temporal and precedence constraints. Advanced Robotics, 31(22), 1193-1207.
///
/// This class is the counterpart of the IteratedAuctionDispositionParticipant.
///
/// Modifications were made to the algorithm by sending CallForPropsal and IterationNotification
/// messages on separate topcis by assuming separate topics for robots (AMRs) with different
/// physical properties (abilities).
class IteratedAuctionDispositionInitiator : public DispositionInitiator {
public:
  explicit IteratedAuctionDispositionInitiator(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola,
                                               std::shared_ptr<CppsLoggerNs3> logger);

  ~IteratedAuctionDispositionInitiator() = default;

  /// @brief Storing bid submission information in a helper class to determine winners.
  REGISTER_IMPLEMENTATION(BidSubmission);

  /// @brief Storing winner response notification in a helper class to determine outdated bids.
  REGISTER_IMPLEMENTATION(WinnerResponse);

  virtual void addMaterialFlow(std::shared_ptr<material_flow::MFDLScheduler> scheduler) override;

  virtual void logMaterialFlowContent(const std::string &material_flow_uuid) override;

private:
  /// @brief Preparing interaction by subscribing to required topics for each ability.
  /// @return Duration needed to prepare interaction, to time the following interactions
  /// accordingly.
  daisi::util::Duration prepareInteraction();

  /// @brief Starting the auction of a set of auctionable tasks.
  /// Initiating the auction of the set with CallForProposal messages and scheduling bid processing.
  void startIteration();

  /// @brief Finishing the iteration of the set of auctionable tasks by updating the precedence
  /// graph and checking whether there are still tasks left in the total graph. If yes, the next
  /// iteration is started.
  void finishIteration();

  /// @brief Processing received bids during the delay. Selecting winners and sending
  /// WinnerNotifications and scheduling the winnerResponseProcessing. If there were no winners, the
  /// participants are getting renotified.
  void bidProcessing();

  /// @brief Processing the received WinnerResponses. IterationNotifications are being published for
  /// successfully auctioned tasks. If all free tasks are scheduled, the finishIteration method is
  /// being scheduled. Otherwise the loop continues with another bidProcessing.
  void winnerResponseProcessing();

  /// @brief Helper method to publish CallForProposal messages about auctionable tasks on the
  /// relevant topics.
  void callForProposal();

  /// @brief Helper method to publish IterationNotifications to participants that did not win on
  /// relevant topics.
  /// @param tasks Tasks that were auctioned in this iteration.
  void iterationNotification(const std::vector<daisi::material_flow::Task> &tasks);

  /// @brief Helper method to send WinnerNotifications to calculated winners.
  /// @param winners Previously calculated information about winners in the iteration.
  void notifyWinners(const std::vector<AuctionInitiatorState::Winner> &winners);

  void setPreparationFinished();

  void logMaterialFlowOrderStatesOfTask(const material_flow::Task &task,
                                        const OrderStates &order_state);

  /// @brief Helper method to determine the relevant abilities to each task and mapping them.
  /// @param tasks Relevant tasks to be mapped
  std::unordered_map<amr::AmrStaticAbility, std::vector<daisi::material_flow::Task>,
                     amr::AmrStaticAbilityHasher>
  getTaskAbilityMapping(const std::vector<daisi::material_flow::Task> &tasks);

  /// @brief Helper class for storing information about tasks and their layers in a precedence
  /// graph, according to the pIA algorithm.
  std::shared_ptr<LayeredPrecedenceGraph> layered_precedence_graph_;

  /// @brief Helper class for storing submitted bids and winner responses for determining winners.
  std::unique_ptr<AuctionInitiatorState> auction_initiator_state_;

  /// @brief Storing which ability corresponds to which topic when publishing CallForProposals and
  /// IterationNotifiations.
  std::unordered_map<amr::AmrStaticAbility, std::string, amr::AmrStaticAbilityHasher>
      ability_topic_mapping_;

  /// @brief Flag to note whether the preparation of subscribing to topics has finished or not.
  bool preparation_finished_ = false;

  /// @brief Storing all delays in one place. The unit is seconds.
  struct {
    /// @brief Delay between the consecutive subscribing to topics in the prepareInteration method.
    daisi::util::Duration subscribe_topic = 0.1;

    /// @brief Delay between the scheduling and execution of the bidProcessing method.
    daisi::util::Duration waiting_to_receive_bids = 0.7;

    /// @brief Delay between the scheduling and execution of the winnerResponseProcessing method.
    daisi::util::Duration waiting_to_receive_winner_responses = 0.3;

  } delays_;
};

}  // namespace daisi::cpps::logical

#endif
