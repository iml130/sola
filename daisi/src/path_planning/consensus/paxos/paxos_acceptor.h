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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_ACCEPTOR_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_ACCEPTOR_H_

#include <memory>

#include "path_planning/consensus/paxos/message/accept_message.h"
#include "path_planning/consensus/paxos/message/ok_message.h"
#include "path_planning/consensus/paxos/message/prepare_message.h"
#include "path_planning/consensus/paxos/paxos_data.h"
#include "path_planning/constants.h"

namespace daisi::path_planning::consensus {
//! Implementation for an acceptor in the paxos consensus algorithm
class PaxosAcceptor {
public:
  explicit PaxosAcceptor(std::shared_ptr<PaxosContainer> container);

  void processPrepareMessage(const PrepareMessage &msg);
  void processAcceptMessage(const AcceptMessage &msg);
  void processOKMessage(const OKMessage &msg);

private:
  std::shared_ptr<PaxosContainer> container_;

  //! Current accepted proposal for which we send an OK message
  uint32_t current_accepted_instance_ = UINT32_MAX;
  uint32_t current_accepted_proposal_id_ = UINT32_MAX;
  uint32_t current_accepted_station_ = UINT32_MAX;
  uint32_t remaining_oks_ = 0;  //!< Remaining OKs for the current accepted instance

  //! Data of an OK message with a count how often this message was already received
  struct AlreadyReceivedOKS {
    uint32_t instance = 0;
    uint32_t proposal_id = 0;
    uint32_t station = 0;
    uint32_t already_received = 0;
  };
  std::vector<AlreadyReceivedOKS>
      already_received_;  //!< List of all received OKs instances from other acceptors

  void sendResponseMessage();
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_ACCEPTOR_H_
