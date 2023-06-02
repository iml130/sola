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

#include "transport_order.h"

namespace daisi::material_flow {

TransportOrder::TransportOrder(std::string uuid,
                               const std::vector<TransportOrderStep> &pickup_transport_order_steps,
                               const TransportOrderStep &delivery_transport_order_step)
    : uuid_(std::move(uuid)),
      pickup_transport_order_steps_(pickup_transport_order_steps),
      delivery_transport_order_step_(delivery_transport_order_step) {}

const std::string &TransportOrder::getUuid() const { return uuid_; }

const TransportOrderStep &TransportOrder::getDeliveryTransportOrderStep() const {
  return delivery_transport_order_step_;
}

const std::vector<TransportOrderStep> TransportOrder::getPickupTransportOrderSteps() const {
  return pickup_transport_order_steps_;
}

bool TransportOrder::operator==(const TransportOrder &other) const {
  return uuid_ == other.uuid_;  // TODO steps
}

}  // namespace daisi::material_flow
