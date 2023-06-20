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

#include <memory>

#include "cpps/common/cpps_logger_ns3.h"
#include "cpps/message/serializer.h"
#include "cpps/model/material_flow_model.h"
#include "cpps/model/task.h"
#include "sola-ns3/sola_ns3_wrapper.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {

class TaskAllocationInitiator {
public:
  TaskAllocationInitiator(const std::shared_ptr<sola_ns3::SOLAWrapperNs3> &sola,
                          const std::shared_ptr<daisi::cpps::CppsLoggerNs3> &logger,
                          const std::shared_ptr<MaterialFlowModel> &material_flow_model)
      : sola_(sola), logger_(logger), material_flow_model_(material_flow_model) {}

  virtual void receiveMessage(const Message &msg) = 0;

  virtual ~TaskAllocationInitiator() = default;

  virtual void init() = 0;

protected:
  const std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;
  const std::shared_ptr<CppsLoggerNs3> logger_;

  const std::shared_ptr<MaterialFlowModel> &material_flow_model_;
};
}  // namespace daisi::cpps
