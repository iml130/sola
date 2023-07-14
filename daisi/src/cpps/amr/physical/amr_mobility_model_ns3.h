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

#ifndef DAISI_CPPS_AMR_PHYSICAL_AMR_MOBILITY_MODEL_NS3_H_
#define DAISI_CPPS_AMR_PHYSICAL_AMR_MOBILITY_MODEL_NS3_H_

#include <functional>
#include <queue>

#include "cpps/amr/amr_description.h"
#include "cpps/amr/amr_mobility_helper.h"
#include "cpps/amr/amr_topology.h"
#include "cpps/amr/physical/amr_mobility_status.h"
#include "cpps/amr/physical/functionality.h"
#include "ns3/core-module.h"
#include "ns3/mobility-model.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {

class AmrMobilityModelNs3 : public ns3::MobilityModel {
public:
  AmrMobilityModelNs3() { phases_.push_back(AmrMobilityStatus()); }

  ~AmrMobilityModelNs3() override = default;

  /// @brief used by ns3
  static ns3::TypeId GetTypeId();

  ns3::TypeId GetInstanceTypeId() const override;

  void execute(const FunctionalityVariant &functionality, const AmrDescription &description,
               const Topology &topology, FunctionalityDoneCallback notifyDone);

  /// @brief calculates the position of the asset and the angle of the asset
  /// @return position and angle to x-axis
  util::Pose getPose() const;
  util::Acceleration getAcceleration() const;
  util::Position getPosition() const;
  util::Velocity getVelocity() const;

private:
  /// @brief implements ns3::MobilityModel::DoGetPosition
  ns3::Vector DoGetPosition() const override;
  /// @brief implements ns3::MobilityModel::DoSetPosition
  void DoSetPosition(const ns3::Vector &position) override;
  /// @brief implements ns3::MobilityModel::DoGetVelocity
  ns3::Vector DoGetVelocity() const override;

  AmrMobilityStatus getStatus() {
    return AmrMobilityHelper::calculateMobilityStatus(phases_.front(), now());
  }
  void startNextPhase();

  util::Duration now() const { return ns3::Simulator::Now().GetSeconds(); }
  std::deque<AmrMobilityStatus> phases_;
  FunctionalityVariant current_functionality_;
  std::function<void(const FunctionalityVariant &)> notifyDone_;
};

}  // namespace daisi::cpps
#endif
