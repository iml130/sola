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

#ifndef DAISI_CPPS_MANAGER_SPAWN_INFO_SCENARIO_H_
#define DAISI_CPPS_MANAGER_SPAWN_INFO_SCENARIO_H_

#include "manager/scenariofile_component.h"
#include "ns3/core-module.h"

namespace daisi::cpps {

struct SpawnDistributionScenario {
  std::string type;

  // absolute
  uint16_t number = 0;

  // relative
  double percentage = 0.0;

  // gaussian
  double mean = 0.0;
  double sigma = 0.0;

  void parse(YAML::Node node) {
    SERIALIZE_VAR(type);

    if (isAbsolute()) {
      SERIALIZE_VAR(number);
    } else if (isRelative()) {
      SERIALIZE_VAR(percentage);
    } else if (isGaussian()) {
      SERIALIZE_VAR(mean);
      SERIALIZE_VAR(sigma);
    } else {
      throw std::runtime_error("Invalid spawn distribution type '" + type + "'.");
    }
  }

  bool isAbsolute() const { return type == "absolute"; }
  bool isRelative() const { return type == "relative"; }
  bool isGaussian() const { return type == "gaussian"; }
};

struct SpawnInfoScenario {
  std::string entity;
  std::string friendly_name;
  ns3::Time start_time;

  SpawnDistributionScenario spawn_distribution;

  void parse(YAML::Node node) {
    SERIALIZE_VAR(entity);
    SERIALIZE_VAR(friendly_name);
    SERIALIZE_NS3_TIME(start_time);

    SERIALIZE_VAR(spawn_distribution);
  }

  bool isAmr() const { return entity == "amr"; }
  bool isMaterialFlow() const { return entity == "mf"; }

  bool operator>(const SpawnInfoScenario &other) const {
    return start_time > other.start_time || friendly_name > other.friendly_name;
  }
};

}  // namespace daisi::cpps

#endif
