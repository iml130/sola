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

#ifndef DAISI_MINHTON_NS3_MINHTON_SCENARIOFILE_HELPER_H_
#define DAISI_MINHTON_NS3_MINHTON_SCENARIOFILE_HELPER_H_

#include <unordered_map>

#include "minhton/utils/algorithm_types_container.h"
#include "minhton/utils/timeout_lengths_container.h"

namespace daisi::minhton_ns3::helper {

minhton::AlgorithmTypesContainer toAlgorithmContainer(
    const std::unordered_map<std::string, std::string> &algorithm_map);

minhton::TimeoutLengthsContainer toTimeoutLengthsContainer(
    const std::unordered_map<std::string, uint64_t> &timeout_map);

}  // namespace daisi::minhton_ns3::helper

#endif
