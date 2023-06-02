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

#ifndef DAISI_NEGOTIATION_PARTICIPANT_ITERATED_AUCTION_PARTICIPANT_PUBSUB_MODIFIED_NS3_H_
#define DAISI_NEGOTIATION_PARTICIPANT_ITERATED_AUCTION_PARTICIPANT_PUBSUB_MODIFIED_NS3_H_

#include <memory.h>

#include "cpps/negotiation/participant/iterated_auction_participant.h"

namespace daisi::cpps {

class IteratedAuctionParticipantPubsubModified : public IteratedAuctionParticipant {
public:
  IteratedAuctionParticipantPubsubModified(const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
                                           const std::unique_ptr<TaskManagement> &task_management);

  virtual ~IteratedAuctionParticipantPubsubModified() = default;

  virtual void init() override;

  std::string agv_fleet_topic;
};

}  // namespace daisi::cpps

#endif
