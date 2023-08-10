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
#include "cpps/common/cpps_communicator.h"
#include "cpps/common/cpps_logger_ns3.h"
#include "material_flow/model/material_flow.h"

namespace daisi::cpps::logical {

/// @brief Algorithm for disposing tasks from a material flow to fitting AMRs.
/// This algorithm is initiating and coordinating the procedure.
/// There always must be a corresponding derived class from DispositionParticipant.
class DispositionInitiator : public AlgorithmInterface {
public:
  DispositionInitiator(daisi::cpps::common::CppsCommunicatorPtr communicator,
                       std::shared_ptr<CppsLoggerNs3> logger)
      : AlgorithmInterface(communicator), logger_(std::move(logger)){};

  ~DispositionInitiator() override = default;

  /// @brief Adding a material flow whose tasks should be allocated.
  /// @param scheduler MFDL Scheduler
  virtual void addMaterialFlow(std::shared_ptr<material_flow::MFDLScheduler> scheduler) = 0;

  virtual void logMaterialFlowContent(const std::string &material_flow_uuid) = 0;

protected:
  /// @brief For logging material flow tasks and orders
  std::shared_ptr<CppsLoggerNs3> logger_;
};

}  // namespace daisi::cpps::logical

#endif
