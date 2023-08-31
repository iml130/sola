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

#include "peer_discovery_attributes.h"

#include "utils/daisi_check.h"

namespace daisi::minhton_ns3 {

void PresenceBehavior::parse(YAML::Node node) { SERIALIZE_VAR(percentage); }

void ChoiceValue::parse(YAML::Node node) {
  SERIALIZE_VAR(content_int);
  SERIALIZE_VAR(content_float);
  SERIALIZE_VAR(content_bool);
  SERIALIZE_VAR(content_string);
  SERIALIZE_VAR(prob);
}

void Choice::parse(YAML::Node node) { SERIALIZE_VAR(values); }

void Constant::parse(YAML::Node node) {
  SERIALIZE_VAR(value_int);
  SERIALIZE_VAR(value_float);
  SERIALIZE_VAR(value_bool);
  SERIALIZE_VAR(value_string);
}

void ContentBehavior::parse(YAML::Node node) {
  std::string type;
  SERIALIZE_VAR(type);

  bool handled = false;
  handled |= deserializeIf<Choice>(node, content_behavior, type);
  handled |= deserializeIf<Constant>(node, content_behavior, type);
  handled |= deserializeIf<Gaussian>(node, content_behavior, type);
  handled |= deserializeIf<Uniform>(node, content_behavior, type);

  DAISI_CHECK(handled, "Invalid content behavior type");
}

void UpdateBehavior::parse(YAML::Node node) {
  std::string type;
  SERIALIZE_VAR(type);

  bool handled = false;
  handled |= deserializeIf<ConstantTime>(node, update_behavior, type);
  handled |= deserializeIf<Gaussian>(node, update_behavior, type);
  handled |= deserializeIf<Uniform>(node, update_behavior, type);
  handled |= deserializeIf<StaticUpdate>(node, update_behavior, type);

  DAISI_CHECK(handled, "Invalid update behavior type");
}

void PeerDiscoveryAttribute::parse(YAML::Node node) {
  SERIALIZE_VAR(name);
  SERIALIZE_VAR(presence_behavior);

  SERIALIZE_VAR(content_behavior);
  SERIALIZE_VAR(update_behavior);
}

}  // namespace daisi::minhton_ns3
