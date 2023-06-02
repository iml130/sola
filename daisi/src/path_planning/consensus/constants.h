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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CONSTANTS_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CONSTANTS_H_

#include <variant>

#include "path_planning/consensus/central/central_settings.h"
#include "path_planning/consensus/paxos/paxos_settings.h"

namespace daisi::path_planning::consensus {
using ConsensusSettings = std::variant<consensus::PaxosSettings, consensus::CentralSettings>;
}

#endif  // DAISI_PATH_PLANNING_CONSENSUS_CONSTANTS_H_
