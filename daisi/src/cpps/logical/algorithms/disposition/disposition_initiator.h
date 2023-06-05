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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_DISPOSITION_INITIATOR_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_DISPOSITION_INITIATOR_H_

#include <memory>
#include <variant>

#include "cpps/logical/algorithms/algorithm_interface.h"
#include "cpps/logical/message/auction_based/bid_submission.h"
#include "cpps/logical/message/auction_based/call_for_proposal.h"

namespace daisi::cpps::logical {

// TODO will become abstract class
// for design pattern purpose its currently not abstract
class DispositionInitiator : public AlgorithmInterface {
public:
  explicit DispositionInitiator(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola)
      : AlgorithmInterface(sola){};

  ~DispositionInitiator() = default;

  bool process(const BidSubmission &msg) override { return true; }
  bool process(const CallForProposal &msg) override { return true; }
};

}  // namespace daisi::cpps::logical

#endif
