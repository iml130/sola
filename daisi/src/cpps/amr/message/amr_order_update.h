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

#ifndef DAISI_CPPS_AMR_MESSAGE_AMR_ORDER_UPDATE_H_
#define DAISI_CPPS_AMR_MESSAGE_AMR_ORDER_UPDATE_H_

#include "cpps/model/order_states.h"
#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {
class AmrOrderUpdate {
public:
  AmrOrderUpdate() = default;
  AmrOrderUpdate(OrderStates state, const util::Position position)
      : state_(state), position_(position) {}
  OrderStates getState() const { return state_; };
  util::Position getPosition() const { return position_; };
  SERIALIZE(state_, position_);

private:
  OrderStates state_ = OrderStates::kInvalid;
  util::Position position_;
};
}  // namespace daisi::cpps

#endif
