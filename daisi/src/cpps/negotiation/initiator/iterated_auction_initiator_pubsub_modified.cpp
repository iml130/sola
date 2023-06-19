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

#include "cpps/negotiation/initiator/iterated_auction_initiator_pubsub_modified.h"

#include <iomanip>
#include <sstream>

#include "ns3/simulator.h"
#include "utils/sola_utils.h"

namespace daisi::cpps {

IteratedAuctionInitiatorPubsubModified::IteratedAuctionInitiatorPubsubModified(
    const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
    const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
    const std::shared_ptr<MaterialFlowModel> &material_flow_model)
    : IteratedAuctionInitiator(sola, logger, material_flow_model) {}

void IteratedAuctionInitiatorPubsubModified::prepareInteraction() {
  existing_abilities_ = AGVFleet::get().getAllExistingAbilities();

  uint64_t subscription_delay = 0;

  uint8_t topic_number = 0;
  for (auto const &ability : existing_abilities_) {
    topic_number++;

    std::string topic_for_ability = AGVFleet::get().getTopicForAbility(ability);
    topics_for_abilities_[ability] = topic_for_ability;

    ns3::Simulator::Schedule(ns3::MilliSeconds(subscription_delay),
                             &IteratedAuctionInitiatorPubsubModified::subscribeTopic, this,
                             topic_for_ability, topic_number);
    subscription_delay += pubsub_delays_.subscription;
  }

  assert(subscription_delay <=
         delays_.interaction_preparation);  // Not enough time to subscribe to topics in preparation
}

void IteratedAuctionInitiatorPubsubModified::subscribeTopic(const std::string &topic,
                                                            const uint8_t &topic_number) {
  sola_->subscribeTopic(topic);

  if (topic_number == existing_abilities_.size()) {
    preparation_finished_ = true;
  }
}

void IteratedAuctionInitiatorPubsubModified::taskAnnoucement() {
  // preparing T_{auct}^{G_j} for each G_j
  std::unordered_map<amr::Ability, std::vector<Task>, amr::AbilityHasher>
      tasks_for_existing_abilities;

  for (auto const &task : auctionable_tasks_) {
    // tau(t)
    auto fitting_abilities =
        AGVFleet::get().getFittingExistingAbilities(task.getAbilityRequirement());
    for (auto const &ability_gj : fitting_abilities) {
      tasks_for_existing_abilities[ability_gj].push_back(task);
    }
  }

  std::string initiator_connection = sola_->getConectionString();
  for (auto const &[ability_gj, tasks_tauct_gj] : tasks_for_existing_abilities) {
    std::string topic_for_ability_gj = topics_for_abilities_[ability_gj];

    SSICallForProposal cfp(tasks_tauct_gj, initiator_connection);
    sola_->publishMessage(topic_for_ability_gj, serialize(cfp), cfp.getLoggingContent());
  }
}

std::vector<IteratedAuctionInitiator::Winner>
IteratedAuctionInitiatorPubsubModified::selectWinners() {
  if (bids_.empty()) {
    if (++no_bids_counter_ >= 5) {
      throw std::runtime_error("Received no bids 5 times in a row in iterated auction");
    }
    return {};
  }

  no_bids_counter_ = 0;

  using Bid = IteratedAuctionInitiator::ReceivedBids;
  auto bid_sorting_lambda = [](const Bid &b1, const Bid &b2) {
    if (b1.udims.getUtility() != b2.udims.getUtility()) {
      return b1.udims.getUtility() > b2.udims.getUtility();

      // if utilities are the same, it doesnt matter whether b1 or b2 is selected
      // but for comparability a unique ordering is necessary
    }
    // if abilities are unequal, we can use ability for ordering
    if (!equalAbility(b1.participant_ability, b2.participant_ability)) {
      return lessAbility(b1.participant_ability, b2.participant_ability);
    }

    // if even the ability is equal, at least the connection strings are unique
    return b1.participant_connection > b2.participant_connection;
  };

  std::vector<IteratedAuctionInitiator::Winner> winners;

  auto bids_temp = bids_;

  while (!bids_temp.empty()) {
    std::sort(bids_temp.begin(), bids_temp.end(), bid_sorting_lambda);
    Bid best_bid = bids_temp[0];

    auto it_task = std::find_if(auctionable_tasks_.begin(), auctionable_tasks_.end(),
                                [&](const Task &t) { return t.getUUID() == best_bid.task_uuid; });
    if (it_task != auctionable_tasks_.end()) {
      // updating latest finish times
      latest_finish_times_[it_task->getUUID()] = best_bid.udims.start_time +
                                                 best_bid.udims.travel_to_pickup_duration +
                                                 best_bid.udims.execution_duration;

      winners.push_back(
          IteratedAuctionInitiator::Winner{best_bid.task_uuid, best_bid.participant_connection});

      removeBidsForTask(bids_temp, it_task->getUUID());

      // remove bids from B of agent a with h(t*) <= g(a)
      removeBidsWhichMeetAbilityRequirement(bids_temp, it_task->getAbilityRequirement());

    } else {
      std::cout << "Received Bid for already auctioned task " << best_bid.task_uuid << std::endl;
      bids_temp.erase(bids_temp.begin());
    }
  }

  if (winners.empty()) {
    throw std::logic_error("No winners selected in iterated auction");
  }

  return winners;
}

void IteratedAuctionInitiatorPubsubModified::iterationNotification(
    const std::vector<std::tuple<std::string, Task>> &iteration_info) {
  std::unordered_map<amr::Ability, std::vector<std::string>, amr::AbilityHasher>
      relevant_auctioned_tasks_info_for_abilities;

  for (auto const &[winner_connection, task] : iteration_info) {
    for (auto const &fitting_ability :
         AGVFleet::get().getFittingExistingAbilities(task.getAbilityRequirement())) {
      relevant_auctioned_tasks_info_for_abilities[fitting_ability].push_back(task.getUUID());
    }
  }

  std::string initiator_connection = sola_->getConectionString();

  for (auto const &[existing_ability, relevant_auctioned_tasks_info] :
       relevant_auctioned_tasks_info_for_abilities) {
    std::string topic_for_ability = topics_for_abilities_[existing_ability];
    SSIIterationNotification notification(relevant_auctioned_tasks_info, initiator_connection);

    sola_->publishMessage(topic_for_ability, serialize(notification),
                          notification.getLoggingContent());
  }
}

void IteratedAuctionInitiatorPubsubModified::renotifyAboutOpenTasks() {
  std::unordered_map<amr::Ability, std::vector<std::string>, amr::AbilityHasher>
      relevant_open_tasks_info_for_abilities;

  for (auto const &task : auctionable_tasks_) {
    for (auto const &fitting_ability :
         AGVFleet::get().getFittingExistingAbilities(task.getAbilityRequirement())) {
      relevant_open_tasks_info_for_abilities[fitting_ability].push_back(task.getUUID());
    }
  }

  std::string initiator_connection = sola_->getConectionString();
  for (auto const &[existing_ability, relevant_open_tasks_info] :
       relevant_open_tasks_info_for_abilities) {
    std::string topic_for_ability = topics_for_abilities_[existing_ability];
    SSIIterationNotification notification(relevant_open_tasks_info, initiator_connection);
    sola_->publishMessage(topic_for_ability, serialize(notification),
                          notification.getLoggingContent());
  }
}

}  // namespace daisi::cpps
