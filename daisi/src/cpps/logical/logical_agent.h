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

#ifndef DAISI_CPPS_LOGICAL_LOGICAL_AGENT_H_
#define DAISI_CPPS_LOGICAL_LOGICAL_AGENT_H_

#include <memory>
#include <vector>

#include "cpps/logical/algorithms/algorithm_interface.h"
#include "cpps/logical/message/serializer.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::cpps::logical {

class LogicalAgent {
public:
  LogicalAgent(uint32_t device_id);

  virtual ~LogicalAgent() = default;

  /// @brief Forwarding a received message to the appropriate algorithm interface for processing.
  /// @param _message
  void processMessage(const Message &msg);

protected:
  /// @brief Initializing communication via Sola which all logical agents require.
  /// @param _config_file
  /// @param _device_id
  void initCommunication(const std::string &config_file);

  /// @brief Method being called by sola when we receive a 1-to-1 message
  /// @param m received message
  virtual void messageReceiveFunction(const sola::Message &msg) = 0;

  /// @brief Method being called by sola when we receive a message via a topic
  /// @param m received message
  virtual void topicMessageReceiveFunction(const sola::TopicMessage &msg) = 0;

  /// @brief The algorithms which logical messages will be forwarded to for processing.
  std::vector<AlgorithmInterface> algorithms_;

  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;

  std::shared_ptr<sola_ns3::SolaLoggerNs3> logger_;

  std::string uuid_;

  uint32_t device_id_;
};

}  // namespace daisi::cpps::logical

#endif
