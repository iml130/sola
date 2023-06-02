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

#ifndef DAISI_NEGOTIATION_PARTICIPANT_ITERATED_AUCTION_PARTICIPANT_PUBSUB_TEPSSI_NS3_H_
#define DAISI_NEGOTIATION_PARTICIPANT_ITERATED_AUCTION_PARTICIPANT_PUBSUB_TEPSSI_NS3_H_

#include <memory.h>

#include "cpps/negotiation/participant/iterated_auction_participant.h"

namespace daisi::cpps {

class IteratedAuctionParticipantPubsubTePSSI : public IteratedAuctionParticipant {
public:
  IteratedAuctionParticipantPubsubTePSSI(const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
                                         const std::unique_ptr<TaskManagement> &task_management);

  ~IteratedAuctionParticipantPubsubTePSSI() override = default;

  void init() override;

private:
  void processCallForProposal(const SSICallForProposal &msg) final;

  const std::string interaction_topic_;
};

}  // namespace daisi::cpps

#endif
