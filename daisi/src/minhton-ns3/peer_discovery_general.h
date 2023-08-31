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

#ifndef DAISI_MINHTON_NS3_MINHTON_PEER_DISCOVERY_GENERAL_H_
#define DAISI_MINHTON_NS3_MINHTON_PEER_DISCOVERY_GENERAL_H_

#include <cstdint>
#include <string>

#include "manager/scenariofile_component.h"

namespace daisi::minhton_ns3 {

struct Gaussian {
  static std::string typeName() { return "gaussian"; }

  uint32_t mean;
  uint32_t sigma;

  void parse(YAML::Node node);
};

struct Uniform {
  static std::string typeName() { return "uniform"; }

  uint32_t min;
  uint32_t max;

  void parse(YAML::Node node);
};

struct ConstantTime {
  static std::string typeName() { return "constant"; }

  uint64_t time;

  void parse(YAML::Node node);
};

}  // namespace daisi::minhton_ns3

#endif
