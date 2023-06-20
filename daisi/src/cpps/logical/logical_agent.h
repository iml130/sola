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

#include "cpps/common/cpps_logger_ns3.h"
#include "cpps/logical/algorithms/algorithm_config.h"
#include "cpps/logical/algorithms/algorithm_interface.h"
#include "cpps/logical/message/serializer.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::cpps::logical {

class LogicalAgent {
public:
  LogicalAgent(uint32_t device_id, std::shared_ptr<CppsLoggerNs3> logger,
               const AlgorithmConfig &config_algo, bool first_node);

  virtual ~LogicalAgent() = default;

  /// @brief Forwarding a received message to the appropriate algorithm interface for processing.
  /// @param _message
  void processMessage(const Message &msg);

  virtual void start() = 0;

  /// @brief Helper method for event scheduling.
  bool isRunning();

  /// @brief Helper method for event scheduling.
  bool canStop();

  /// @brief Helper method for event scheduling.
  void prepareStop();

protected:
  /// @brief Initializing communication via Sola which all logical agents require.
  /// @param first_node
  void initCommunication();

  /// @brief Initializing algorithm interfaces depending on information from algorithm_config_.
  /// Only a part of the available interfaces might be allowed depending on the type of logical
  /// agent.
  virtual void initAlgorithms() = 0;

  /// @brief Method being called by sola when we receive a 1-to-1 message
  /// @param m received message
  virtual void messageReceiveFunction(const sola::Message &msg) = 0;

  /// @brief Method being called by sola when we receive a message via a topic
  /// @param m received message
  virtual void topicMessageReceiveFunction(const sola::TopicMessage &msg) = 0;

  /// @brief Needed for initialization of Sola.
  uint32_t device_id_;

  /// @brief The algorithms which logical messages will be forwarded to for processing.
  std::vector<std::unique_ptr<AlgorithmInterface>> algorithms_;

  /// @brief Decentralized communication middleware.
  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;

  /// @brief Logging relevant information into Database.
  std::shared_ptr<CppsLoggerNs3> logger_;

  /// @brief Information about which algorithm interfaces will be initialized after the
  /// initialization of Sola is finished.
  const AlgorithmConfig algorithm_config_;

  /// @brief Needed for initialization of Sola.
  std::string uuid_;

  bool first_node_;
};

}  // namespace daisi::cpps::logical

#endif
