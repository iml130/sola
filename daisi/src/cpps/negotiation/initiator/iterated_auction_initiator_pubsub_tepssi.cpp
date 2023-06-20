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

#include "cpps/negotiation/initiator/iterated_auction_initiator_pubsub_tepssi.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "ns3/simulator.h"
#include "utils/sola_utils.h"

namespace daisi::cpps {

IteratedAuctionInitiatorPubsubTePSSI::IteratedAuctionInitiatorPubsubTePSSI(
    const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
    const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
    const std::shared_ptr<MaterialFlowModel> &material_flow_model)
    : IteratedAuctionInitiator(sola, logger, material_flow_model),
      interaction_topic_(AGVFleet::getDefaultTopic()) {}

void IteratedAuctionInitiatorPubsubTePSSI::prepareInteraction() {
  sola_->subscribeTopic(interaction_topic_);
  preparation_finished_ = true;
}

void IteratedAuctionInitiatorPubsubTePSSI::taskAnnoucement() {
  std::string initiator_connection = sola_->getConectionString();
  SSICallForProposal cfp(auctionable_tasks_, initiator_connection);

  sola_->publishMessage(interaction_topic_, serialize(cfp), cfp.getLoggingContent());
}

std::vector<IteratedAuctionInitiator::Winner>
IteratedAuctionInitiatorPubsubTePSSI::selectWinners() {
  if (bids_.empty()) {
    if (++no_bids_counter_ >= 5) {
      throw std::runtime_error("Received no bids 5 times in a row in iterated auction");
    }
    return {};
  }

  no_bids_counter_ = 0;

  if (sola_->getConectionString() == "1.1.0.13:2000") {
    std::string t;
  }

  using Bid = IteratedAuctionInitiator::ReceivedBids;
  auto bid_sorting_lambda = [](const Bid &b1, const Bid &b2) {
    if (b1.udims.getUtility() != b2.udims.getUtility()) {
      return b1.udims.getUtility() > b2.udims.getUtility();

      // if utilities are the same, it doesnt matter whether b1 or b2 is selected
      // but for comparability a unique ordering is necessary
    }

    // if abilities are unequal, we can use ability for ordering
    if (b1.participant_ability != b2.participant_ability) {
      return b1.participant_ability < b2.participant_ability;
    }

    // if even the ability is equal, at least the connection strings are unique
    return b1.participant_connection > b2.participant_connection;
  };

  std::sort(bids_.begin(), bids_.end(), bid_sorting_lambda);
  bids_.erase(std::unique(bids_.begin(), bids_.end()), bids_.end());

  auto bids_temp = bids_;

  while (!bids_temp.empty()) {
    std::sort(bids_temp.begin(), bids_temp.end(), bid_sorting_lambda);
    Bid best_bid = bids_temp[0];

    auto it_task = std::find_if(auctionable_tasks_.begin(), auctionable_tasks_.end(),
                                [&](const Task &t) { return t.getUUID() == best_bid.task_uuid; });
    if (it_task != auctionable_tasks_.end()) {
      IteratedAuctionInitiator::Winner winner{best_bid.task_uuid, best_bid.participant_connection};

      // updating latest finish times
      latest_finish_times_[it_task->getUUID()] = best_bid.udims.start_time +
                                                 best_bid.udims.travel_to_pickup_duration +
                                                 best_bid.udims.execution_duration;

      return {winner};
    }

    std::cout << "Received Bid for already auctioned task " << best_bid.task_uuid << std::endl;
    bids_temp.erase(bids_temp.begin());
  }

  return {};
}

void IteratedAuctionInitiatorPubsubTePSSI::iterationNotification(
    const std::vector<std::tuple<std::string, Task>> &iteration_info) {
  std::vector<std::string> auctioned_tasks;
  for (auto const &[winner_connection, task] : iteration_info) {
    auctioned_tasks.push_back(task.getUUID());
  }

  SSIIterationNotification notification(auctioned_tasks, sola_->getConectionString());
  sola_->publishMessage(interaction_topic_, serialize(notification),
                        notification.getLoggingContent());
}

void IteratedAuctionInitiatorPubsubTePSSI::renotifyAboutOpenTasks() {
  std::cout << "renotifyAboutOpenTasks " << sola_->getConectionString() << std::endl;

  std::vector<std::string> auctionable_tasks;
  for (auto const &task : auctionable_tasks_) {
    auctionable_tasks.push_back(task.getUUID());
  }

  SSIIterationNotification notification(auctionable_tasks, sola_->getConectionString());
  sola_->publishMessage(interaction_topic_, serialize(notification),
                        notification.getLoggingContent());
}

}  // namespace daisi::cpps
