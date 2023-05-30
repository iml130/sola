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

#ifndef DAISI_CPPS_MATERIAL_FLOW_MATERIAL_FLOW_LOGICAL_AGENT_H_
#define DAISI_CPPS_MATERIAL_FLOW_MATERIAL_FLOW_LOGICAL_AGENT_H_

#include "../logical_agent.h"
#include "daisi/cpps/material_flow/task.h"

namespace daisi::cpps::logical {

class MaterialFlowLogicalAgent : public LogicalAgent {
public:
  MaterialFlowLogicalAgent();

  virtual void init() = 0;

  virtual void addMaterialFlow(const daisi::material_flow::Task &_task) = 0;

  // for interaction with manager
  void setWaitingForStart();
  virtual bool isBusy() = 0;

protected:
  // virtual void messageReceiveFunction(const sola::Message &m);
  // virtual void topicMessageReceiveFunction(const sola::TopicMessage &m);

  bool waiting_for_start_;
};

}  // namespace daisi::cpps::logical

#endif
