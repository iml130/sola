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

#include "cpps/amr/physical/amr_asset_connector.h"
#include "cpps/amr/physical/amr_mobility_model_ns3.h"
#include "ns3/ptr.h"

namespace daisi::cpps {

/// Global variable to pass ns-3 mobility model to this asset
ns3::Ptr<daisi::cpps::AmrMobilityModelNs3> next_mobility_model;

class AmrAssetConnector::AmrAssetConnectorImpl {
public:
  AmrAssetConnectorImpl() {
    if (!next_mobility_model) {
      throw std::runtime_error("Mobility model empty");
    }

    mobility_model = next_mobility_model;
    next_mobility_model = nullptr;
  }

  virtual ~AmrAssetConnectorImpl() = default;

  ns3::Ptr<AmrMobilityModelNs3> mobility_model;
};

///////////////////////
// AmrAssetConnector //
///////////////////////

AmrAssetConnector::AmrAssetConnector(AmrDescription description, Topology topology)
    : description_(std::move(description)),
      topology_(std::move(topology)),
      pimpl_(std::make_unique<AmrAssetConnectorImpl>()) {}

AmrAssetConnector::~AmrAssetConnector() = default;

AmrAssetConnector::AmrAssetConnector(AmrAssetConnector &&other) noexcept = default;

void AmrAssetConnector::execute(const FunctionalityVariant &functionality,
                                const FunctionalityDoneCallback notifyDone) {
  pimpl_->mobility_model->execute(functionality, description_, topology_, notifyDone);
}

util::Position AmrAssetConnector::getPosition() const {
  return pimpl_->mobility_model->getPosition();
}

util::Pose AmrAssetConnector::getPose() { return pimpl_->mobility_model->getPose(); }

util::Velocity AmrAssetConnector::getVelocity() { return pimpl_->mobility_model->getVelocity(); }

util::Acceleration AmrAssetConnector::getAcceleration() const {
  return pimpl_->mobility_model->getAcceleration();
}

AmrDescription AmrAssetConnector::getDescription() const { return description_; }

Topology AmrAssetConnector::getTopology() const { return topology_; }

void AmrAssetConnector::setTopology(const Topology &topology) { topology_ = topology; }
}  // namespace daisi::cpps
