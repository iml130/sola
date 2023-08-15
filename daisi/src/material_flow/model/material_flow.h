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

#ifndef DAISI_MATERIAL_FLOW_MATERIAL_FLOW_H_
#define DAISI_MATERIAL_FLOW_MATERIAL_FLOW_H_

#include <variant>
#include <vector>

#include "solanet/serializer/serialize.h"
#include "task.h"

namespace daisi::material_flow {

// TODO just placeholder for now
class MFDLScheduler {
public:
  MFDLScheduler() = default;

  explicit MFDLScheduler(const std::string & /*mfdl_program*/) {}

  SERIALIZE(tasks_)

private:
  std::vector<Task> tasks_;
};

}  // namespace daisi::material_flow

#endif
