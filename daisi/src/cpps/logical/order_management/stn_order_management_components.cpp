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

#include "stn_order_management_components.h"

using namespace daisi::material_flow;

namespace daisi::cpps::logical {

StnOrderManagementVertex::StnOrderManagementVertex(Order order, const bool is_start)
    : order_(std::move(order)), is_start_(is_start) {}

StnOrderManagementVertex StnOrderManagementVertex::createOrigin() {
  MoveOrder tmp(
      "origin",
      MoveOrderStep("origin", {}, Location("origin", "origin", daisi::util::Position(-1, -1))));
  auto v = StnOrderManagementVertex(tmp, true);
  v.is_origin_ = true;
  return v;
}

const Order &StnOrderManagementVertex::getOrder() const { return order_; }
bool StnOrderManagementVertex::isStart() const { return is_start_; }
bool StnOrderManagementVertex::isOrigin() const { return is_origin_; }

void StnOrderManagementVertex::setLastPosition(const daisi::util::Position &position) {
  if (is_start_) {
    position_ = position;
  }
}

const daisi::util::Position &StnOrderManagementVertex::getLastPosition() const { return position_; }

bool operator==(const StnOrderManagementVertex &v1, const StnOrderManagementVertex &v2) {
  if (v1.is_origin_ && v2.is_origin_) {
    return true;
  }

  if (v1.is_start_ != v2.is_start_) {
    return false;
  }

  if (std::holds_alternative<MoveOrder>(v1.order_) &&
      std::holds_alternative<MoveOrder>(v2.order_)) {
    return std::get<MoveOrder>(v1.order_).getUuid() == std::get<MoveOrder>(v2.order_).getUuid();
  }

  if (std::holds_alternative<ActionOrder>(v1.order_) &&
      std::holds_alternative<ActionOrder>(v2.order_)) {
    return std::get<ActionOrder>(v1.order_).getUuid() == std::get<ActionOrder>(v2.order_).getUuid();
  }

  if (std::holds_alternative<TransportOrder>(v1.order_) &&
      std::holds_alternative<TransportOrder>(v2.order_)) {
    return std::get<TransportOrder>(v1.order_).getUuid() ==
           std::get<TransportOrder>(v2.order_).getUuid();
  }

  return false;
}

}  // namespace daisi::cpps::logical
