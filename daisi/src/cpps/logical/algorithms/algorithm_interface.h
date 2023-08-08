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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_ALGORITHM_INTERFACE_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_ALGORITHM_INTERFACE_H_

#include <memory>
#include <variant>

#include "cpps/logical/message/auction_based/bid_submission.h"
#include "cpps/logical/message/auction_based/call_for_proposal.h"
#include "cpps/logical/message/auction_based/iteration_notification.h"
#include "cpps/logical/message/auction_based/winner_notification.h"
#include "cpps/logical/message/auction_based/winner_response.h"
#include "cpps/logical/message/central_allocation/assignment_notification.h"
#include "cpps/logical/message/central_allocation/assignment_response.h"
#include "cpps/logical/message/central_allocation/status_update.h"
#include "cpps/logical/message/central_allocation/status_update_request.h"
#include "cpps/logical/message/material_flow_update.h"
#include "cpps/logical/message/serializer.h"
#include "cpps/sola_wrapper.h"

#define REGISTER_LOGICAL_MESSAGE(MessageType) \
  virtual bool process(const MessageType &) { return false; }

#define REQUIRE_IMPLEMENTATION(MessageType) virtual bool process(const MessageType &msg) = 0;

#define REGISTER_IMPLEMENTATION(MessageType) virtual bool process(const MessageType &msg) override;

namespace daisi::cpps::logical {

class AlgorithmInterface {
public:
  explicit AlgorithmInterface(std::shared_ptr<SOLACppsWrapper> sola) : sola_(std::move(sola)){};

  virtual ~AlgorithmInterface() = default;

  REGISTER_LOGICAL_MESSAGE(CallForProposal);
  REGISTER_LOGICAL_MESSAGE(BidSubmission);
  REGISTER_LOGICAL_MESSAGE(IterationNotification);
  REGISTER_LOGICAL_MESSAGE(WinnerNotification);
  REGISTER_LOGICAL_MESSAGE(WinnerResponse);
  REGISTER_LOGICAL_MESSAGE(AssignmentNotification);
  REGISTER_LOGICAL_MESSAGE(AssignmentResponse);
  REGISTER_LOGICAL_MESSAGE(StatusUpdate);
  REGISTER_LOGICAL_MESSAGE(StatusUpdateRequest);
  REGISTER_LOGICAL_MESSAGE(MaterialFlowUpdate);  // TODO Ref #79

protected:
  std::shared_ptr<SOLACppsWrapper> sola_;
};

}  // namespace daisi::cpps::logical

#endif
