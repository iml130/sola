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

#ifndef DAISI_CPPS_LOGICAL_ALGORITHMS_ASSIGNMENT_ASSIGNMENT_PARTICIPANT_H_
#define DAISI_CPPS_LOGICAL_ALGORITHMS_ASSIGNMENT_ASSIGNMENT_PARTICIPANT_H_

#include <memory>
#include <variant>

#include "cpps/common/cpps_communicator.h"
#include "cpps/logical/algorithms/algorithm_interface.h"

namespace daisi::cpps::logical {

/// @brief Algorithm for assigning tasks from a material flow to fitting AMRs.
/// This algorithm is participating in the procedure which is beeing coordinated by an initiator.
/// There always must be a corresponding derived class from AssignmentInitiator.
class AssignmentParticipant : public AlgorithmInterface {
public:
  explicit AssignmentParticipant(daisi::cpps::common::CppsCommunicatorPtr communicator)
      : AlgorithmInterface(communicator){};

  ~AssignmentParticipant() override = default;
};

}  // namespace daisi::cpps::logical

#endif
