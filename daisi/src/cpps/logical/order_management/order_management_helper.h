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

#ifndef DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_ORDER_MANAGEMENT_HELPER_H_
#define DAISI_CPPS_LOGICAL_ORDER_MANAGEMENT_ORDER_MANAGEMENT_HELPER_H_

#include <optional>

#include "material_flow/model/task.h"

namespace daisi::cpps::logical {

class OrderManagementHelper {
public:
  /// @brief get the end location of a given order.
  /// @param order the order to get the end location for
  /// @return the end location or null, if the passed order is an ActionOrder
  static std::optional<daisi::material_flow::Location> getEndLocationOfOrder(
      const daisi::material_flow::Order &order);
};

}  // namespace daisi::cpps::logical

#endif
