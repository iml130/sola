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

#ifndef DAISI_CPPS_AMR_PHYSICAL_AMR_ORDER_H_
#define DAISI_CPPS_AMR_PHYSICAL_AMR_ORDER_H_

#include "cpps/model/order_states.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {
struct AmrOrder {
  AmrOrder() = default;
  AmrOrder(std::string uuid, const util::Position &pick_up_location,
           const util::Position &delivery_location)
      : pick_up_location(pick_up_location),
        delivery_location(delivery_location),
        order_state(OrderStates::kCreated),
        uuid(std::move(uuid)) {}
  util::Position pick_up_location;
  util::Position delivery_location;
  OrderStates order_state = OrderStates::kInvalid;
  std::string uuid;
};
}  // namespace daisi::cpps

#endif
