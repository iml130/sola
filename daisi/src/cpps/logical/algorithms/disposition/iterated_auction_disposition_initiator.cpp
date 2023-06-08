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

#include "ns3/simulator.h"

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
    std::shared_ptr<material_flow::MFDLScheduler> scheduler) {
  if (layered_precedence_graph_) {
    throw std::runtime_error("A material flow is already processed currently. Support of multiple "
                             "is not implemented yet.");
  }

  layered_precedence_graph_ = std::make_unique<LayeredPrecedenceGraph>(scheduler);

  // TODO set sim time for finish time of tasks in free layer
}

daisi::util::Duration IteratedAuctionDispositionInitiator::prepareInteraction() {
  // TODO set available abilities

  uint8_t topic_counter = 0;

  for (const auto &ability : available_abilities_) {
    // TODO get topic for ability
    std::string topic_for_ability = "something";

    ability_topic_mapping_[ability] = topic_for_ability;

    ns3::Simulator::Schedule(ns3::Seconds(delays_.subscribe_topic * topic_counter++),
                             &daisi::sola_ns3::SOLAWrapperNs3::subscribeTopic, sola_.get(),
                             topic_for_ability);
  }

  return delays_.subscribe_topic * topic_counter;
}

void IteratedAuctionDispositionInitiator::startIteration() {}

void IteratedAuctionDispositionInitiator::finishIteration() {}

void IteratedAuctionDispositionInitiator::bidProcessing() {}

void IteratedAuctionDispositionInitiator::winnerResponseProcessing() {}

void IteratedAuctionDispositionInitiator::taskAnnouncement() {}

std::vector<Winner> IteratedAuctionDispositionInitiator::selectWinners() { return {}; }

void IteratedAuctionDispositionInitiator::notifyWinners(const std::vector<Winner> &winners) {}

void IteratedAuctionDispositionInitiator::iterationNotification() {}
void IteratedAuctionDispositionInitiator::renotifyAboutOpenTasks() {}

bool IteratedAuctionDispositionInitiator::process(const BidSubmission &bid_submission) {
  return true;
}

bool IteratedAuctionDispositionInitiator::process(const WinnerResponse &bid_submission) {
  return true;
}

}  // namespace daisi::cpps::logical