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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_ITERATED_AUCTION_DISPOSITION_PARTICIPANT_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_ITERATED_AUCTION_DISPOSITION_PARTICIPANT_H_

#include <memory>
#include <variant>

#include "disposition_participant.h"

namespace daisi::cpps::logical {

class IteratedAuctionDispositionParticipant : public DispositionParticipant {
public:
  explicit IteratedAuctionDispositionParticipant(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola);

  ~IteratedAuctionDispositionParticipant() = default;

  bool process(const BidSubmission &msg) override { return true; }
  bool process(const CallForProposal &msg) override { return true; }
};

}  // namespace daisi::cpps::logical

#endif
