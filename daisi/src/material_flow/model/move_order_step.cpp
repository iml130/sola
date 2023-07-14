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

#include "move_order_step.h"

namespace daisi::material_flow {

MoveOrderStep::MoveOrderStep(std::string name,
                             std::unordered_map<std::string, std::string> parameters,
                             Location location)
    : name_(std::move(name)), parameters_(std::move(parameters)), location_(std::move(location)) {}

const std::string &MoveOrderStep::getName() const { return name_; }

const std::unordered_map<std::string, std::string> &MoveOrderStep::getParameters() const {
  return parameters_;
}

const Location &MoveOrderStep::getLocation() const { return location_; }

}  // namespace daisi::material_flow
