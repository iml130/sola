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

#include "action_order_step.h"

namespace daisi::material_flow {

ActionOrderStep::ActionOrderStep(std::string name,
                                 const std::unordered_map<std::string, std::string> &parameters)
    : name_(std::move(name)), parameters_(parameters) {}

const std::string &ActionOrderStep::getName() const { return name_; }

const std::unordered_map<std::string, std::string> &ActionOrderStep::getParameters() const {
  return parameters_;
}

}  // namespace daisi::material_flow
