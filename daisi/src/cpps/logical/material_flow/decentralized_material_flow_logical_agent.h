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

#ifndef DAISI_CPPS_LOGICAL_MATERIAL_FLOW_DECENTRALIZED_MATERIAL_FLOW_LOGICAL_AGENT_H_
#define DAISI_CPPS_LOGICAL_MATERIAL_FLOW_DECENTRALIZED_MATERIAL_FLOW_LOGICAL_AGENT_H_

#include "material_flow_logical_agent.h"

namespace daisi::cpps::logical {

class DecentralizedMaterialFlowLogicalAgent : public MaterialFlowLogicalAgent {
public:
  DecentralizedMaterialFlowLogicalAgent(uint32_t device_id);

  virtual ~DecentralizedMaterialFlowLogicalAgent() = default;

  virtual void init();

  virtual void addMaterialFlow(std::string mfdl_program);

  virtual bool isBusy();

private:
};

}  // namespace daisi::cpps::logical

#endif