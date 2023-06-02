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

#ifndef daisi_cpps_amr_physical_material_flow_functionality_mapping_H_
#define daisi_cpps_amr_physical_material_flow_functionality_mapping_H_

#include <optional>

#include "functionality.h"
#include "material_flow/model/task.h"
#include "utils/structure_helpers.h"

namespace daisi::cpps {

std::vector<FunctionalityVariant> materialFlowToFunctionalities(
    const std::vector<daisi::material_flow::Order> &orders,
    std::optional<daisi::util::Position> last_position = std::nullopt);

}  // namespace daisi::cpps
#endif
