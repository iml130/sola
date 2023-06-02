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

#ifndef DAISI_CPPS_AMR_MESSAGE_AMR_ORDER_INFO_H_
#define DAISI_CPPS_AMR_MESSAGE_AMR_ORDER_INFO_H_

#include "cpps/model/ability.h"
#include "cpps/model/order_states.h"
#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {
class AmrOrderInfo {
public:
  AmrOrderInfo() = default;
  AmrOrderInfo(std::string order_uuid, OrderStates state, const util::Position &load_position,
               const util::Position &unload_position,
               const mrta::model::Ability &ability_requirement)
      : order_uuid_(std::move(order_uuid)),
        state_(state),
        load_position_(load_position),
        unload_position_(unload_position),
        ability_requirement_(ability_requirement) {}

  std::string getOrderUuid() const { return order_uuid_; }
  util::Position getLoadPosition() const { return load_position_; }
  util::Position getUnloadPosition() const { return unload_position_; }
  mrta::model::Ability getAbilityRequirement() const { return ability_requirement_; }
  OrderStates getState() { return state_; }
  SERIALIZE(order_uuid_, state_, load_position_, unload_position_, ability_requirement_);

private:
  std::string order_uuid_;
  OrderStates state_ = OrderStates::kInvalid;
  util::Position load_position_;
  util::Position unload_position_;
  mrta::model::Ability ability_requirement_;
};
}  // namespace daisi::cpps

#endif
