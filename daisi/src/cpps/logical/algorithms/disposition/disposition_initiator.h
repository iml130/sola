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

#include "../algorithm_interface.h"
#include "material_flow/model/material_flow.h"

namespace daisi::cpps::logical {

class DispositionInitiator : public AlgorithmInterface {
public:
  explicit DispositionInitiator(std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola)
      : AlgorithmInterface(sola){};

  virtual ~DispositionInitiator() = default;

  REQUIRE_IMPLEMENTATION(BidSubmission);
  REQUIRE_IMPLEMENTATION(WinnerResponse);

  virtual void addMaterialFlow(std::shared_ptr<material_flow::MFDLScheduler> scheduler) = 0;
};

}  // namespace daisi::cpps::logical

#endif
