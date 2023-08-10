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

#include "iterated_auction_disposition_participant.h"

#include "cpps/amr/model/amr_fleet.h"
#include "cpps/logical/message/auction_based/bid_submission.h"

namespace daisi::cpps::logical {

IteratedAuctionDispositionParticipant::IteratedAuctionDispositionParticipant(
    std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola,
    std::shared_ptr<AuctionBasedOrderManagement> order_management, AmrDescription description)
    : DispositionParticipant(sola),
      order_management_(std::move(order_management)),
      description_(std::move(description)) {
  auto topic = AmrFleet::get().getTopicForAbility(description_.getLoadHandling().getAbility());
  sola_->subscribeTopic(topic);
}

bool IteratedAuctionDispositionParticipant::process(const CallForProposal &call_for_proposal) {
  AuctionParticipantState state(call_for_proposal.getTasks());

  auto initiator_connection = call_for_proposal.getInitiatorConnection();

  if (initiator_auction_state_mapping_.count(initiator_connection) > 0) {
    throw std::runtime_error(
        "Auction Participant process already in place for initiator connection.");
  }

  calculateBids(state);

  if (state.hasEntries()) {
    initiator_auction_state_mapping_.emplace(initiator_connection, state);

    submitBid(initiator_connection);
  }

  return true;
}

bool IteratedAuctionDispositionParticipant::process(
    const IterationNotification &iteration_notification) {
  std::string initiator_connection = iteration_notification.getInitiatorConnection();

  auto it_auction_state = initiator_auction_state_mapping_.find(initiator_connection);
  if (it_auction_state != initiator_auction_state_mapping_.end()) {
    AuctionParticipantState &state = it_auction_state->second;

    for (const auto &task_uuid : iteration_notification.getTaskUuids()) {
      state.task_state_mapping.erase(task_uuid);
    }

    if (state.hasEntries()) {
      submitBid(initiator_connection);
    } else {
      initiator_auction_state_mapping_.erase(it_auction_state);
    }
  }

  return true;
}

bool IteratedAuctionDispositionParticipant::process(const WinnerNotification &winner_notification) {
  std::string initiator_connection = winner_notification.getInitiatorConnection();
  std::string task_uuid = winner_notification.getTaskUuid();

  auto it_auction_state = initiator_auction_state_mapping_.find(initiator_connection);
  if (it_auction_state == initiator_auction_state_mapping_.end()) {
    throw std::runtime_error("The auction winner does not have information about auction process.");
  }

  AuctionParticipantState &state = it_auction_state->second;
  auto it_auction_task_state = state.task_state_mapping.find(task_uuid);
  if (it_auction_task_state == state.task_state_mapping.end()) {
    throw std::runtime_error(
        "The auction winner does not have information about the auctioned task. ");
  }

  AuctionParticipantTaskState &task_state = it_auction_task_state->second;

  bool accept = false;
  if (task_state.isValid() &&
      order_management_->canAddTask(task_state.getTask(), task_state.getInsertionPoint())) {
    auto result = order_management_->getLatestCalculatedInsertionInfo();
    auto metrics_comp = result.first;

    if (metrics_comp == task_state.getMetricsComposition()) {
      accept = true;
    }
  }

  std::string participant_connection = sola_->getConectionString();
  if (accept) {
    bool success = order_management_->addTask(task_state.getTask(), task_state.getInsertionPoint());

    if (!success) {
      throw std::runtime_error("Winner accepts, but cannot add the task.");
    }

    // Send winner response acception
    WinnerResponse response(task_uuid, participant_connection, true);
    sola_->sendData(serialize(response), sola::Endpoint(initiator_connection));

    // Update states
    calculateBids(state);

  } else {
    task_state.removeInformation();
    state.prune();

    // Send winner response rejection
    WinnerResponse response(task_uuid, participant_connection, false);
    sola_->sendData(serialize(response), sola::Endpoint(initiator_connection));
  }

  return true;
}

void IteratedAuctionDispositionParticipant::calculateBids(AuctionParticipantState &state) {
  for (auto &pair : state.task_state_mapping) {
    // Iterating through each task state of this auction process

    if (order_management_->canAddTask(pair.second.getTask(), nullptr)) {
      // Setting new calculated information if we can add the task
      auto result = order_management_->getLatestCalculatedInsertionInfo();
      pair.second.setInformation(result.first, result.second);
    } else {
      // Setting previous information to invalid because we cannot accept anymore
      pair.second.removeInformation();
    }
  }

  state.prune();
}

void IteratedAuctionDispositionParticipant::submitBid(const std::string &initiator_connection) {
  auto it_state = initiator_auction_state_mapping_.find(initiator_connection);
  if (it_state == initiator_auction_state_mapping_.end()) {
    throw std::runtime_error("Auction state for initiator connection not found.");
  }

  AuctionParticipantState &state = it_state->second;
  AuctionParticipantTaskState best_task_state = state.pickBest();

  std::string task_uuid = best_task_state.getTask().getUuid();

  // Only submitting again if we previously submitted something else
  if (task_uuid != state.previously_submitted) {
    std::string participant_connection = sola_->getConectionString();

    BidSubmission bid_submission(task_uuid, participant_connection,
                                 description_.getLoadHandling().getAbility(),
                                 best_task_state.getMetricsComposition());

    sola_->sendData(serialize(bid_submission), sola::Endpoint(initiator_connection));

    state.previously_submitted = task_uuid;
  }
}

}  // namespace daisi::cpps::logical
