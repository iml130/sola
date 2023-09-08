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

#ifndef DAISI_SOLA_NS3_SOLA_SCENARIOFILE_H_
#define DAISI_SOLA_NS3_SOLA_SCENARIOFILE_H_

#include <string>
#include <variant>

#include "manager/general_scenariofile.h"

namespace daisi::sola_ns3 {

struct StartSOLA {
  static std::string typeName() { return "start_sola"; }

  uint64_t delay;

  void parse(YAML::Node node);
};

struct SubscribeTopic {
  static std::string typeName() { return "subscribe_topic"; }

  std::string topic;
  uint64_t delay;

  void parse(YAML::Node node);
};

struct Delay {
  static std::string typeName() { return "delay"; }

  uint64_t delay;

  void parse(YAML::Node node);
};

struct Publish {
  static std::string typeName() { return "publish"; }

  std::string topic;
  uint64_t message_size;  // in bytes
  uint64_t node_id;

  void parse(YAML::Node node);
};

struct SolaScenarioSequenceStep {
  std::variant<StartSOLA, SubscribeTopic, Delay, Publish> step;

  void parse(YAML::Node node);
};

struct SolaScenariofile : public GeneralScenariofile {
  explicit SolaScenariofile(const std::string &path_to_file);

  uint32_t number_nodes = 0;

  std::vector<SolaScenarioSequenceStep> scenario_sequence;
};
}  // namespace daisi::sola_ns3
#endif
