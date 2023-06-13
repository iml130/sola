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

#include "cpps/logical/order_management/auction_based_order_management.h"
#include "disposition_participant.h"

namespace daisi::cpps::logical {

class IteratedAuctionDispositionParticipant : public DispositionParticipant {
public:
  struct AuctionParticipantTaskState {
    AuctionParticipantTaskState() = default;

    AuctionParticipantTaskState(const daisi::material_flow::Task &task);

    std::shared_ptr<daisi::material_flow::Task> task = nullptr;

    std::shared_ptr<order_management::AuctionBasedOrderManagement::InsertionPoint> insertion_point =
        nullptr;

    std::optional<order_management::MetricsComposition> metrics_composition = std::nullopt;
  };

  struct AuctionParticipantState {
    AuctionParticipantState() = default;

    AuctionParticipantState(const std::vector<daisi::material_flow::Task> &tasks);

    AuctionParticipantTaskState pickBest();

    void removeTaskState(const std::string &task_uuid);

    bool hasOpenTasks();

    std::unordered_map<std::string, AuctionParticipantTaskState> task_state_mapping;

    std::string previously_submitted;
  };

  explicit IteratedAuctionDispositionParticipant(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola);

  ~IteratedAuctionDispositionParticipant() = default;

  REGISTER_IMPLEMENTATION(CallForProposal);
  REGISTER_IMPLEMENTATION(IterationNotification);
  REGISTER_IMPLEMENTATION(WinnerNotification);

private:
  // initiator connection -> state
  std::unordered_map<std::string, AuctionParticipantState> initiator_auction_state_mapping_;

  std::shared_ptr<order_management::AuctionBasedOrderManagement> order_management_;

  void calculateBids(AuctionParticipantState &state);

  void submitBid(const std::string &initiator_connection);

  void clearStoredInformationAfterInsertion();
};

}  // namespace daisi::cpps::logical

#endif
