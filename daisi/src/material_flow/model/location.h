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

#ifndef DAISI_MATERIAL_FLOW_LOCATION_H_
#define DAISI_MATERIAL_FLOW_LOCATION_H_

#include <string>

#include "solanet/serializer/serialize.h"
#include "utils/structure_helpers.h"

namespace daisi::material_flow {

struct Location {
  Location() = default;

  Location(std::string id, std::string type);
  Location(std::string id, std::string type, const daisi::util::Position &position);

  const std::string &getId() const;
  const std::string &getType() const;
  const daisi::util::Position getPosition() const;

  SERIALIZE(id_, type_, position_)

private:
  std::string id_;
  std::string type_;

  daisi::util::Position position_;
};

}  // namespace daisi::material_flow

#endif
