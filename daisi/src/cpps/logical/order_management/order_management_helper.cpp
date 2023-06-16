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

#include "order_management_helper.h"

using namespace daisi::material_flow;
namespace daisi::cpps::logical {

template <class> inline constexpr bool always_false_v = false;

std::optional<Location> OrderManagementHelper::getEndLocationOfOrder(const Order &order) {
  return std::visit(
      [&](auto &&arg) -> std::optional<Location> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, MoveOrder>) {
          return std::get<MoveOrder>(order).getMoveOrderStep().getLocation();
        } else if constexpr (std::is_same_v<T, TransportOrder>) {
          return std::get<TransportOrder>(order).getDeliveryTransportOrderStep().getLocation();
        } else if constexpr (std::is_same_v<T, ActionOrder>) {
          return std::nullopt;
        } else {
          static_assert(always_false_v<T>, "Order type not handled");
        }
      },
      order);
}
}  // namespace daisi::cpps::logical