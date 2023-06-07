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

namespace daisi::cpps::logical {

IteratedAuctionDispositionParticipant::IteratedAuctionDispositionParticipant(
    std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola)
    : DispositionParticipant(sola) {}

bool IteratedAuctionDispositionParticipant::process(const CallForProposal &call_for_proposal) {
  return true;
}

bool IteratedAuctionDispositionParticipant::process(
    const IterationNotification &call_for_proposal) {
  return true;
}

bool IteratedAuctionDispositionParticipant::process(const WinnerNotification &call_for_proposal) {
  return true;
}

}  // namespace daisi::cpps::logical