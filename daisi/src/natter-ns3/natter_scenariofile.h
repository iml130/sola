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

#ifndef DAISI_NATTER_NS3_NATTER_SCENARIOFILE_H_
#define DAISI_NATTER_NS3_NATTER_SCENARIOFILE_H_

#include <string>
#include <variant>

#include "manager/general_scenariofile.h"
#include "ns3/core-module.h"

namespace daisi::natter_ns3 {

struct Join {
  static std::string typeName() { return "join"; }

  std::string mode;
  std::optional<ns3::Time> delay;

  void parse(YAML::Node node);
};

struct Publish {
  static std::string typeName() { return "publish"; }

  std::string mode;
  uint64_t number = 0;
  std::optional<ns3::Time> delay;
  uint64_t message_size = 0;

  void parse(YAML::Node node);
};

struct NatterScenarioSequenceStep {
  std::variant<Join, Publish> step;

  void parse(YAML::Node node);
};

struct NatterScenariofile : public GeneralScenariofile {
  explicit NatterScenariofile(const std::string &path_to_file);

  uint16_t fanout = 0;
  uint32_t number_nodes = 0;
  std::string mode;

  std::vector<NatterScenarioSequenceStep> scenario_sequence;
};
}  // namespace daisi::natter_ns3
#endif
