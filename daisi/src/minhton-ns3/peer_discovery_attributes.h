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

#ifndef DAISI_MINHTON_NS3_MINHTON_PEER_DISCOVERY_ATTRIBUTES_H_
#define DAISI_MINHTON_NS3_MINHTON_PEER_DISCOVERY_ATTRIBUTES_H_

#include <variant>

#include "manager/scenariofile_component.h"
#include "peer_discovery_general.h"

namespace daisi::minhton_ns3 {

/*********************/
/* PRESENCE BEHAVIOR */
/*********************/

struct PresenceBehavior {
  double percentage;

  void parse(const YAML::Node &node);
};

/*******************/
/* CONTENT BEHAVIOR */
/*******************/

struct ChoiceValue {
  // Only exactly one must be valid
  std::optional<int> content_int;
  std::optional<float> content_float;
  std::optional<bool> content_bool;
  std::optional<std::string> content_string;

  double prob;

  void parse(const YAML::Node &node);
};

struct Choice {
  static std::string typeName() { return "choice"; }

  std::vector<ChoiceValue> values;

  void parse(const YAML::Node &node);
};

struct Constant {
  static std::string typeName() { return "constant"; }

  // Only exactly one must be valid
  std::optional<int> value_int;
  std::optional<float> value_float;
  std::optional<bool> value_bool;
  std::optional<std::string> value_string;

  void parse(const YAML::Node &node);
};

struct ContentBehavior {
  std::variant<Choice, Constant, Gaussian, Uniform> content_behavior;

  void parse(const YAML::Node &node);
};

/*******************/
/* UPDATE BEHAVIOR */
/*******************/

struct StaticUpdate {
  static std::string typeName() { return "static"; }

  void parse(YAML::Node) { /* nothing required */
  }
};

struct UpdateBehavior {
  std::variant<ConstantTime, Gaussian, Uniform, StaticUpdate> update_behavior;

  void parse(const YAML::Node &node);
};

/*******************/
/**** ATTRIBUTE ****/
/*******************/

struct PeerDiscoveryAttribute {
  std::string name;
  PresenceBehavior presence_behavior;
  ContentBehavior content_behavior;
  UpdateBehavior update_behavior;

  void parse(const YAML::Node &node);
};

}  // namespace daisi::minhton_ns3

#endif
