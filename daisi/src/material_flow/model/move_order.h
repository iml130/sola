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

#ifndef DAISI_MATERIAL_FLOW_MOVE_ORDER_H_
#define DAISI_MATERIAL_FLOW_MOVE_ORDER_H_

#include "move_order_step.h"

namespace daisi::material_flow {

class MoveOrder {
public:
  MoveOrder(std::string uuid, const MoveOrderStep &move_order_step);

  const std::string &getUuid() const;
  const MoveOrderStep &getMoveOrderStep() const;

  bool operator==(const MoveOrder &other) const;

private:
  std::string uuid_;
  MoveOrderStep move_order_step_;
};

}  // namespace daisi::material_flow

namespace std {

template <> struct hash<daisi::material_flow::MoveOrder> {
  std::size_t operator()(const daisi::material_flow::MoveOrder &order) const {
    string repr = order.getUuid();
    return hash<string>()(repr);
  }
};

}  // namespace std

#endif
