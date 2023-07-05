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

#ifndef DAISI_CPPS_MANAGER_MATERIAL_FLOW_SCENARIO_H_
#define DAISI_CPPS_MANAGER_MATERIAL_FLOW_SCENARIO_H_

#include "manager/scenariofile_component.h"

namespace daisi::cpps {

struct MaterialFlowDescriptionScenario {
  std::string mfdl_program;
  std::string friendly_name;

  void parse(YAML::Node node) {
    SERIALIZE_VAR(friendly_name);
    SERIALIZE_VAR(mfdl_program);
  }
};

}  // namespace daisi::cpps

#endif
