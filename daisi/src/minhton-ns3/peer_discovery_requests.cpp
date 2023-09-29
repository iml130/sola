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

#include "peer_discovery_requests.h"

#include "utils/daisi_check.h"

namespace daisi::minhton_ns3 {

void ValidityThreshold::parse(const YAML::Node &node) {
  std::string type;
  SERIALIZE_VAR(type);

  bool handled = false;
  handled |= deserializeIf<ConstantTime>(node, threshold, type);

  DAISI_CHECK(handled, "Invalid request validty threshold");
}

void Absolute::parse(const YAML::Node &node) { SERIALIZE_VAR(number); }

void RandomNode::parse(const YAML::Node &node) { SERIALIZE_VAR(percentage); }

void Nodes::parse(const YAML::Node &node) {
  std::string type;
  SERIALIZE_VAR(type);

  bool handled = false;
  handled |= deserializeIf<RandomNode>(node, nodes, type);
  handled |= deserializeIf<Absolute>(node, nodes, type);

  DAISI_CHECK(handled, "Invalid request nodes type");
}

void StaticTime::parse(const YAML::Node &node) { SERIALIZE_VAR(time); }

void Frequency::parse(const YAML::Node &node) {
  std::string type;
  SERIALIZE_VAR(type);

  bool handled = false;
  handled |= deserializeIf<Gaussian>(node, frequency, type);
  handled |= deserializeIf<StaticTime>(node, frequency, type);

  DAISI_CHECK(handled, "Invalid request frequency type");
}

void Static::parse(const YAML::Node &node) { SERIALIZE_VAR(value); }

void Depth::parse(const YAML::Node &node) {
  std::string type;
  SERIALIZE_VAR(type);

  bool handled = false;
  handled |= deserializeIf<Uniform>(node, depth, type);
  handled |= deserializeIf<Static>(node, depth, type);

  DAISI_CHECK(handled, "Invalid request depth type");
}

void Requests::parse(const YAML::Node &node) {
  SERIALIZE_VAR(validity_threshold);
  SERIALIZE_VAR(nodes);
  SERIALIZE_VAR(frequency);
  SERIALIZE_VAR(depth);
  SERIALIZE_VAR(inquire_outdated);
  SERIALIZE_VAR(inquire_unknown);
}

}  // namespace daisi::minhton_ns3
