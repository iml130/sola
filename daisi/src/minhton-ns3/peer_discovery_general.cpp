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

#include "peer_discovery_general.h"

namespace daisi::minhton_ns3 {

void Gaussian::parse(YAML::Node node) {
  SERIALIZE_VAR(mean);
  SERIALIZE_VAR(sigma);
}

void Uniform::parse(YAML::Node node) {
  SERIALIZE_VAR(min);
  SERIALIZE_VAR(max);
}

void ConstantTime::parse(YAML::Node node) { SERIALIZE_VAR(time); }

}  // namespace daisi::minhton_ns3
