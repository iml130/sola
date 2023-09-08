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

#include "sola_scenariofile.h"

#include "utils/daisi_check.h"

namespace daisi::sola_ns3 {

void StartSOLA::parse(YAML::Node node) { SERIALIZE_VAR(delay); }

void SubscribeTopic::parse(YAML::Node node) {
  SERIALIZE_VAR(topic);
  SERIALIZE_VAR(delay);
}

void Delay::parse(YAML::Node node) { SERIALIZE_VAR(delay); }

void Publish::parse(YAML::Node node) {
  SERIALIZE_VAR(topic);
  SERIALIZE_VAR(message_size);
  SERIALIZE_VAR(node_id);
}

void SolaScenarioSequenceStep::parse(YAML::Node node) {
  std::string type;
  SERIALIZE_VAR(type);
  bool handled = false;

  handled |= deserializeIf<StartSOLA>(node, step, type);
  handled |= deserializeIf<SubscribeTopic>(node, step, type);
  handled |= deserializeIf<Delay>(node, step, type);
  handled |= deserializeIf<Publish>(node, step, type);
  DAISI_CHECK(handled, "Invalid ScenarioSequence type");
}

SolaScenariofile::SolaScenariofile(const std::string &path_to_file)
    : GeneralScenariofile(path_to_file) {
  SERIALIZE_VAR(number_nodes);
  SERIALIZE_VAR(scenario_sequence);

  DAISI_CHECK(!scenario_sequence.empty(), "Empty ScenarioSequence");
}
}  // namespace daisi::sola_ns3
