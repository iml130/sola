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

#include "minhton_scenariofile.h"

#include "utils/daisi_check.h"

namespace daisi::minhton_ns3 {

void Timeouts::parse(YAML::Node node) {
  SERIALIZE_VAR(bootstrap_response);
  SERIALIZE_VAR(join_response);
  SERIALIZE_VAR(join_accept_ack_response);
  SERIALIZE_VAR(replacement_offer_response);
  SERIALIZE_VAR(replacement_ack_response);
  SERIALIZE_VAR(dsn_aggregation);
  SERIALIZE_VAR(inquiry_aggregation);
}

void Algorithms::parse(YAML::Node node) {
  SERIALIZE_VAR(join);
  SERIALIZE_VAR(leave);
  SERIALIZE_VAR(search_exact);
  SERIALIZE_VAR(response);
  SERIALIZE_VAR(bootstrap);
}

void PeerDiscoveryEnvironment::parse(YAML::Node node) {
  SERIALIZE_VAR(attributes);
  SERIALIZE_VAR(requests);
}

void MinhtonScenarioSequenceStep::parse(YAML::Node node) {
  std::string type;
  SERIALIZE_VAR(type);
  bool handled = false;

  handled |= deserializeIf<JoinOne>(node, step, type);
  handled |= deserializeIf<JoinMany>(node, step, type);
  handled |= deserializeIf<LeaveOne>(node, step, type);
  handled |= deserializeIf<LeaveMany>(node, step, type);
  handled |= deserializeIf<SearchMany>(node, step, type);
  handled |= deserializeIf<SearchAll>(node, step, type);
  handled |= deserializeIf<FailOne>(node, step, type);
  handled |= deserializeIf<FailMany>(node, step, type);
  handled |= deserializeIf<MixedExecution>(node, step, type);
  handled |= deserializeIf<ValidateLeave>(node, step, type);
  handled |= deserializeIf<FindQuery>(node, step, type);
  handled |= deserializeIf<Time>(node, step, type);
  handled |= deserializeIf<StaticBuild>(node, step, type);
  handled |= deserializeIf<RequestCountdown>(node, step, type);

  DAISI_CHECK(handled, "Invalid Scenariosequence type");
}

MinhtonScenariofile::MinhtonScenariofile(const std::string &path_to_file)
    : GeneralScenariofile(path_to_file) {
  SERIALIZE_VAR(fanout);
  SERIALIZE_VAR(timeouts);
  SERIALIZE_VAR(algorithms);
  SERIALIZE_VAR(peer_discovery_environment);
  SERIALIZE_VAR(scenario_sequence);
}

}  // namespace daisi::minhton_ns3
