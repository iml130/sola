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

#ifndef DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_TYPES_H_
#define DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_TYPES_H_

#include <unordered_map>

namespace daisi::path_planning::consensus {
enum class ConsensusType { kNone, kPaxos, kCentral };

/**
 * Map consensus name string to consensus type enum
 * @param consensus_type
 * @return
 */
inline ConsensusType consensusTypeFromString(const std::string &consensus_type) {
  static std::unordered_map<std::string, ConsensusType> string_to_consensus_type{
      {"paxos", ConsensusType::kPaxos}, {"central", ConsensusType::kCentral}};
  return string_to_consensus_type.at(consensus_type);
}
}  // namespace daisi::path_planning::consensus

#endif  // DAISI_PATH_PLANNING_CONSENSUS_CONSENSUS_TYPES_H_
