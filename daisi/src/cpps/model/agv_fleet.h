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

#ifndef DAISI_MODEL_AGV_FLEET_NS3_H_
#define DAISI_MODEL_AGV_FLEET_NS3_H_

#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "cpps/model/ability.h"
#include "cpps/model/kinematics.h"

namespace daisi::cpps {

class AGVFleet {
  using AGVInfo = std::pair<amr::Ability, Kinematics>;

private:
  std::vector<AGVInfo> infos_;

  static AGVFleet &getImpl(const std::optional<std::vector<AGVInfo>> &infos = std::nullopt) {
    static AGVFleet instance{infos};
    return instance;
  }

  explicit AGVFleet(const std::optional<std::vector<AGVInfo>> &infos) {
    if (!infos.has_value()) {
      throw std::runtime_error("AGVFleet not initialized");
    }

    infos_ = infos.value();
  }

  AGVFleet() = default;

public:
  static AGVFleet &get() { return getImpl(); }

  static void init(const std::vector<AGVInfo> &infos) { getImpl(infos); }

  AGVFleet(AGVFleet const &) = delete;
  void operator=(AGVFleet const &) = delete;

  // -----------------------------------------

  std::vector<amr::Ability> getFittingExistingAbilities(const amr::Ability &ability_requirement);

  amr::Ability getClosestExistingAbility(const amr::Ability &ability_requirement);

  std::vector<amr::Ability> getAllExistingAbilities();

  std::string getTopicForAbility(const amr::Ability &ability);

  Kinematics getKinematicsOfAbility(const amr::Ability &ability);

  static std::string getDefaultTopic() { return "defaulttopic"; }
};

}  // namespace daisi::cpps

#endif
