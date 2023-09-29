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

#ifndef DAISI_MATERIAL_FLOW_MOVE_ORDER_STEP_H_
#define DAISI_MATERIAL_FLOW_MOVE_ORDER_STEP_H_

#include <string>
#include <unordered_map>

#include "location.h"
#include "solanet/serializer/serialize.h"

namespace daisi::material_flow {

struct MoveOrderStep {
  MoveOrderStep() = default;

  MoveOrderStep(std::string name, std::unordered_map<std::string, std::string> parameters,
                Location location);

  const std::string &getName() const;
  const std::unordered_map<std::string, std::string> &getParameters() const;
  const Location &getLocation() const;

  SERIALIZE(name_, parameters_, location_);

private:
  std::string name_;
  std::unordered_map<std::string, std::string> parameters_;
  Location location_;
};

}  // namespace daisi::material_flow

#endif
