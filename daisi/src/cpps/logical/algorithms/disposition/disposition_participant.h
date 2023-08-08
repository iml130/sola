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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_DISPOSITION_PARTICIPANT_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_DISPOSITION_DISPOSITION_PARTICIPANT_H_

#include <memory>
#include <variant>

#include "cpps/logical/algorithms/algorithm_interface.h"

namespace daisi::cpps::logical {

/// @brief Algorithm for disposing tasks from a material flow to fitting AMRs.
/// This algorithm is participating in the procedure which is beeing coordinated by an initiator.
/// There always must be a corresponding derived class from DispositionInitiator.
class DispositionParticipant : public AlgorithmInterface {
public:
  explicit DispositionParticipant(std::shared_ptr<SOLACppsWrapper> sola)
      : AlgorithmInterface(sola){};

  ~DispositionParticipant() override = default;
};

}  // namespace daisi::cpps::logical

#endif
