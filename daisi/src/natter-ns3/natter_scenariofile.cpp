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

#include "natter_scenariofile.h"

#include "utils/daisi_check.h"

namespace daisi::natter_ns3 {

void Join::parse(YAML::Node node) {
  SERIALIZE_VAR(mode);
  SERIALIZE_VAR(delay);
}

void Publish::parse(YAML::Node node) {
  SERIALIZE_VAR(mode);
  SERIALIZE_VAR(number);
  SERIALIZE_VAR(delay);
  SERIALIZE_VAR(message_size);
}

void NatterScenarioSequenceStep::parse(YAML::Node node) {
  std::string type;
  SERIALIZE_VAR(type);
  bool handled = false;

  handled |= deserializeIf<Join>(node, step, type);
  handled |= deserializeIf<Publish>(node, step, type);
  DAISI_CHECK(handled, "Invalid Scenariosequence type");
}

NatterScenariofile::NatterScenariofile(const std::string &path_to_file)
    : GeneralScenariofile(path_to_file) {
  SERIALIZE_VAR(fanout);
  SERIALIZE_VAR(number_nodes);
  SERIALIZE_VAR(mode);
  SERIALIZE_VAR(scenario_sequence);
}

}  // namespace daisi::natter_ns3
