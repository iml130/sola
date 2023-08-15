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

#ifndef DAISI_CPPS_LOGICAL_MESSAGE_MATERIAL_FLOW_UPDATE_H_
#define DAISI_CPPS_LOGICAL_MESSAGE_MATERIAL_FLOW_UPDATE_H_

#include <cstdint>
#include <string>

#include "cpps/model/order_states.h"
#include "material_flow/model/task.h"
#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps::logical {

struct MaterialFlowUpdate {
  std::string amr_uuid;
  OrderStates order_state;
  material_flow::Task task;
  uint8_t order_index = 0;
  util::Position position;

  SERIALIZE(amr_uuid, order_state, task, order_index, position)
};
}  // namespace daisi::cpps::logical

#endif
