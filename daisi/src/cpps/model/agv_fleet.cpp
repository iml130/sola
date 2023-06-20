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
std::vector<amr::AmrStaticAbility> AGVFleet::getFittingExistingAbilities(
    const amr::AmrStaticAbility &ability_requirement) {
  std::vector<amr::AmrStaticAbility> fitting_abilities;
  for (auto const &[existing_ability, _] : infos_) {
    if (ability_requirement <= existing_ability) {
      fitting_abilities.push_back(existing_ability);
    }
  }

  return fitting_abilities;
}

amr::AmrStaticAbility AGVFleet::getClosestExistingAbility(
    const amr::AmrStaticAbility &ability_requirement) {
  /*
  finding minimal element in tau(t), where h(t) = ability_requirement

  m in S is a minimal element if
  if s in S, and s <= m, then necessarily m <= s
  https://en.wikipedia.org/wiki/Maximal_and_minimal_elements

  (a => b) <=> (not a or b)
  a = s <= m
  b = m <= s
  */

  // NOLINTNEXTLINE(readability-identifier-naming)
  std::vector<amr::AmrStaticAbility> S = getFittingExistingAbilities(ability_requirement);

  if (S.size() == 1) {
    return S[0];
  }

  for (auto const &m : S) {
    bool is_min = true;

    for (auto const &s : S) {
      bool a = s <= m;
      bool b = m <= s;

      bool valid = !a || b;
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
std::vector<amr::AmrStaticAbility> AGVFleet::getAllExistingAbilities() {
  std::vector<amr::AmrStaticAbility> abilities;
  for (auto const &[ability, _] : infos_) {
    abilities.push_back(ability);
  }
  return abilities;
}

std::string AGVFleet::getTopicForAbility(const amr::AmrStaticAbility &ability) {
  std::ostringstream stream;
  stream << "topic";
  stream << ability;
  std::string topic = stream.str();
  return topic;
}

Kinematics AGVFleet::getKinematicsOfAbility(const amr::AmrStaticAbility &ability) {
  for (auto const &[existing_ability, kinematics] : infos_) {
    if (ability == existing_ability) {
      return kinematics;
    }
  }
  throw std::runtime_error("Kinematics for Ability not found. ");
}

}  // namespace daisi::cpps
