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

#include <function>
#include <memory>
#include <vector>

#include "daisi/cpps/logical/algorithms/algorithm_interface.h"
#include "sola-ns3/sola_ns3_wrapper.h"

namespace daisi::cpps::logical {

class LogicalAgent {
public:
  virtual ~LogicalAgent() = default;

  void processMessage(const LogicalMessage &_message);

protected:
  void initCommunication(const std::string &_config_file, uint32_t _device_id);

  virtual void messageReceiveFunction(const sola::Message &m) = 0;
  virtual void topicMessageReceiveFunction(const sola::TopicMessage &m) = 0;

  std::vector<AlgorithmInterface> algorithms_;

  // Logger logger_;

  std::shared_ptr<sola_ns3::SOLAWrapperNs3> sola_;

  std::string uuid_;
};

}  // namespace daisi::cpps::logical

#endif
