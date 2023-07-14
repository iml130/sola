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

#ifndef DAISI_CPPS_AMR_PHYSICAL_AMR_ASSET_CONNECTOR_H_
#define DAISI_CPPS_AMR_PHYSICAL_AMR_ASSET_CONNECTOR_H_

#include <functional>
#include <memory>
#include <random>
#include <vector>

#include "cpps/amr/amr_description.h"
#include "cpps/amr/amr_mobility_helper.h"
#include "cpps/amr/amr_topology.h"
#include "cpps/amr/physical/functionality.h"

namespace daisi::cpps {

// Wrapper class
class AmrAssetConnector {
public:
  // Constructor
  AmrAssetConnector(AmrDescription description, Topology topology);
  AmrAssetConnector();
  ~AmrAssetConnector();

  AmrAssetConnector(AmrAssetConnector &&other);
  AmrAssetConnector(const AmrAssetConnector &) = delete;

  /// @brief start execution, notify whenever a functionality is done
  void execute(const FunctionalityVariant &functionality, FunctionalityDoneCallback notifyDone);
  util::Position getPosition() const;
  util::Pose getPose();
  util::Velocity getVelocity();
  util::Acceleration getAcceleration() const;
  Topology getTopology() const;
  AmrDescription getDescription() const;
  void setTopology(const Topology &topology);

private:
  AmrDescription description_;
  Topology topology_;

  class AmrAssetConnectorImpl;
  std::unique_ptr<AmrAssetConnectorImpl> pimpl_;
};
}  // namespace daisi::cpps
#endif
