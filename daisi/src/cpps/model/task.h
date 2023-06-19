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

#ifndef DAISI_MODEL_TASK_NS3_H_
#define DAISI_MODEL_TASK_NS3_H_

#include <iostream>
#include <optional>

#include "cpps/amr/model/amr_static_ability.h"
#include "cpps/model/agv_fleet.h"
#include "cpps/model/kinematics.h"
#include "cpps/model/order_states.h"
#include "cpps/model/precedence_constraint.h"
#include "cpps/model/time_window.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/vector.h"
#include "solanet/serializer/serialize.h"

namespace daisi::cpps {

class Task {
public:
  Task() = default;
  ~Task() = default;

  Task(const ns3::Vector &from, const ns3::Vector &to, const amr::Ability &ability);
  Task(const std::string &uuid, const ns3::Vector &from, const ns3::Vector &to,
       const amr::Ability &ability);

  ns3::Vector getPickupLocation() const;
  ns3::Vector getDeliveryLocation() const;

  OrderStates getOrderState() const;
  void setOrderState(const OrderStates &state);

  ns3::Vector getCurrentPosition() const;
  void setCurrentPosition(const ns3::Vector &currentPosition);

  std::string getUUID() const;
  amr::Ability getAbilityRequirement() const;

  void setConnection(const std::string &connection);
  std::string getConnection() const;

  void setName(const std::string &name);
  std::string getName() const;

  void setSpecificAbilityRequirement(const amr::Ability &specific_requirement);
  bool hasSpecificAbilityRequirement() const;

  friend std::ostream &operator<<(std::ostream &os, const Task &order);
  friend bool operator<(const Task &lhs, const Task &rhs) { return lhs.uuid_ < rhs.uuid_; };
  friend bool operator<=(const Task &lhs, const Task &rhs) { return lhs.uuid_ <= rhs.uuid_; };
  friend bool operator>(const Task &lhs, const Task &rhs) { return lhs.uuid_ > rhs.uuid_; };
  friend bool operator>=(const Task &lhs, const Task &rhs) { return lhs.uuid_ >= rhs.uuid_; };
  friend bool operator==(const Task &lhs, const Task &rhs) { return lhs.uuid_ == rhs.uuid_; };
  friend bool operator!=(const Task &lhs, const Task &rhs) { return lhs.uuid_ != rhs.uuid_; };

  TimeWindow time_window;
  PrecedenceConstraints precedence_constraints;

  SERIALIZE(uuid_, from_x_, from_y_, to_x_, to_y_, ability_requirement_, possible_kinematics_,
            connection_, name_, time_window, precedence_constraints);

  double getDistance() const;                        // independent of kinematics
  double getDuration(bool execution = false) const;  // only if we have specific ability requirement
  double getFastestDuration(bool execution = false) const;
  double getSlowestDuration(bool execution = false) const;

  void combineConstraints();

private:
  std::string uuid_;
  std::string connection_;
  std::string name_;

  double from_x_ = 0.0, from_y_ = 0.0;
  double to_x_ = 0.0, to_y_ = 0.0;

  ns3::Vector current_pos_;
  OrderStates order_state_ = OrderStates::kError;

  amr::Ability ability_requirement_;
  std::vector<Kinematics> possible_kinematics_;

  std::optional<amr::Ability> specific_ability_;
  std::optional<Kinematics> specific_kinematics_;

  void loadKinematics();
};
}  // namespace daisi::cpps

namespace std {

template <> struct hash<daisi::cpps::Task> {
  std::size_t operator()(const daisi::cpps::Task &v) const {
    using std::hash;
    using std::size_t;
    using std::string;

    string repr = v.getUUID();

    return hash<string>()(repr);
  }
};
}  // namespace std

#endif
