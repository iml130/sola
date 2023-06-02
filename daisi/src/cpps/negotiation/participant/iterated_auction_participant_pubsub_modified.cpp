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

#include "cpps/negotiation/participant/iterated_auction_participant_pubsub_modified.h"

#include "cpps/model/agv_fleet.h"

namespace daisi::cpps {

IteratedAuctionParticipantPubsubModified::IteratedAuctionParticipantPubsubModified(
    const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
    const std::unique_ptr<TaskManagement> &task_management)
    : IteratedAuctionParticipant(sola, task_management) {
  resubmit_bids_ = false;
}

void IteratedAuctionParticipantPubsubModified::init() {
  auto our_ability = task_management_->getAbility();
  auto ability_fit = AGVFleet::get().getClosestExistingAbility(our_ability);
  agv_fleet_topic = AGVFleet::get().getTopicForAbility(ability_fit);

  sola_->subscribeTopic(agv_fleet_topic);
}

}  // namespace daisi::cpps
