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
#include <optional>

#include "auction_participant_state.h"
#include "cpps/logical/order_management/auction_based_order_management.h"
#include "disposition_participant.h"

namespace daisi::cpps::logical {

/// @brief This class is the counterpart of the IteratedAuctionDispositionInitiator, particpanting
/// in the iterated auction procedure. It must be able to process, IterationNotification, and
/// WinnerNotification messages.
///
/// The participant is responsible for managing states of each auction it is taking place,
/// represented by different initiator connection strings, and storing which bids with insertion
/// infos it has submitted.
class IteratedAuctionDispositionParticipant : public DispositionParticipant {
public:
  explicit IteratedAuctionDispositionParticipant(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola);

  ~IteratedAuctionDispositionParticipant() = default;

  REGISTER_IMPLEMENTATION(CallForProposal);
  REGISTER_IMPLEMENTATION(IterationNotification);
  REGISTER_IMPLEMENTATION(WinnerNotification);

private:
  // initiator connection -> state
  std::unordered_map<std::string, AuctionParticipantState> initiator_auction_state_mapping_;

  std::shared_ptr<AuctionBasedOrderManagement> order_management_;

  void calculateBids(AuctionParticipantState &state);

  void submitBid(const std::string &initiator_connection);

  void clearStoredInformationAfterInsertion();
};

}  // namespace daisi::cpps::logical

#endif
