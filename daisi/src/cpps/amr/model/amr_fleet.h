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

#ifndef DAISI_CPPS_AMR_MODEL_AMR_FLEET_H_
#define DAISI_CPPS_AMR_MODEL_AMR_FLEET_H_

#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "cpps/amr/amr_kinematics.h"
#include "cpps/amr/model/amr_static_ability.h"

namespace daisi::cpps {

class AmrFleet {
public:
  using AmrInfo = std::pair<amr::AmrStaticAbility, AmrKinematics>;

  static AmrFleet &get() { return getImpl(); }

  static void init(const std::vector<AmrInfo> &infos) { getImpl(infos); }

  AmrFleet(AmrFleet const &) = delete;
  void operator=(AmrFleet const &) = delete;

  // -----------------------------------------

  std::vector<amr::AmrStaticAbility> getFittingExistingAbilities(
      const amr::AmrStaticAbility &ability_requirement) const;

  amr::AmrStaticAbility getClosestExistingAbility(
      const amr::AmrStaticAbility &ability_requirement) const;

  std::vector<amr::AmrStaticAbility> getAllExistingAbilities() const;

  std::string getTopicForAbility(const amr::AmrStaticAbility &ability) const;

  AmrKinematics getKinematicsOfAbility(const amr::AmrStaticAbility &ability) const;

  static std::string getDefaultTopic() { return "defaulttopic"; }

private:
  std::vector<AmrInfo> infos_;

  static AmrFleet &getImpl(const std::optional<std::vector<AmrInfo>> &infos = std::nullopt) {
    static AmrFleet instance{infos};
    return instance;
  }

  explicit AmrFleet(const std::optional<std::vector<AmrInfo>> &infos) {
    if (!infos.has_value()) {
      throw std::runtime_error("AmrFleet not initialized");
    }

    infos_ = infos.value();
  }

  AmrFleet() = default;
};

}  // namespace daisi::cpps

#endif
