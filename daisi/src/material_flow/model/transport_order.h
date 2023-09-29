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

#ifndef DAISI_MATERIAL_FLOW_TRANSPORT_ORDER_H_
#define DAISI_MATERIAL_FLOW_TRANSPORT_ORDER_H_

#include <vector>

#include "cpps/model/order_states.h"
#include "solanet/serializer/serialize.h"
#include "transport_order_step.h"

namespace daisi::material_flow {

class TransportOrder {
public:
  TransportOrder() = default;

  TransportOrder(std::vector<TransportOrderStep> pickup_transport_order_steps,
                 TransportOrderStep delivery_transport_order_step);

  TransportOrder(std::string uuid, std::vector<TransportOrderStep> pickup_transport_order_steps,
                 TransportOrderStep delivery_transport_order_step);

  const std::string &getUuid() const;

  const TransportOrderStep &getDeliveryTransportOrderStep() const;
  std::vector<TransportOrderStep> getPickupTransportOrderSteps() const;

  bool operator==(const TransportOrder &other) const;

  void setOrderState(cpps::OrderStates state) { state_ = state; }
  cpps::OrderStates getOrderState() const { return state_; }

  SERIALIZE(uuid_, pickup_transport_order_steps_, delivery_transport_order_step_);

private:
  cpps::OrderStates state_ = cpps::OrderStates::kInvalid;

  std::string uuid_;

  std::vector<TransportOrderStep> pickup_transport_order_steps_;
  TransportOrderStep delivery_transport_order_step_;
};

}  // namespace daisi::material_flow

namespace std {

template <> struct hash<daisi::material_flow::TransportOrder> {
  std::size_t operator()(const daisi::material_flow::TransportOrder &order) const {
    string repr = order.getUuid();
    return hash<string>()(repr);
  }
};

}  // namespace std

#endif
