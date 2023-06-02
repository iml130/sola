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

#ifndef DAISI_NEGOTIATION_ITERATED_AUCTION_PARTICIPANT_NS3_H_
#define DAISI_NEGOTIATION_ITERATED_AUCTION_PARTICIPANT_NS3_H_

#include <memory>
#include <queue>
#include <unordered_map>

#include "cpps/message/ssi_call_for_proposal.h"
#include "cpps/message/ssi_iteration_notification.h"
#include "cpps/message/ssi_submission.h"
#include "cpps/message/ssi_winner_notification.h"
#include "cpps/message/ssi_winner_response.h"
#include "cpps/model/ability.h"
#include "cpps/model/task.h"
#include "cpps/negotiation/participant/task_allocation_participant.h"
#include "cpps/negotiation/utils/overload.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::cpps {

class IteratedAuctionParticipant : public TaskAllocationParticipant {
public:
  IteratedAuctionParticipant(const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
                             const std::unique_ptr<TaskManagement> &task_management);

  ~IteratedAuctionParticipant() = default;

  void receiveMessage(const Message &msg) override;

protected:
  struct AuctionInfo {
    // keeping track of open task of a MF agent
    std::vector<Task> auctioning_orders;

    // storing previous calculations for the open tasks
    std::unordered_map<std::string, std::tuple<UtilityDimensions, std::shared_ptr<InsertInfo>>>
        previous_calculation_info = {};

    // uuid of bid that we have submitted a bid for previously
    std::string previously_submitted = {};
  };

  // key is MF agent connection
  std::unordered_map<std::string, AuctionInfo> auction_infos_;

  virtual void sendBidSubmission(const SSISubmission &submission);

  virtual void performBidSubmission(const std::string &initiator_connection);

  bool resubmit_bids_ = false;

private:
  virtual void processCallForProposal(const SSICallForProposal &msg);
  void processIterationNotification(const SSIIterationNotification &msg);
  void processWinnerNotification(const SSIWinnerNotification &msg);

  void clearStoredInformationAfterInsertion();
};
}  // namespace daisi::cpps

#endif
