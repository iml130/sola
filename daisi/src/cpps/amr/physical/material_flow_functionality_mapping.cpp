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

#include "material_flow_functionality_mapping.h"

namespace daisi::cpps {

// helper constant for the visitor
template <class> inline constexpr bool kAlwaysFalseV = false;

void handleMoveOrder(std::vector<FunctionalityVariant> &functionalities,
                     std::optional<daisi::util::Position> &last_position,
                     const daisi::material_flow::MoveOrder &move_order) {
  if (!last_position.has_value()) {
    throw std::logic_error("MoveOrder can only happen after other operations");
  }

  auto move_to_position = move_order.getMoveOrderStep().getLocation().getPosition();

  functionalities.push_back(MoveTo(move_to_position));
  last_position = move_to_position;
}

void handleActionOrder(std::vector<FunctionalityVariant> &functionalities,
                       std::optional<daisi::util::Position> &last_position,
                       const daisi::material_flow::ActionOrder &action_order) {
  if (!last_position.has_value()) {
    throw std::logic_error("ActionOrder can only happen after other operations");
  }

  auto params = action_order.getActionOrderStep().getParameters();

  if (params.count("load") == 1) {
    functionalities.push_back(Load{last_position.value()});
  } else if (params.count("unload") == 1) {
    functionalities.push_back(Unload{last_position.value()});
  } else {
    throw std::invalid_argument("ActionOrder invalid");
  }
}

void handleTransportOrder(std::vector<FunctionalityVariant> &functionalities,
                          std::optional<daisi::util::Position> &last_position,
                          const daisi::material_flow::TransportOrder &transport_order) {
  auto pickup_to_steps = transport_order.getPickupTransportOrderSteps();
  auto delivery_to_step = transport_order.getDeliveryTransportOrderStep();

  for (auto const &pickup_step : pickup_to_steps) {
    functionalities.push_back(MoveTo(pickup_step.getLocation().getPosition()));
    functionalities.push_back(Load(pickup_step.getLocation().getPosition()));
  }

  functionalities.push_back(MoveTo(delivery_to_step.getLocation().getPosition()));
  functionalities.push_back(Unload(delivery_to_step.getLocation().getPosition()));

  last_position = delivery_to_step.getLocation().getPosition();
}

std::vector<FunctionalityVariant> materialFlowToFunctionalities(
    const std::vector<daisi::material_flow::Order> &orders,
    std::optional<daisi::util::Position> last_position) {
  std::vector<FunctionalityVariant> functionalities;

  for (const auto &order : orders) {
    std::visit(
        [&](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, daisi::material_flow::MoveOrder>) {
            handleMoveOrder(functionalities, last_position,
                            std::get<daisi::material_flow::MoveOrder>(order));
          } else if constexpr (std::is_same_v<T, daisi::material_flow::ActionOrder>) {
            handleActionOrder(functionalities, last_position,
                              std::get<daisi::material_flow::ActionOrder>(order));
          } else if constexpr (std::is_same_v<T, daisi::material_flow::TransportOrder>) {
            handleTransportOrder(functionalities, last_position,
                                 std::get<daisi::material_flow::TransportOrder>(order));
          } else {
            static_assert(kAlwaysFalseV<T>, "Order type not handled");
          }
        },
        order);
  }

  return functionalities;
}

}  // namespace daisi::cpps
