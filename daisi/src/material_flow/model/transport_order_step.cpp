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

#include "transport_order_step.h"

namespace daisi::material_flow {

TransportOrderStep::TransportOrderStep(
    std::string name, const std::unordered_map<std::string, std::string> &parameters,
    const Location &location)
    : name_(std::move(name)), parameters_(parameters), location_(location) {}

const std::string &TransportOrderStep::getName() const { return name_; }

const std::unordered_map<std::string, std::string> &TransportOrderStep::getParameters() const {
  return parameters_;
}

const Location &TransportOrderStep::getLocation() const { return location_; }

}  // namespace daisi::material_flow
