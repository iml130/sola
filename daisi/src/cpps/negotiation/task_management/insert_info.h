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

#ifndef DAISI_NEGOTIATION_TASK_MANAGEMENT_INSERT_INFO_NS3_H_
#define DAISI_NEGOTIATION_TASK_MANAGEMENT_INSERT_INFO_NS3_H_

#include <string>
#include <utility>
#include <vector>

#include "cpps/negotiation/utils/simple_temporal_network_components.h"

namespace daisi::cpps {

struct InsertInfo {};

struct ListInsertInfo : public InsertInfo {
  int list_position;
};

struct GraphInsertInfo : public InsertInfo {
  std::vector<int> permutation;
};

struct STNInsertInfo : public InsertInfo {
  STN_Vertex finish_before;
  STN_Vertex start_after;

  double required_finish_time;
  std::vector<std::pair<std::string, double>> ordering_and_start_times;
};

}  // namespace daisi::cpps

#endif
