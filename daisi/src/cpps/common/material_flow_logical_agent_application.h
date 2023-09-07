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

#ifndef DAISI_CPPS_COMMON_MATERIAL_FLOW_LOGICAL_AGENT_APPLICATION_H_
#define DAISI_CPPS_COMMON_MATERIAL_FLOW_LOGICAL_AGENT_APPLICATION_H_

#include <memory>

#include "cpps/logical/material_flow/material_flow_logical_agent.h"
#include "ns3/application.h"

namespace daisi::cpps {

/// Wrapper to run a Material Flow Logical Agent as a ns3::Application
struct MaterialFlowLogicalAgentApplication final : public ns3::Application {
  static ns3::TypeId GetTypeId();

  std::unique_ptr<logical::MaterialFlowLogicalAgent> application;
};

}  // namespace daisi::cpps

#endif
