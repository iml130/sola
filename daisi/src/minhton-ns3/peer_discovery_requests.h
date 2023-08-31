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

#ifndef DAISI_MINHTON_NS3_MINHTON_PEER_DISCOVERY_REQUESTS_H_
#define DAISI_MINHTON_NS3_MINHTON_PEER_DISCOVERY_REQUESTS_H_

#include <variant>

#include "manager/scenariofile_component.h"
#include "peer_discovery_general.h"

namespace daisi::minhton_ns3 {

/************************/
/*** VALIDITY THRESHOLD */
/************************/

struct ValidityThreshold {
  std::variant<ConstantTime> threshold;

  void parse(YAML::Node node);
};

/************************/
/******** NODES *********/
/************************/

struct Absolute {
  static std::string typeName() { return "absolute"; }

  uint64_t number;

  void parse(YAML::Node node);
};

struct RandomNode {
  static std::string typeName() { return "random"; }

  double percentage;

  void parse(YAML::Node node);
};

struct Nodes {
  std::variant<RandomNode, Absolute> nodes;

  void parse(YAML::Node node);
};

/************************/
/****** FREQUENCY *******/
/************************/

struct StaticTime {
  static std::string typeName() { return "static"; }

  uint64_t time;

  void parse(YAML::Node node);
};

struct Frequency {
  std::variant<Gaussian, StaticTime> frequency;

  void parse(YAML::Node node);
};

/************************/
/******** DEPTH *********/
/************************/

struct Static {
  static std::string typeName() { return "static"; }

  uint8_t value;

  void parse(YAML::Node node);
};

struct Depth {
  std::variant<Uniform, Static> depth;

  void parse(YAML::Node node);
};

/************************/
/****** REQUESTS ********/
/************************/

struct Requests {
  ValidityThreshold validity_threshold;
  Nodes nodes;
  Frequency frequency;
  Depth depth;
  bool inquire_outdated;
  bool inquire_unknown;

  void parse(YAML::Node node);
};

}  // namespace daisi::minhton_ns3

#endif
