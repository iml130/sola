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

#include "cpps/logical/logical_agent.h"

namespace daisi::cpps::logical {

class MaterialFlowLogicalAgent : public LogicalAgent {
public:
  MaterialFlowLogicalAgent(uint32_t device_id);

  virtual ~MaterialFlowLogicalAgent() = 0;

  /// @brief Method called by the container on start. Initializing further components which require
  /// the initialization of network components such as Sola.
  virtual void init() = 0;

  /// @brief Adding a material flow in the form of the pure string.
  /// @param mfdl_program
  virtual void addMaterialFlow(std::string mfdl_program) = 0;

  /// @brief Setting a flag that the agent is currently waiting for other processes to finish before
  /// the handling of material flows is possible. An example is the initialization of Sola.
  void setWaitingForStart();

  /// @brief Checking whether the agent is currently handling a material flow or running idle
  /// instead.
  /// @return status whether the agent is busy or not
  virtual bool isBusy() = 0;

protected:
  /// @brief Method being called by sola when we receive a 1-to-1 message. Here, logging of the
  /// messages will be added in comparison to the implementation of the logical agent interface.
  /// @param m received message
  virtual void messageReceiveFunction(const sola::Message &m) override;

  /// @brief Method being called by sola when we receive a message via a topic. Here, logging of the
  /// messages will be added in comparison to the implementation of the logical agent interface
  /// @param m received message
  virtual void topicMessageReceiveFunction(const sola::TopicMessage &m) override;

private:
  /// Simple flag to represent that the agent is still in the initialization process.
  bool waiting_for_start_;
};

}  // namespace daisi::cpps::logical

#endif