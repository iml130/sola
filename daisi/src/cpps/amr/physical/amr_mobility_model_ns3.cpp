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

#include "cpps/amr/physical/amr_mobility_model_ns3.h"

#include <cassert>

namespace daisi::cpps {

NS_OBJECT_ENSURE_REGISTERED(AmrMobilityModelNs3);

void AmrMobilityModelNs3::execute(const FunctionalityVariant &functionality,
                                  const AmrDescription &description, const Topology &topology,
                                  FunctionalityDoneCallback notifyDone) {
  if (phases_.empty()) {
    phases_.emplace_back();
  }
  std::vector<AmrMobilityStatus> new_phases =
      AmrMobilityHelper::calculatePhases(now(), getPose(), functionality, description, topology);
  phases_.insert(phases_.end(), new_phases.begin(), new_phases.end());
  notifyDone_ = std::move(notifyDone);
  current_functionality_ = functionality;
  startNextPhase();
}

util::Position AmrMobilityModelNs3::getPosition() const {
  assert(!phases_.empty());
  return AmrMobilityHelper::calculateMobilityStatus(phases_.front(), now()).position;
}

util::Pose AmrMobilityModelNs3::getPose() const {
  assert(!phases_.empty());
  AmrMobilityStatus current_status =
      AmrMobilityHelper::calculateMobilityStatus(phases_.front(), now());
  util::Velocity v = current_status.velocity;
  double angle = std::atan2(v.x, -v.y);
  return {current_status.position, angle};
}

util::Velocity AmrMobilityModelNs3::getVelocity() const {
  assert(!phases_.empty());
  return AmrMobilityHelper::calculateMobilityStatus(phases_.front(), now()).velocity;
}

util::Acceleration AmrMobilityModelNs3::getAcceleration() const {
  assert(!phases_.empty());
  return AmrMobilityHelper::calculateMobilityStatus(phases_.front(), now()).acceleration;
}

ns3::TypeId AmrMobilityModelNs3::GetTypeId() {
  static ns3::TypeId tid = ns3::TypeId("ns3::AmrMobilityModelNs3")
                               .SetParent<ns3::MobilityModel>()
                               .SetGroupName("Mobility")
                               .AddConstructor<AmrMobilityModelNs3>();
  return tid;
}

ns3::TypeId AmrMobilityModelNs3::GetInstanceTypeId() const {
  return AmrMobilityModelNs3::GetTypeId();
}

inline ns3::Vector AmrMobilityModelNs3::DoGetVelocity() const {
  util::Velocity v = getVelocity();
  return {v.x, v.y, 0};
}

inline ns3::Vector AmrMobilityModelNs3::DoGetPosition() const {
  util::Position p = getPosition();
  return {p.x, p.y, 0};
}

void AmrMobilityModelNs3::DoSetPosition(const ns3::Vector &position) {
  assert(!phases_.empty());
  if (phases_.front().state == AmrMobilityState::kIdle) {
    AmrMobilityStatus idle;
    idle.state = AmrMobilityState::kIdle;
    idle.position = {position.x, position.y};
    idle.velocity = {0, 0};
    idle.acceleration = {0, 0};
    idle.timestamp = now();
    phases_.front() = idle;
  }
  // tell ns3
  NotifyCourseChange();
}

void AmrMobilityModelNs3::startNextPhase() {
  phases_.pop_front();
  assert(!phases_.empty());
  if (phases_.front().state == AmrMobilityState::kIdle) {
    notifyDone_(current_functionality_);
  } else {
    ns3::Simulator::Schedule(ns3::Seconds(phases_.at(1).timestamp - now()),
                             &AmrMobilityModelNs3::startNextPhase, this);
  }
  // tell ns3
  NotifyCourseChange();
}
}  // namespace daisi::cpps
