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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_SERIALIZER_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_SERIALIZER_H_

#include <string>
#include <variant>

#include "auction_based/bid_submission.h"
#include "auction_based/call_for_proposal.h"
#include "auction_based/iteration_notification.h"
#include "auction_based/winner_notification.h"
#include "auction_based/winner_response.h"
#include "central_allocation/assignment_notification.h"
#include "central_allocation/assignment_response.h"
#include "central_allocation/status_update.h"
#include "central_allocation/status_update_request.h"

namespace daisi::cpps::logical {

using Message = std::variant<CallForProposal, BidSubmission, IterationNotification,
                             WinnerNotification, WinnerResponse, AssignmentNotification,
                             AssignmentResponse, StatusUpdate, StatusUpdateRequest>;

std::string serialize(const Message &msg);

Message deserialize(const std::string &msg);

}  // namespace daisi::cpps::logical

#endif
