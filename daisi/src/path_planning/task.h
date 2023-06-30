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

#include "cpps/model/order_states.h"
#include "ns3/simulator.h"
#include "ns3/vector.h"
#include "solanet/serializer/serialize.h"

namespace daisi::path_planning {

class Task {
public:
  Task() = default;
  ~Task() = default;

  Task(const ns3::Vector &from, const ns3::Vector &to);
  Task(const std::string &uuid, const ns3::Vector &from, const ns3::Vector &to);

  ns3::Vector getPickupLocation() const;
  ns3::Vector getDeliveryLocation() const;

  cpps::OrderStates getOrderState() const;
  void setOrderState(const cpps::OrderStates &state);

  ns3::Vector getCurrentPosition() const;
  void setCurrentPosition(const ns3::Vector &currentPosition);

  std::string getUuid() const;

  void setConnection(const std::string &connection);
  std::string getConnection() const;

  void setName(const std::string &name);
  std::string getName() const;

  SERIALIZE(uuid_, from_x_, from_y_, to_x_, to_y_, connection_, name_);

private:
  std::string uuid_;
  std::string connection_;
  std::string name_;

  double from_x_ = 0.0, from_y_ = 0.0;
  double to_x_ = 0.0, to_y_ = 0.0;

  ns3::Vector current_pos_;
  cpps::OrderStates order_state_ = cpps::OrderStates::kError;
};
}  // namespace daisi::path_planning

#endif
