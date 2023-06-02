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

#include "cpps/negotiation/participant/iterated_auction_participant_pubsub_tepssi.h"

namespace daisi::cpps {

IteratedAuctionParticipantPubsubTePSSI::IteratedAuctionParticipantPubsubTePSSI(
    const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
    const std::unique_ptr<TaskManagement> &task_management)
    : IteratedAuctionParticipant(sola, task_management),
      interaction_topic_(AGVFleet::getDefaultTopic()) {
  resubmit_bids_ = true;
}

void IteratedAuctionParticipantPubsubTePSSI::init() { sola_->subscribeTopic(interaction_topic_); }

void IteratedAuctionParticipantPubsubTePSSI::processCallForProposal(const SSICallForProposal &msg) {
  std::string initiator_connection = msg.getInitiatorConnection();

  std::vector<Task> relevant_tasks;
  for (auto const &task : msg.getTasks()) {
    if (lessOrEqualAbility(task.getAbilityRequirement(), task_management_->getAbility())) {
      relevant_tasks.push_back(task);
    }
  }

  auction_infos_[initiator_connection] = AuctionInfo{relevant_tasks};
  performBidSubmission(initiator_connection);
}

}  // namespace daisi::cpps
