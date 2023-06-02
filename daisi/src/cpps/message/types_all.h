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

#ifndef SOLA_NS3_MESSAGE_TYPES_CPPS_H_
#define SOLA_NS3_MESSAGE_TYPES_CPPS_H_

#include <variant>

#include "cpps/message/ssi_call_for_proposal.h"
#include "cpps/message/ssi_iteration_notification.h"
#include "cpps/message/ssi_submission.h"
#include "cpps/message/ssi_winner_notification.h"
#include "cpps/message/ssi_winner_response.h"
#include "cpps/message/task_info.h"
#include "cpps/message/task_update.h"
#include "cpps/model/ability.h"

namespace daisi::cpps {

using Message = std::variant<TaskUpdate, TaskInfo, SSICallForProposal, SSISubmission,
                             SSIIterationNotification, SSIWinnerNotification, SSIWinnerResponse>;

enum MessageTypes : uint8_t {
  kTaskUpdate = 6,
  kSsiCallForProposal = 7,
  kSsiSubmission = 8,
  kSsiIterationNotification = 9,
  kSsiWinnerNotification = 10,
  kSsiWinnerResponse = 11,

  kUnknownNegotiationMsgType = 100,
};

// adding a key-value map to get the string for easier debugging purposes
const std::map<uint32_t, std::string> kMapCppsMessageTypeStrings = {
    {MessageTypes::kTaskUpdate, "TASK_UPDATE"},
    {MessageTypes::kSsiCallForProposal, "CALL_FOR_PROPOSAL"},
    {MessageTypes::kSsiSubmission, "SUBMISSION"},
    {MessageTypes::kSsiIterationNotification, "ITERATION_NOTIFICATION"},
    {MessageTypes::kSsiWinnerNotification, "WINNER_NOTIFICATION"},
    {MessageTypes::kSsiWinnerResponse, "WINNER_RESPONSE"},

    {MessageTypes::kUnknownNegotiationMsgType, "UNKNOWN"},
};

}  // namespace daisi::cpps

#endif
