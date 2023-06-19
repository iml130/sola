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

  /// @brief To process a CallForProposal we need to initiate AuctionParticipantState for this
  /// auction process, initially calculate possible bids for each open task, and consequently submit
  /// the first bid.
  REGISTER_IMPLEMENTATION(CallForProposal);

  /// @brief When receiving an IterationNotification, it means that another task was assigned to a
  /// different participant. Consequently, we need to remove this open task from the
  /// AuctionParticipantState and submit a new bid.
  REGISTER_IMPLEMENTATION(IterationNotification);

  /// @brief A WinnerNotification means that a task was assigned to us. First, we need to check
  /// whether we can still accept the task. If yes, we add the task to the order management and send
  /// an acceptance WinnerResponse back to the initiator. Otherwise, we must reject.
  REGISTER_IMPLEMENTATION(WinnerNotification);

private:
  /// @brief Storing the auction states for different auction processes with changing initiators.
  /// The initiator connection string is the key. In the AuctionParticipantState we store the open
  /// tasks for each auction process, as well as previously calculated bids and insertion infos.
  std::unordered_map<std::string, AuctionParticipantState> initiator_auction_state_mapping_;

  /// @brief Pointer to the order management of the corresponding AmrLogicalAgent.
  /// We need access to calculate bids and add tasks after receiving WinnerNotifications.
  std::shared_ptr<AuctionBasedOrderManagement> order_management_;

  /// @brief Calculating bids for each open task in a state.
  /// @param state Relevant auction state.
  void calculateBids(const AuctionParticipantState &state);

  /// @brief Picking the best bid from the referring auction state and submitting it.
  /// If we already submitted the same bid previously, the submission is not sent to avoid overhead
  /// traffic.
  /// @param initiator_connection Key to referr to the correct auction participant state.
  void submitBid(const std::string &initiator_connection);
};

}  // namespace daisi::cpps::logical

#endif
