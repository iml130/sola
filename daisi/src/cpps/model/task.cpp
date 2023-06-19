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

#include "cpps/model/task.h"

#include <algorithm>

#include "cpps/common/uuid_generator.h"

namespace daisi::cpps {

std::ostream &operator<<(std::ostream &os, const Task &order) {
  os << "OrderId: {" << order.uuid_ << "}, From: {" << order.getPickupLocation() << "}, To: {"
     << order.getDeliveryLocation() << "}";
  return os;
}

Task::Task(const ns3::Vector &from, const ns3::Vector &to, const amr::AmrStaticAbility &ability)
    : Task(UUIDGenerator::get()(), from, to, ability) {
  loadKinematics();
}

Task::Task(const std::string &uuid, const ns3::Vector &from, const ns3::Vector &to,
           const amr::AmrStaticAbility &ability)
    : uuid_(uuid), current_pos_(from), ability_requirement_(ability) {
  from_x_ = from.x;
  from_y_ = from.y;

  to_x_ = to.x;
  to_y_ = to.y;

  order_state_ = OrderStates::kCreated;

  loadKinematics();
}

OrderStates Task::getOrderState() const { return order_state_; }

void Task::setOrderState(const OrderStates &state) { order_state_ = state; }

ns3::Vector Task::getPickupLocation() const { return ns3::Vector(from_x_, from_y_, 0); }

ns3::Vector Task::getDeliveryLocation() const { return ns3::Vector(to_x_, to_y_, 0); }

ns3::Vector Task::getCurrentPosition() const { return current_pos_; };

void Task::setCurrentPosition(const ns3::Vector &currentPosition) {
  current_pos_ = currentPosition;
}

std::string Task::getUUID() const { return uuid_; }

amr::AmrStaticAbility Task::getAbilityRequirement() const { return ability_requirement_; }

void Task::setConnection(const std::string &connection) { connection_ = connection; }

std::string Task::getConnection() const { return connection_; }

void Task::setName(const std::string &name) { name_ = name; }

std::string Task::getName() const { return name_; }

void Task::setSpecificAbilityRequirement(const amr::AmrStaticAbility &specific_requirement) {
  specific_ability_ = specific_requirement;
  specific_kinematics_ = AGVFleet::get().getKinematicsOfAbility(specific_requirement);
  time_window.setDuration(getDuration(true));
}

bool Task::hasSpecificAbilityRequirement() const { return specific_ability_.has_value(); }

void Task::loadKinematics() {
  auto lc_fits = AGVFleet::get().getFittingExistingAbilities(ability_requirement_);
  for (auto const &fit : lc_fits) {
    possible_kinematics_.push_back(AGVFleet::get().getKinematicsOfAbility(fit));
  }
}

double Task::getDistance() const {
  return CalculateDistance(getPickupLocation(), getDeliveryLocation());
}

double Task::getDuration(bool execution) const {
  if (!specific_kinematics_) {
    throw std::runtime_error("No specific Kinematics given for order");
  }
  return std::get<0>(specific_kinematics_->getStartStopTimeAndDistance(
      getPickupLocation(), getDeliveryLocation(), execution));
}

double Task::getFastestDuration(bool execution) const {
  std::vector<double> durations;
  for (auto const &kine : possible_kinematics_) {
    durations.push_back(std::get<0>(
        kine.getStartStopTimeAndDistance(getPickupLocation(), getDeliveryLocation(), execution)));
  }
  return *std::min_element(durations.begin(), durations.end());
}

double Task::getSlowestDuration(bool execution) const {
  std::vector<double> durations;
  for (auto const &kine : possible_kinematics_) {
    durations.push_back(std::get<0>(
        kine.getStartStopTimeAndDistance(getPickupLocation(), getDeliveryLocation(), execution)));
  }
  return *std::max_element(durations.begin(), durations.end());
}

void Task::combineConstraints() {
  time_window.constraintEarliestStart(precedence_constraints.getEarliestValidStartTime());
}

}  // namespace daisi::cpps
