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

#include "iterated_auction_disposition_initiator.h"

#include "cpps/model/agv_fleet.h"
#include "ns3/simulator.h"

using namespace daisi::material_flow;
using namespace daisi::cpps::mrta::model;

namespace daisi::cpps::logical {

IteratedAuctionDispositionInitiator::IteratedAuctionDispositionInitiator(
    std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola)
    : DispositionInitiator(sola) {
  // assuming that sola is fully initialized at this point

  auto preparation_duration = prepareInteraction();

  ns3::Simulator::Schedule(ns3::Seconds(preparation_duration),
                           &IteratedAuctionDispositionInitiator::startIteration, this);
}

void IteratedAuctionDispositionInitiator::addMaterialFlow(
    std::shared_ptr<MFDLScheduler> scheduler) {
  if (layered_precedence_graph_) {
    throw std::runtime_error("A material flow is already processed currently. Support of multiple "
                             "is not implemented yet.");
  }

  layered_precedence_graph_ = std::make_shared<LayeredPrecedenceGraph>(scheduler);

  auto sim_time = (double)ns3::Simulator::Now().GetMilliSeconds();
  for (const auto &task : layered_precedence_graph_->getAuctionableTasks()) {
    layered_precedence_graph_->setEarliestValidStartTime(task.getUuid(), sim_time);
  }
}

daisi::util::Duration IteratedAuctionDispositionInitiator::prepareInteraction() {
  available_abilities_ = AGVFleet::get().getAllExistingAbilities();

  uint8_t topic_counter = 0;

  for (const auto &ability : available_abilities_) {
    std::string topic_for_ability = AGVFleet::get().getTopicForAbility(ability);

    ability_topic_mapping_[ability] = topic_for_ability;

    ns3::Simulator::Schedule(ns3::Seconds(delays_.subscribe_topic * topic_counter++),
                             &daisi::sola_ns3::SOLAWrapperNs3::subscribeTopic, sola_.get(),
                             topic_for_ability);
  }

  return delays_.subscribe_topic * topic_counter;
}

void IteratedAuctionDispositionInitiator::startIteration() {
  if (layered_precedence_graph_->areAllTasksScheduled()) {
    return;
  }

  auto auctionable_tasks = layered_precedence_graph_->getAuctionableTasks();

  taskAnnouncement();

  // starting (while T_{auct} != emptyset) loop
  // waiting to receive bids
  ns3::Simulator::Schedule(ns3::Seconds(delays_.waiting_to_receive_bids),
                           &IteratedAuctionDispositionInitiator::bidProcessing, this);
}

void IteratedAuctionDispositionInitiator::bidProcessing() {
  // receiving bids in the meantime
  auction_initiator_state_->countBidSubmissionProcessing();
  auto winners = auction_initiator_state_->selectWinner();
  if (!winners.empty()) {
    notifyWinners(winners);

    // waiting to receive winner responses
    ns3::Simulator::Schedule(ns3::Seconds(delays_.waiting_to_receive_winner_responses),
                             &IteratedAuctionDispositionInitiator::winnerResponseProcessing, this);
  } else {
    // renotify
    iterationNotification(layered_precedence_graph_->getAuctionableTasks());

    ns3::Simulator::Schedule(ns3::Seconds(delays_.waiting_to_receive_bids),
                             &IteratedAuctionDispositionInitiator::bidProcessing, this);
  }
}

void IteratedAuctionDispositionInitiator::notifyWinners(
    const std::vector<AuctionInitiatorState::Winner> &winners) {
  auction_initiator_state_->clearWinnerAcceptions();
  auto initiator_connection = sola_->getConectionString();

  for (const auto &winner : winners) {
    WinnerNotification notification(winner.task_uuid, initiator_connection,
                                    winner.latest_finish_time);

    sola_->sendData(serialize(notification), sola::Endpoint(winner.winner_connection));
  }
}

void IteratedAuctionDispositionInitiator::finishIteration() {
  // Moving all free tasks from the free layer to the scheduled layer
  // and updating the other layers accordingly
  layered_precedence_graph_->next();

  auction_initiator_state_->clearIterationInfo();

  startIteration();
}

void IteratedAuctionDispositionInitiator::taskAnnouncement() {
  auto initiator_connection = sola_->getConectionString();
  auto task_ability_mapping =
      getTaskAbilityMapping(layered_precedence_graph_->getAuctionableTasks());

  for (const auto &[ability, tasks] : task_ability_mapping) {
    auto topic = ability_topic_mapping_[ability];

    CallForProposal cfp(initiator_connection, tasks);
    sola_->publishMessage(topic, serialize(cfp), "TODO log cfp");
  }
}

std::unordered_map<Ability, std::vector<material_flow::Task>, AbilityHasher>
IteratedAuctionDispositionInitiator::getTaskAbilityMapping(
    const std::vector<material_flow::Task> &tasks) {
  std::unordered_map<Ability, std::vector<material_flow::Task>, AbilityHasher> task_ability_mapping;

  for (const auto &task : tasks) {
    auto fitting_abilities =
        AGVFleet::get().getFittingExistingAbilities(task.getAbilityRequirement());
    for (const auto &ability : fitting_abilities) {
      task_ability_mapping[ability].push_back(task);
    }
  }

  return task_ability_mapping;
}

void IteratedAuctionDispositionInitiator::winnerResponseProcessing() {
  auction_initiator_state_->countWinnerResponseProcessing();

  auto auctioned_tasks = auction_initiator_state_->processWinnerAcceptions();

  iterationNotification(auctioned_tasks);

  if (layered_precedence_graph_->areAllFreeTasksScheduled()) {
    finishIteration();
  } else {
    // continue (while T_{auct} != emptyset) loop

    // waiting to receive bids
    ns3::Simulator::Schedule(ns3::Seconds(delays_.waiting_to_receive_bids),
                             &IteratedAuctionDispositionInitiator::bidProcessing, this);
  }
}

void IteratedAuctionDispositionInitiator::iterationNotification(
    const std::vector<material_flow::Task> &tasks) {
  auto initiator_connection = sola_->getConectionString();
  auto task_ability_mapping = getTaskAbilityMapping(tasks);

  for (const auto &[ability, tasks_for_ability] : task_ability_mapping) {
    auto topic = ability_topic_mapping_[ability];

    std::vector<std::string> task_uuids;
    std::transform(tasks_for_ability.begin(), tasks_for_ability.end(),
                   std::back_inserter(task_uuids),
                   [&](const auto &task) { return task.getUuid(); });

    IterationNotification notification(initiator_connection, task_uuids);
    sola_->publishMessage(topic, serialize(notification), "TODO log iteration notification");
  }
}

bool IteratedAuctionDispositionInitiator::process(const BidSubmission &bid_submission) {
  auction_initiator_state_->addBidSubmission(bid_submission);
  return true;
}

bool IteratedAuctionDispositionInitiator::process(const WinnerResponse &winner_response) {
  auction_initiator_state_->addWinnerResponse(winner_response);
  return true;
}

}  // namespace daisi::cpps::logical