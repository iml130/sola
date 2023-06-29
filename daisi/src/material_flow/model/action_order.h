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

#ifndef DAISI_MATERIAL_FLOW_ACTION_ORDER_H_
#define DAISI_MATERIAL_FLOW_ACTION_ORDER_H_

#include "action_order_step.h"
#include "solanet/serializer/serialize.h"


namespace daisi::material_flow {

class ActionOrder {
public:
  ActionOrder() = default;
  ActionOrder(const ActionOrderStep &action_order_step);
  ActionOrder(std::string uuid, const ActionOrderStep &action_order_step);

  const std::string &getUuid() const;
  const ActionOrderStep &getActionOrderStep() const;

  bool operator==(const ActionOrder &other) const;

  SERIALIZE(uuid_, action_order_step_);

private:
  std::string uuid_;
  ActionOrderStep action_order_step_;
};

}  // namespace daisi::material_flow

namespace std {

template <> struct hash<daisi::material_flow::ActionOrder> {
  std::size_t operator()(const daisi::material_flow::ActionOrder &order) const {
    string repr = order.getUuid();
    return hash<string>()(repr);
  }
};

}  // namespace std

#endif
