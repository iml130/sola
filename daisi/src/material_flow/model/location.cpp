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

#include "location.h"

namespace daisi::material_flow {

Location::Location(std::string id, std::string type) : id_(std::move(id)), type_(std::move(type)) {
  throw std::runtime_error("Location constructor not implemented yet");
}

Location::Location(std::string id, std::string type, const daisi::util::Position &position)
    : id_(std::move(id)), type_(std::move(type)), position_(position) {}

const std::string &Location::getId() const { return id_; }

const std::string &Location::getType() const { return type_; }

const daisi::util::Position Location::getPosition() const { return position_; }

}  // namespace daisi::material_flow
