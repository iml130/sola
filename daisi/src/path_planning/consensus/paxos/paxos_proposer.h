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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_PAXOS_PROPOSER_H_
#define DAISI_PATH_PLANNING_CONSENSUS_PAXOS_PROPOSER_H_

#include <functional>
#include <memory>

#include "path_planning/consensus/paxos/message/ok_message.h"
#include "path_planning/consensus/paxos/message/promise_message.h"
#include "path_planning/consensus/paxos/message/response_message.h"
#include "path_planning/consensus/paxos/paxos_data.h"
#include "path_planning/constants.h"

namespace daisi::path_planning::consensus {
//! Implementation for an proposer in the paxos consensus algorithm
class PaxosProposer {
public:
  explicit PaxosProposer(std::shared_ptr<PaxosContainer> container);

  void findConsensus(const PointTimePairs &points, double seconds_till_start,
                     std::function<void(uint32_t, double)> success_cb,
                     std::function<void(uint32_t)> fail_cb);

  void processPromiseMessage(const PromiseMessage &msg);
  void processOKMessage(const OKMessage &msg);
  void processResponseMessage(const ResponseMessage &msg);

private:
  std::shared_ptr<PaxosContainer> container_;

  enum class Phase { kNone, kPrepare, kAccept, kFinished };

  //! Data for a proposal
  struct Proposal {
    PointTimePairs points;
    double start_time = 0.0;
    uint32_t instance_id = 0;
    uint32_t proposal_id = 0;
    uint32_t needed_for_quorum = 0;
    uint32_t outstanding_oks = 0;
    ns3::EventId timeout_event;
    Phase phase = Phase::kNone;
    std::function<void(uint32_t, double)> success_cb;
    std::function<void(uint32_t)> fail_cb;

    [[nodiscard]] bool initialized() const { return !points.empty(); }
  };

  double calculatePossibleStartTime(const PointTimePairs &points, double seconds_till_start) const;

  void prepareTimeout(uint32_t proposal_id);

  uint32_t remaining_responses_ = 0;  //!< Number of remaining responses for the current proposal
  Proposal current_proposal_;

  std::unordered_map<InstanceID, uint32_t>
      instance_to_proposal_id_;  //!< Next proposal ID to be used for a given instance
};
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_PAXOS_PROPOSER_H_
