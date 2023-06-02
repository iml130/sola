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

#include "cpps/model/agv_fleet.h"

namespace daisi::cpps {

/// as tau(t) of task t in thesis
/// where ability_requirement is h(t)
std::vector<mrta::model::Ability> AGVFleet::getFittingExistingAbilities(
    const mrta::model::Ability &ability_requirement) {
  std::vector<mrta::model::Ability> fitting_abilities;
  for (auto const &[existing_ability, _] : infos_) {
    if (lessOrEqualAbility(ability_requirement, existing_ability)) {
      fitting_abilities.push_back(existing_ability);
    }
  }

  return fitting_abilities;
}

mrta::model::Ability AGVFleet::getClosestExistingAbility(
    const mrta::model::Ability &ability_requirement) {
  /*
  finding minimal element in taut(t), where h(t) = ability_requirement

  m in S is a minimal element if
  if s in S, and s <= m, then necessarily m <= s
  https://en.wikipedia.org/wiki/Maximal_and_minimal_elements

  (a => b) <=> (not a or b)
  a = s <= m
  b = m <= s
  */

  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<mrta::model::Ability> S = getFittingExistingAbilities(ability_requirement);

  if (S.size() == 1) {
    return S[0];
  }

  for (auto const &m : S) {
    bool is_min = true;

    for (auto const &s : S) {
      bool valid = !lessOrEqualAbility(s, m) || lessOrEqualAbility(m, s);
      if (!valid) {
        is_min = false;
      }
    }

    if (is_min) {
      return m;
    }
  }

  throw std::logic_error("Minimal ability element not found. Algorithm wrong. ");
}

/// \mathcal{G} = {G1, G2, ...} in thesis
std::vector<mrta::model::Ability> AGVFleet::getAllExistingAbilities() {
  std::vector<mrta::model::Ability> abilities;
  for (auto const &[ability, _] : infos_) {
    abilities.push_back(ability);
  }
  return abilities;
}

std::string AGVFleet::getTopicForAbility(const mrta::model::Ability &ability) {
  std::ostringstream stream;
  stream << "topic";
  printAbility(stream, ability);
  std::string topic = stream.str();
  return topic;
}

Kinematics AGVFleet::getKinematicsOfAbility(const mrta::model::Ability &ability) {
  for (auto const &[existing_ability, kinematics] : infos_) {
    if (equalAbility(ability, existing_ability)) {
      return kinematics;
    }
  }
  throw std::runtime_error("Kinematics for Ability not found. ");
}

}  // namespace daisi::cpps
