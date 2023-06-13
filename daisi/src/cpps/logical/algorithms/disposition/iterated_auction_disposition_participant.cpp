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

#include "cpps/logical/message/auction_based/bid_submission.h"

namespace daisi::cpps::logical {

IteratedAuctionDispositionParticipant::IteratedAuctionDispositionParticipant(
    std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola)
    : DispositionParticipant(sola) {}

bool IteratedAuctionDispositionParticipant::process(const CallForProposal &call_for_proposal) {
  IteratedAuctionDispositionParticipant::AuctionParticipantState state(
      call_for_proposal.getTasks());
  auto initiator_connection = call_for_proposal.getInitiatorConnection();

  if (initiator_auction_state_mapping_.count(initiator_connection) > 0) {
    throw std::runtime_error(
        "Auction Participant process already in place for initiator connection.");
  }

  calculateBids(state);

  initiator_auction_state_mapping_[initiator_connection] = state;

  submitBid(initiator_connection);

  return true;
}

bool IteratedAuctionDispositionParticipant::process(
    const IterationNotification &iteration_notification) {
  std::string initiator_connection = iteration_notification.getInitiatorConnection();

  auto it_auction_state = initiator_auction_state_mapping_.find(initiator_connection);
  if (it_auction_state != initiator_auction_state_mapping_.end()) {
    AuctionParticipantState &state = it_auction_state->second;

    for (const auto &task_uuid : iteration_notification.getTaskUuids()) {
      state.removeTaskState(task_uuid);
    }

    if (state.hasOpenTasks()) {
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
  if (task_state.metrics_composition.has_value() &&
      order_management_->canAddTask(*task_state.task)) {
    auto result = order_management_->getLatestCalculatedInsertionInfo();
    auto metrics_comp = result.first;

    if (metrics_comp == task_state.metrics_composition.value()) {
      accept = true;
    }
  }

  std::string participant_connecton = sola_->getConectionString();
  if (accept) {
    bool success = order_management_->addTask(*task_state.task, task_state.insertion_point);

    if (!success) {
      throw std::runtime_error("Winner accepts, but cannot add the task.");
    }

    // send winner response acception
    WinnerResponse response(task_uuid, participant_connecton, true);
    sola_->sendData(serialize(response), sola::Endpoint(initiator_connection));

    // update states
    calculateBids(state);

  } else {
    task_state.metrics_composition = std::nullopt;
    task_state.insertion_point = nullptr;

    // send winner response rejection
    WinnerResponse response(task_uuid, participant_connecton, false);
    sola_->sendData(serialize(response), sola::Endpoint(initiator_connection));
  }

  return true;
}

void IteratedAuctionDispositionParticipant::clearStoredInformationAfterInsertion() {}

void IteratedAuctionDispositionParticipant::calculateBids(
    IteratedAuctionDispositionParticipant::AuctionParticipantState &state) {
  for (auto &[task_uuid, task_state] : state.task_state_mapping) {
    if (order_management_->canAddTask(*task_state.task)) {
      auto result = order_management_->getLatestCalculatedInsertionInfo();

      task_state.metrics_composition = result.first;
      task_state.insertion_point = result.second;
    } else {
      task_state.metrics_composition = std::nullopt;
      task_state.insertion_point = nullptr;
    }
  }
}

void IteratedAuctionDispositionParticipant::submitBid(const std::string &initiator_connection) {
  AuctionParticipantState &state = initiator_auction_state_mapping_[initiator_connection];

  AuctionParticipantTaskState best_task_state = state.pickBest();

  // only submitting again if we previously submitted something else
  std::string task_uuid = best_task_state.task->getUuid();
  if (task_uuid != state.previously_submitted) {
    std::string participant_connection = sola_->getConectionString();

    daisi::cpps::mrta::model::Ability participant_ability(
        0, daisi::cpps::mrta::model::LoadCarrier::Types::kNoLoadCarrierType);

    BidSubmission bid_submission(task_uuid, participant_connection, participant_ability,
                                 best_task_state.metrics_composition.value());

    sola_->sendData(serialize(bid_submission), sola::Endpoint(initiator_connection));

    state.previously_submitted = task_uuid;
  }
}

IteratedAuctionDispositionParticipant::AuctionParticipantTaskState::AuctionParticipantTaskState(
    const daisi::material_flow::Task &task)
    : task(std::make_shared<daisi::material_flow::Task>(task)) {}

IteratedAuctionDispositionParticipant::AuctionParticipantState::AuctionParticipantState(
    const std::vector<daisi::material_flow::Task> &tasks) {
  for (const auto &task : tasks) {
    AuctionParticipantTaskState task_state(task);
    task_state_mapping[task.getUuid()] = task_state;
  }
}

IteratedAuctionDispositionParticipant::AuctionParticipantTaskState
IteratedAuctionDispositionParticipant::AuctionParticipantState::pickBest() {
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

void IteratedAuctionDispositionParticipant::AuctionParticipantState::removeTaskState(
    const std::string &task_uuid) {
  task_state_mapping.erase(task_uuid);
}

bool IteratedAuctionDispositionParticipant::AuctionParticipantState::hasOpenTasks() {
  return !task_state_mapping.empty();
}

}  // namespace daisi::cpps::logical