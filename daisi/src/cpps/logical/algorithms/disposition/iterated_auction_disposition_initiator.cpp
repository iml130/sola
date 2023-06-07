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

namespace daisi::cpps::logical {

IteratedAuctionDispositionInitiator::IteratedAuctionDispositionInitiator(
    std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola)
    : DispositionInitiator(sola) {}

bool IteratedAuctionDispositionInitiator::process(const BidSubmission &bid_submission) {
  return true;
}

bool IteratedAuctionDispositionInitiator::process(const WinnerResponse &bid_submission) {
  return true;
}

}  // namespace daisi::cpps::logical